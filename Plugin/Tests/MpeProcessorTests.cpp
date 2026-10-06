/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <vector>

using WireMessage = std::vector<uint8_t>;
using WireMessages = std::vector<WireMessage>;

// Ports are closed before constructing a processor: production shutdown drain
// cannot accidentally send test notes or Store commands to real hardware.
struct MidiRoutingTestAccess {
    static void offline(Pfm2MidiDevice& d) { d.stopThread(2000); d.resetDevices(); }
    static WireMessages drain(Pfm2MidiDevice& d) {
        WireMessages result;
        d.drainOutputQueue([&](const Pfm2MidiDevice::OutputEvent& e) {
            d.emitOutputEvent(e,[&](const uint8_t* data,int size){result.emplace_back(data,data+size);});
        });
        return result;
    }
    static void channel(Pfm2AudioProcessor& p,int ch) {p.setControlChannel(ch);}
    static void async(Pfm2AudioProcessor& p) {p.handleAsyncUpdate();}
    static void supported(Pfm2AudioProcessor& p) {
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_PROTOCOL_VERSION,PREENFM_EDITOR_PROTOCOL_VERSION);
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_CAPABILITIES,
            PREENFM_EDITOR_CAPABILITY_STORE|PREENFM_EDITOR_CAPABILITY_POSITION_QUERY);
    }
    static void position(Pfm2AudioProcessor& p,int bank,int preset) {
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_POSITION_BANKTYPE,0);
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_POSITION_BANK,bank);
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_POSITION_PRESET,preset);
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_POSITION_VALID,1);
    }
    static void storeDone(Pfm2AudioProcessor& p,int target) {
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_STORE_TARGET,target);
        p.handleEditorProtocolResponse(PREENFM_EDITOR_RSP_STORE_STATUS,0);
    }
    static void pull(Pfm2AudioProcessor& p) {p.requestCurrentHardwarePreset();}
};

namespace {
int checks=0,failures=0;
void check(bool pass,const char* label) {++checks;if(!pass){++failures;std::cerr<<"FAIL: "<<label<<'\n';}}
void process(Pfm2AudioProcessor& p,const WireMessages& input) {
    juce::AudioBuffer<float> audio(2,64);
    juce::MidiBuffer midi;
    int sample=0;
    for(const auto& m:input)midi.addEvent(m.data(),static_cast<int>(m.size()),sample++);
    p.processBlock(audio,midi);
    check(midi.isEmpty(),"processor consumes host MIDI");
}
WireMessages nrpn(int parameter,int value,int channel=1) {
    WireMessages result;
    emitEditorNrpn(channel,parameter,value,[&](int ch,int cc,int val){result.push_back({
        static_cast<uint8_t>(0xb0|(ch-1)),static_cast<uint8_t>(cc),static_cast<uint8_t>(val)});});
    return result;
}
void noUnsafePrefix(const WireMessages& messages) {
    for(const auto& m:messages)check(m.size()!=3||(m[0]&0xf0)!=0xb0||(m[1]!=100&&m[1]!=101),
        "editor output never contains unsafe RPN Null/arpeggiator selectors");
}
void sync(Pfm2AudioProcessor& p,Pfm2MidiDevice& d) {MidiRoutingTestAccess::async(p);MidiRoutingTestAccess::drain(d);}
void capability(Pfm2AudioProcessor& p,Pfm2MidiDevice& d) {
    p.requestEditorCapabilities();noUnsafePrefix(MidiRoutingTestAccess::drain(d));
    MidiRoutingTestAccess::supported(p);check(p.isStoreSupported(),"fresh capability enables Store");
}
juce::ToggleButton* findMpe(juce::Component& c) {
    if(auto* b=dynamic_cast<juce::ToggleButton*>(&c))if(b->getButtonText()=="MPE")return b;
    for(auto* child:c.getChildren())if(auto* found=findMpe(*child))return found;
    return nullptr;
}

// Small, explicit model of the reviewed page-4 selector semantics, not a
// substitute for running firmware/hardware. Detect an unintended second Store
// and the legacy arpeggiator CC mapping with the real serialized output.
struct FirmwareWireProbe {
    int page=0,lsb=0,valueMsb=0,clock=0,direction=0;
    bool fresh=false;
    std::vector<int> stores;
    void feed(const WireMessages& messages) {
        for(const auto& m:messages) {
            if(m.size()!=3 || (m[0]&0xf0)!=0xb0)continue;
            switch(m[1]) {
                case 100: clock=m[2];break;
                case 101: direction=m[2];break;
                case 99: page=m[2];fresh=false;break;
                case 98: lsb=m[2];break;
                case 6: valueMsb=m[2];fresh=true;break;
                case 38:
                    if(page==4 && lsb==2 && fresh)stores.push_back((valueMsb<<7)|m[2]);
                    if(page==4)fresh=false;
                    break;
                default:break;
            }
        }
    }
};
}

int main() {
    juce::ScopedJuceInitialiser_GUI juceInit;
    juce::SharedResourcePointer<Pfm2MidiDevice> device;
    MidiRoutingTestAccess::offline(*device);MidiRoutingTestAccess::drain(*device);
    Pfm2AudioProcessor processor;
    const auto& params=processor.getParameters();
    std::vector<std::string> names;
    for(auto* p:params)names.push_back(p->getName(128).toStdString());
    check(names.size()==242,"parameter count matches reviewed 4.0.4 baseline");
    check(!processor.isMpeEnabled(),"new session defaults to normal mode");
    MidiRoutingTestAccess::channel(processor,7);
    const WireMessages performance{{0x91,60,100},{0x92,60,101},{0xa1,60,91},{0xd2,92},
        {0xe1,2,70},{0xb2,74,93},{0x81,60,0},{0x82,60,0}};
    process(processor,performance);
    auto expected=performance;
    for(auto& m:expected)m[0]=static_cast<uint8_t>((m[0]&0xf0)|6);
    check(MidiRoutingTestAccess::drain(*device)==expected,"normal mode remaps notes and expression");
    processor.setMpeEnabled(true);process(processor,performance);
    check(MidiRoutingTestAccess::drain(*device)==performance,"MPE preserves same-pitch members and expression");
    process(processor,{{0x91,60,100},{0x92,64,100},{0xb1,64,127}});MidiRoutingTestAccess::drain(*device);
    processor.setMpeEnabled(false);process(processor,{{0x93,67,100}});
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x81,60,0},{0xb1,64,0},{0x82,64,0},{0x96,67,100}},
        "mode change releases previous routing before new note");
    processor.releaseResources();
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x86,67,0}},"instance deactivation releases note");

    for(bool mpe:{false,true}) {
        processor.setMpeEnabled(mpe);WireMessages raw;
        for(int ch=0;ch<16;++ch)for(int cc:{6,38,96,97,98,99,100,101})
            raw.push_back({static_cast<uint8_t>(0xb0|ch),static_cast<uint8_t>(cc),48});
        const auto suppressed=processor.getSuppressedConfigurationCount();process(processor,raw);
        check(MidiRoutingTestAccess::drain(*device).empty(),"host configuration cannot poison editor/Store or arpeggiator");
        check(processor.getSuppressedConfigurationCount()==suppressed+128,"configuration suppression observable");
    }
    process(processor,{{0x91,65,100},{0xb1,64,127}});MidiRoutingTestAccess::drain(*device);
    juce::AudioBuffer<float> audio(2,64);juce::MidiBuffer bypassMidi;
    bypassMidi.addEvent(juce::MidiMessage::noteOn(2,66,uint8_t{100}),0);
    processor.processBlockBypassed(audio,bypassMidi);
    check(bypassMidi.isEmpty(),"bypass consumes incoming MIDI");
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x81,65,0},{0xb1,64,0}},"bypass releases own note/sustain");
    process(processor,{{0x91,60,100}});MidiRoutingTestAccess::drain(*device);
    const uint8_t filler[]={0xf8};
    for(int i=0;i<2048;++i)check(device->queueMidiMessage(filler,1),"fill actual bounded output queue");
    process(processor,{{0x81,60,0}});MidiRoutingTestAccess::drain(*device);process(processor,{});
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x81,60,0}},"processor retries rejected note off next block");
    processor.setMpeEnabled(false);process(processor,{{0x91,62,100}});MidiRoutingTestAccess::drain(*device);
    MidiRoutingTestAccess::channel(processor,8);process(processor,{});
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x86,62,0}},"processor releases previous normal channel");
    process(processor,{{0x91,62,100}});MidiRoutingTestAccess::drain(*device);
    device->resetDevices();process(processor,{});
    check(MidiRoutingTestAccess::drain(*device).empty(),"new generation receives no old-device releases");

    processor.setMpeEnabled(true);juce::MemoryBlock saved;processor.getStateInformation(saved);
    processor.setMpeEnabled(false);
    processor.setStateInformation(saved.getData(),static_cast<int>(saved.getSize()),false);
    check(processor.isMpeEnabled(),"MPE XML round trip");
    auto xml=juce::AudioProcessor::getXmlFromBinary(saved.getData(),static_cast<int>(saved.getSize()));
    xml->removeAttribute("MpeEnabled");juce::MemoryBlock legacy;juce::AudioProcessor::copyXmlToBinary(*xml,legacy);
    processor.setStateInformation(legacy.getData(),static_cast<int>(legacy.getSize()),false);
    check(!processor.isMpeEnabled(),"legacy state restores normal mode");
    for(int i=0;i<params.size();++i)check(params[i]->getName(128).toStdString()==names[static_cast<size_t>(i)],
        "parameter order/name unchanged");
    MidiRoutingTestAccess::channel(processor,1);processor.setMpeEnabled(true);sync(processor,*device);capability(processor,*device);
    processor.sendNrpnPresetName();auto letters=MidiRoutingTestAccess::drain(*device);
    check(letters.size()==48,"name has twelve unprefixed NRPNs");noUnsafePrefix(letters);
    processor.flushAllParametrsToNrpn();auto push=MidiRoutingTestAccess::drain(*device);
    check(push.size()==247*4,"Push retains original byte count in MPE");noUnsafePrefix(push);
    MidiRoutingTestAccess::pull(processor);
    check(MidiRoutingTestAccess::drain(*device)==nrpn(0x3fff,0),"Pull uses unprefixed NRPN");
    processor.onParameterUpdated(params[0]);check(MidiRoutingTestAccess::drain(*device).size()==4,"UI edit uses one NRPN");
    processor.hostParameterChanged(0);check(MidiRoutingTestAccess::drain(*device).size()==4,"automation uses one NRPN");
    processor.requestHardwarePosition();
    check(MidiRoutingTestAccess::drain(*device)==nrpn(PREENFM_EDITOR_NRPN_FIRST+PREENFM_EDITOR_REQ_POSITION,0),"Position uses safe NRPN");
    MidiRoutingTestAccess::position(processor,1,2);processor.loadHardwarePreset(2,3);
    auto load=MidiRoutingTestAccess::drain(*device);check(load.size()==7,"Load emits bank, program and Position");noUnsafePrefix(load);
    MidiRoutingTestAccess::position(processor,1,2);
    check(MidiRoutingTestAccess::drain(*device)==nrpn(0x3fff,0),"confirmed Load pulls buffer");
    processor.setStateInformation(saved.getData(),static_cast<int>(saved.getSize()),true);
    auto restore=MidiRoutingTestAccess::drain(*device);check(restore.size()==247*4,"session restore byte count unchanged");noUnsafePrefix(restore);
    MidiRoutingTestAccess::channel(processor,1);sync(processor,*device);capability(processor,*device);
    process(processor,{{0xb0,101,0},{0xb0,100,6}});
    check(processor.beginHardwareStore(2,3),"Store accepted after fresh capability");
    process(processor,{{0xb0,6,48},{0xb0,38,0},{0xb0,96,0}});const auto store=MidiRoutingTestAccess::drain(*device);
    check(store.size()==249*4,"Store has original-size snapshot plus one disarm NRPN");noUnsafePrefix(store);
    const auto request=nrpn(PREENFM_EDITOR_NRPN_FIRST+PREENFM_EDITOR_REQ_STORE,130),park=nrpn(PREENFM_EDITOR_NRPN_LAST,0);
    check(store.size()>=8&&WireMessages(store.end()-8,store.end()-4)==request,"Store target before disarm exact");
    check(store.size()>=4&&WireMessages(store.end()-4,store.end())==park,"Store address disarmed last");
    FirmwareWireProbe wire;
    wire.feed(store);
    wire.feed({{0xb0,6,48},{0xb0,38,0}}); // another physical input, after the batch
    check(wire.stores==std::vector<int>{130},"parked firmware selector cannot repeat Store for later bare data entry");
    check(wire.clock==0&&wire.direction==0,"hardware-MPE-off model receives no legacy arpeggiator CCs");
    MidiRoutingTestAccess::storeDone(processor,130);MidiRoutingTestAccess::channel(processor,5);
    check(!processor.isStoreSupported(),"channel change invalidates old capability immediately");
    check(!processor.beginHardwareStore(2,3),"Store blocked before new-channel handshake");sync(processor,*device);
    check(processor.getEditorProtocolState()==Pfm2AudioProcessor::EditorProtocolState::querying,"channel change starts fresh query");
    MidiRoutingTestAccess::supported(processor);processor.setMpeEnabled(false);
    check(!processor.isStoreSupported(),"MPE change invalidates protocol immediately");sync(processor,*device);MidiRoutingTestAccess::supported(processor);
    processor.setPfmType(2);process(processor,{{0xb0,100,9},{0xb0,6,48}});
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0xb4,100,9},{0xb4,6,48}},"PreenFM3 raw controller routing preserved");
    processor.setPfmType(1);sync(processor,*device);MidiRoutingTestAccess::supported(processor);
    {
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        check(editor!=nullptr,"GUI editor constructs");auto* toggle=findMpe(*editor);check(toggle!=nullptr,"permanent MPE switch exists");
        editor->setSize(Pfm2AudioProcessorEditor::minimumWidth,Pfm2AudioProcessorEditor::minimumHeight);
        if(toggle){check(toggle->getParentComponent()->getLocalBounds().contains(toggle->getBounds()),"MPE switch fits minimum header");
            check(toggle->getToggleState()==processor.isMpeEnabled(),"MPE switch reflects restored state");}
    }
    MidiRoutingTestAccess::drain(*device);
    { Pfm2AudioProcessor other;check(!other.isMpeEnabled(),"second instance independent mode");
      process(other,{{0x91,60,100},{0xb1,64,127}});MidiRoutingTestAccess::drain(*device); }
    check(MidiRoutingTestAccess::drain(*device)==WireMessages{{0x80,60,0},{0xb0,64,0}},"closing processor queues releases for synchronous final drain");
    std::cout<<checks<<" processor checks, "<<failures<<" failures\n";
    return failures?1:0;
}
