/*
 * Copyright 2017 Xavier Hosxe
 *
 * Author: Xavier Hosxe (xavier <dot> hosxe
 *                      (at) g m a i l <dot> com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/PreenLookAndFeel.h"


 //==============================================================================
Pfm2AudioProcessor::Pfm2AudioProcessor()
{
    for (auto& word : pendingUiParameterUpdates) {
        word.store(0, std::memory_order_relaxed);
    }

    myLookAndFeel = new preenfmLookAndFeel();


    // Register to midi device even if not initialized correctly
    pfm2MidiDevice->addListener(this);

    pfm2Editor = nullptr;
    MidifiedFloatParameter* newParam;

    editorWidth = 0;
    editorHeight = 0;

    for (int k = 0; k < 2048; k++) {
        nrpmIndex[k] = -1;
        nrpmIndexPfm3[k] = -1;
    }

    // Algo
    int nrpmParam = PREENFM2_NRPN_ALGO;
    newParam = new MidifiedFloatParameter(String("Algo"), nrpmParam, 1, 1, 32, 1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    // Velocity
    nrpmParam = PREENFM2_NRPN_VELOCITY;
    newParam = new MidifiedFloatParameter(String("Velocity"), nrpmParam, 1, 0, 16, 1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    // Voices pfm2 - same NRPN as playMode for pfm3
    nrpmParam = PREENFM2_NRPN_VOICE;
    // Save voices Param
    newParam = new MidifiedFloatParameter(String("Voices"), nrpmParam, 1, 0, 16, 1);
    voicesParam = newParam;
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    // For play mode we use nrpmIndexPfm3
    // Save voices Param
    newParam = new MidifiedFloatParameter(String("Play Mode pfm3"), nrpmParam, 1, 1, 3, 1);
    playModeParam = newParam;
    addMidifiedParameter(newParam);
    nrpmIndexPfm3[nrpmParam] = newParam->getParamIndex();

    // Glide
    nrpmParam = PREENFM2_NRPN_GLIDE;
    newParam = new MidifiedFloatParameter(String("Glide"), nrpmParam, 1, 0, 12, 1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    for (int k = 0; k < 6; k++) {
        nrpmParam = PREENFM2_NRPN_MIX1 + k * 2;
        newParam = new MidifiedFloatParameter(String("Mix " + String(k + 1)), nrpmParam, 100, 0, 1, 1);
        newParam->setOldName(String("Volume " + String(k + 1)));
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }
    for (int k = 0; k < 6; k++) {
        nrpmParam = PREENFM2_NRPN_PAN1 + k * 2;
        newParam = new MidifiedFloatParameter(String("Pan " + String(k + 1)), nrpmParam, 100, -1, 1, 0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }
    for (int k = 0; k < 6; k++) {
        nrpmParam = PREENFM2_NRPN_IM1 + k * 2;
        newParam = new MidifiedFloatParameter(String("IM " + String(k + 1)), nrpmParam, 100, 0, 16, 1.5);
        addMidifiedParameter(newParam);
        if (k < 5) {
            nrpmIndex[nrpmParam] = newParam->getParamIndex();
        }
        else {
            nrpmIndexPfm3[nrpmParam] = newParam->getParamIndex();
        }
    }
    for (int k = 0; k < 6; k++) {
        nrpmParam = PREENFM2_NRPN_IM1_VELOCITY + k * 2;
        newParam = new MidifiedFloatParameter(String("IM Velocity " + String(k + 1)), nrpmParam, 100, 0, 16, 1);
        addMidifiedParameter(newParam);
        if (k < 5) {
            nrpmIndex[nrpmParam] = newParam->getParamIndex();
        }
        else {
            nrpmIndexPfm3[nrpmParam] = newParam->getParamIndex();
        }
    }
    // OPERATOR
    for (int k = 0; k < 6; k++) {
        //        opShape[k] = new ComboBox("OP"+String(k+1)+" Shape");
        //		0, 7,
        nrpmParam = PREENFM2_NRPN_OPERATOR1_SHAPE + k * 4;
        newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Shape"), nrpmParam, 1, 1, 14, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

    }
    for (int k = 0; k < 6; k++) {
        //        opFrequencyType[k] = new ComboBox("Op"+ String(1) + " Freq Type");
        //        0, 1
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_TYPE + k * 4;
        newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Freq Type"), nrpmParam, 1, 1, 3, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }
    for (int k = 0; k < 6; k++) {
        //
        //        opFrequency[k] = new Slider("Op" + String(k+1) + " Frequency");
        //        opFrequency[k]->setRange (0, 16, 1.0f / 12.0f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY + k * 4;
        newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Frequency"), nrpmParam, 100, 0, 16, 1.0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }
    for (int k = 0; k < 6; k++) {
        //        opFrequencyFineTune[k] = new Slider("Op"+ String(k+1)+ " Fine Tune");
        //        opFrequencyFineTune[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_FINE_TUNE + k * 4;
        newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Fine Tune"), nrpmParam, 100, -9, 9, 0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

    }
    for (int k = 0; k < 6; k++) {
        const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
        for (int p = 0; p < 4; p++) {
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Env" + pointName[p]), nrpmParam, 100, 0, 16, 1);
            addMidifiedParameter(newParam);
            nrpmIndex[nrpmParam] = newParam->getParamIndex();
        }
        const char* pointNameLvl[] = { " Attk lvl", " Deca lvl", " Sust lvl", " Rele lvl" };
        for (int p = 0; p < 4; p++) {
            float level[] = { 1.0, 1.0, 1.0, 0.0 };
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK_LEVEL + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(String("Op" + String(k + 1) + " Env" + pointNameLvl[p]), nrpmParam, 100, 0, 1, level[p]);
            addMidifiedParameter(newParam);
            nrpmIndex[nrpmParam] = newParam->getParamIndex();
        }
        //        enveloppe[k]->setName (TRANS("enveloppe " + String(k+1)));
        //
    }
    for (int k = 0; k < 12; k++) {
        nrpmParam = PREENFM2_NRPN_MTX1_SOURCE + k * 4;
        newParam = new MidifiedFloatParameter(String("Mtx" + String(k + 1) + " Source"), nrpmParam, 1, 1, 100, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        nrpmParam = PREENFM2_NRPN_MTX1_MULTIPLIER + k * 4;
        newParam = new MidifiedFloatParameter(String("Mtx" + String(k + 1) + " Multiplier"), nrpmParam, 100, -10, 10, 0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        nrpmParam = PREENFM2_NRPN_MTX1_DESTINATION1 + k * 4;
        newParam = new MidifiedFloatParameter(String("Mtx" + String(k + 1) + " Destination1"), nrpmParam, 1, 1, 100, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        nrpmParam = PREENFM2_NRPN_MTX1_DESTINATION2 + k * 4;
        newParam = new MidifiedFloatParameter(String("Mtx" + String(k + 1) + " Destination2"), nrpmParam, 1, 1, 100, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }

    for (int k = 0; k < 3; k++) {
        //        addAndMakeVisible(lfoShape[k] = new ComboBox("LFO"+ String(k+1)+ " Shape"));
        //        lfoShape[k]->addItem("Sin", 1);
        //        lfoShape[k]->addItem("Ramp", 2);
        //        lfoShape[k]->addItem("Saw", 3);
        //        lfoShape[k]->addItem("Square", 4);
        //        lfoShape[k]->addItem("Random", 5);
        nrpmParam = PREENFM2_NRPN_LFO1_SHAPE + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " Shape"), nrpmParam, 1, 1, 8, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " External Sync"), nrpmParam, 1, 9990, 10080, 9990);
        newParam->setSendRealValue(true);
        newParam->setDiscreteStepCount(10);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        //        addAndMakeVisible(lfoFrequency[k] = new Slider("LFO"+ String(k+1) + " Frequency"));
        //        lfoFrequency[k]->setRange (0, 24.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " Frequency"), nrpmParam, 100, 0, 99.9f, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        //        addAndMakeVisible(lfoBias[k] = new Slider("LFO"+ String(k+1) + " Bias"));
        //        lfoBias[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_BIAS + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " Bias"), nrpmParam, 100, -1, 1, 0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        //        addAndMakeVisible(lfoKsynOnOff[k] = new ComboBox("LFO"+ String(k+1) + " KeySync"));
        //        lfoKsynOnOff[k]->addItem("Off", 1);
        //        lfoKsynOnOff[k]->addItem("On", 2);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " KeySync"), nrpmParam, 1, 1, 2, 1);
        newParam->setBias(-1);
        newParam->setSendRealValue(true);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        //        addAndMakeVisible(lfoKSync[k] = new Slider("LFO"+ String(k+1) + " KeySync time"));
        //        lfoKSync[k]->setRange (0.0f, 16.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k * 4;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " KeySync time"), nrpmParam, 100, 0, 16, 0.01f);
        newParam->setBias(.01f);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }

    for (int p = 0; p < 4; p++) {
        const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV1_ATTK + p;
        newParam = new MidifiedFloatParameter(String(String("Free Env 1") + pointName[p]), nrpmParam, 100, 0, 16, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }

    for (int p = 0; p < 3; p++) {
        const char* pointName[] = { " Sile", " Attk", " Deca" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV2_SILENCE + p;
        newParam = new MidifiedFloatParameter(String(String("Free Env 2") + pointName[p]), nrpmParam, 100, 0, 16, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }

    //"Step Seq " + String(k+1)
    for (int seq = 0; seq < 2; seq++) {
        //        addAndMakeVisible(stepSeqExtMidiSync[k] = new ComboBox("Step Seq " + String(k+1) + " External Sync"));
        //        stepSeqExtMidiSync[k]->setEditableText (false);
        //        stepSeqExtMidiSync[k]->setColour (ComboBox::buttonColourId, Colours::blue);
        //        stepSeqExtMidiSync[k]->setJustificationType (Justification::left);
        //        stepSeqExtMidiSync[k]->addItem("Internal", 1);
        //        stepSeqExtMidiSync[k]->addItem("MC/4", 2);
        //        stepSeqExtMidiSync[k]->addItem("MC/2", 3);
        //        stepSeqExtMidiSync[k]->addItem("MC", 4);
        //        stepSeqExtMidiSync[k]->addItem("MC*2", 5);
        //        stepSeqExtMidiSync[k]->addItem("MC*4", 6);
        //        stepSeqExtMidiSync[k]->setSelectedId(1);
        //        stepSeqExtMidiSync[k]->addListener (this);


        nrpmParam = PREENFM2_NRPN_STEPSEQ1_BPM + seq * 4;
        newParam = new MidifiedFloatParameter(String("Step Seq " + String(seq + 1) + " External Sync"), nrpmParam, 1, 240, 245, 240);
        newParam->setSendRealValue(true);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();


        //        addAndMakeVisible(stepSeqBPM[k] = new Slider("SEQ"+ String(k+1) + " BPM"));
        //        stepSeqBPM[k]->setRange (10, 240.0f, 1.0f);
        //        stepSeqBPM[k]->setSliderStyle (Slider::RotaryVerticalDrag);
        //        stepSeqBPM[k]->setTextBoxStyle (Slider::TextBoxLeft, false, 35, 16);
        //        stepSeqBPM[k]->setDoubleClickReturnValue(true, 3.0f);
        //        stepSeqBPM[k]->setValue(3.0f, dontSendNotification);
        //        stepSeqBPM[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_BPM + seq * 4;
        newParam = new MidifiedFloatParameter(String("Step Seq " + String(seq + 1) + " BPM"), nrpmParam, 1, 10, 240, 60);
        newParam->setBias(10);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();


        //        addAndMakeVisible(stepSeqGate[k] = new Slider("SEQ"+ String(k+1) + " Gate"));
        //        stepSeqGate[k]->setRange (0.0f, 1.0f, 0.01f);
        //        stepSeqGate[k]->setSliderStyle (Slider::LinearHorizontal);
        //        stepSeqGate[k]->setTextBoxStyle (Slider::TextBoxBelow, false, 35, 16);
        //        stepSeqGate[k]->setDoubleClickReturnValue(true, 0.5f);
        //        stepSeqGate[k]->setValue(0.5f, dontSendNotification);
        //        stepSeqGate[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_GATE + seq * 4;
        newParam = new MidifiedFloatParameter(String("Step Seq " + String(seq + 1) + " Gate"), nrpmParam, 100, 0, 1, .5);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();


        for (int step = 0; step < 16; step++) {
            nrpmParam = PREENFM2_NRPN_STEPSEQ1_STEP1 + (seq * 128) + step;
            newParam = new MidifiedFloatParameter(String(String("Step Seq ") + String(seq + 1) + " Step " + String(step + 1)), nrpmParam, 1, 0, 15, (float)(15 - step));
            addMidifiedParameter(newParam);
            nrpmIndex[nrpmParam] = newParam->getParamIndex();
        }
    }

    nrpmParam = PREENFM2_NRPN_FREE_ENV2_LOOP;
    newParam = new MidifiedFloatParameter("Free Env 2 Loop", nrpmParam, 1, 1, 3, 1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();



    nrpmParam = PREENFM2_NRPN_ARP_CLOCK;
    newParam = new MidifiedFloatParameter(String("Arp clock"), nrpmParam, 1, 1, 3, 1);
    newParam->setOldName("Clock Combo");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_BPM;
    newParam = new MidifiedFloatParameter(String("Arp bpm"), nrpmParam, 1, 10, 240, 60);
    newParam->setOldName("arp bpm slider");
    newParam->setBias(10);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_DIRECTION;
    newParam = new MidifiedFloatParameter(String("Arp direction"), nrpmParam, 1, 1, 12, 1);
    newParam->setOldName("arp dir combo box");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_OCTAVE;
    newParam = new MidifiedFloatParameter(String("Arp octave"), nrpmParam, 1, 1, 3, 1);
    newParam->setOldName("arp octave slider");
    newParam->setBias(1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_PATTERN;
    newParam = new MidifiedFloatParameter(String("Arp pattern"), nrpmParam, 1, 1, 26, 1);
    newParam->setOldName("arp pattern combo box");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_DIVISION;
    newParam = new MidifiedFloatParameter(String("Arp division"), nrpmParam, 1, 1, 17, 1);
    newParam->setOldName("arp division combo box");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_DURATION;
    newParam = new MidifiedFloatParameter(String("Arp duration"), nrpmParam, 1, 1, 17, 1);
    newParam->setOldName("arp duration combo box");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_ARP_LATCH;
    newParam = new MidifiedFloatParameter(String("Arp latch"), nrpmParam, 1, 1, 2, 1);
    newParam->setOldName("arp latch combo box");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_FILTER_TYPE;
    newParam = new MidifiedFloatParameter(String("Filter type"), nrpmParam, 1, 1, 100, 1);
    newParam->setOldName("Filter Combo");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM1;
    newParam = new MidifiedFloatParameter(String("Filter param1"), nrpmParam, 100, 0, 1, .5f);
    newParam->setOldName("filter param1 slider");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM2;
    newParam = new MidifiedFloatParameter(String("Filter param2"), nrpmParam, 100, 0, 1, .5f);
    newParam->setOldName("filter param2 slider");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_FILTER_GAIN;
    newParam = new MidifiedFloatParameter(String("Filter gain"), nrpmParam, 100, 0, 2, .9f);
    newParam->setOldName("filter gain slider");
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();


    for (int k = 0; k < 3; k++) {
        // addAndMakeVisible(lfoPhase[k] = new SliderPfm2("LFO"+ String(k+1) + " Phase"));
        // lfoPhase[k]->setRange (0, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO_PHASE1 + k;
        newParam = new MidifiedFloatParameter(String("LFO" + String(k + 1) + " Phase"), nrpmParam, 100, 0, 1, 0);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();
    }

    for (int n = 0; n < 2; n++) {

        //    addAndMakeVisible(noteBefore[n] = new ComboBox("Note"+ String(n + 1)+" before"));
        //    noteBefore[n]->setEditableText (false);
        //    noteBefore[n]->setJustificationType (Justification::left);
        //    noteBefore[n]->setColour (ComboBox::buttonColourId, Colours::blue);
        //    noteBefore[n]->addItem("Flat", 1);
        //    noteBefore[n]->addItem("+Linear", 2);
        //    noteBefore[n]->addItem("+Linear*8", 3);
        //    noteBefore[n]->addItem("+Exp", 4);
        //    noteBefore[n]->addItem("-Linear", 5);
        //    noteBefore[n]->addItem("-Linear*8", 6);
        //    noteBefore[n]->addItem("-Exp", 7);
        //    noteBefore[n]->setSelectedId(1);
        //    noteBefore[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_BEFORE + n * 4;
        newParam = new MidifiedFloatParameter(String("Note" + String(n + 1) + " before"), nrpmParam, 1, 1, 7, 5);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();


        //    addAndMakeVisible(noteBreak[n] = new SliderPfm2("Note"+ String(n + 1)+" break"));
        //    noteBreak[n]->setRange (0, 127.0f, 1.0f);
        //    noteBreak[n]->setSliderStyle (Slider::RotaryVerticalDrag);
        //    noteBreak[n]->setTextBoxStyle (Slider::TextBoxBelow, false, 35, 16);
        //    noteBreak[n]->setDoubleClickReturnValue(true, 3.0f);
        //    noteBreak[n]->setValue(3.0f, dontSendNotification);
        //    noteBreak[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_BREAKNOTE + n * 4;
        newParam = new MidifiedFloatParameter(String("Note" + String(n + 1) + " break"), nrpmParam, 1, 0, 127, 60);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

        //    addAndMakeVisible(noteAfter[n] = new ComboBox("Note"+ String(n + 1)+" after"));
        //    noteAfter[n]->setEditableText (false);
        //    noteAfter[n]->setJustificationType (Justification::left);
        //    noteAfter[n]->setColour (ComboBox::buttonColourId, Colours::blue);
        //    noteAfter[n]->addItem("Flat", 1);
        //    noteAfter[n]->addItem("+Linear", 2);
        //    noteAfter[n]->addItem("+Linear*8", 3);
        //    noteAfter[n]->addItem("+Exp", 4);
        //    noteAfter[n]->addItem("-Linear", 5);
        //    noteAfter[n]->addItem("-Linear*8", 6);
        //    noteAfter[n]->addItem("-Exp", 7);
        //    noteAfter[n]->setSelectedId(1);
        //    noteAfter[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_AFTER + n * 4;
        newParam = new MidifiedFloatParameter(String("Note" + String(n + 1) + " after"), nrpmParam, 1, 1, 7, 1);
        addMidifiedParameter(newParam);
        nrpmIndex[nrpmParam] = newParam->getParamIndex();

    }

    // PFM3
    nrpmParam = PREENFM2_NRPN_GLIDE_TYPE;
    newParam = new MidifiedFloatParameter(String("Glide Type"), nrpmParam, 1, 1, 3, 1);
    addMidifiedParameter(newParam);
    nrpmIndexPfm3[nrpmParam] = newParam->getParamIndex();

    // PFM2 playmode has same NRPN as preenfm3 Glide Type
    newParam = new MidifiedFloatParameter(String("Play Mode pfm2"), nrpmParam, 1, 1, 2, 1);
    newParam->setBias(1);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();
    
    nrpmParam = PREENFM2_NRPN_UNISON_SPREAD;
    newParam = new MidifiedFloatParameter(String("Unison Spread"), nrpmParam, 100, 0, 1, .12f);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    nrpmParam = PREENFM2_NRPN_UNISON_DETUNE;
    newParam = new MidifiedFloatParameter(String("Unison Detune"), nrpmParam, 100, -1, 1, .5f);
    addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = newParam->getParamIndex();

    presetName = "New Preset";


    // preenfm2 / 3 selector
    pfmType = 1;
    nrpmParam = 127 * 128 + 124;
    newParam = new MidifiedFloatParameter("pfm Type", nrpmParam, 1, 1, 2, 1);
    newParam->setIsAutomatable(false);
    addMidifiedParameter(newParam);
    nrpmIndex[2044] = newParam->getParamIndex();

    // Midi Channel
    currentMidiChannel = 1;
    nrpmParam = 127 * 128 + 125;
    newParam = new MidifiedFloatParameter("Midi Channel", nrpmParam, 1, 1, 16, 1);
    newParam->setIsAutomatable(false);
    addMidifiedParameter(newParam);
    nrpmIndex[2045] = newParam->getParamIndex();

    nrpmParam = 127 * 128 + 126;
    newParam = new MidifiedFloatParameter("push button", nrpmParam, 1, 0, 127, 0);
    newParam->setIsAutomatable(false);
    addMidifiedParameter(newParam);
    // Put in last slot
    nrpmIndex[2046] = newParam->getParamIndex();

    nrpmParam = 127 * 128 + 127;
    newParam = new MidifiedFloatParameter("pull button", nrpmParam, 1, 0, 127, 0);
    newParam->setIsAutomatable(false);
    addMidifiedParameter(newParam);
    // Put in last slot
    nrpmIndex[2047] = newParam->getParamIndex();

    midiMessageCollector.reset(44100);


}

Pfm2AudioProcessor::~Pfm2AudioProcessor()
{
    stopTimer();
    releaseResources();
    pfm2MidiDevice->removeListener(this);
    cancelPendingUpdate();
    // Releasing here is mandatory: the transaction claim lives in the shared
    // device, so an instance dying with an open claim would block the protocol
    // for every other instance for the rest of the session.
    finishEditorTransaction();

    delete myLookAndFeel;

}

//==============================================================================
const String Pfm2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}


bool Pfm2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Pfm2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double Pfm2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pfm2AudioProcessor::getNumPrograms()
{
    return 1;
}

int Pfm2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Pfm2AudioProcessor::setCurrentProgram(int)
{
    // Nothing to do;
}

const String Pfm2AudioProcessor::getProgramName(int)
{
    return getPresetName();
}

void Pfm2AudioProcessor::changeProgramName(int, const String& newName)
{
    setPresetName(newName);
}

//==============================================================================
void Pfm2AudioProcessor::prepareToPlay(double, int)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Pfm2AudioProcessor::releaseResources()
{
    const auto send = [this](const uint8_t* data, int size, int channel) {
        return pfm2MidiDevice->queueMidiMessage(data, size, channel);
    };
    if (performanceRouter.configure(isMpeEnabled(), currentMidiChannel.load(),
            pfm2MidiDevice->getDeviceGeneration(), send))
        performanceRouter.release(send);
}

void Pfm2AudioProcessor::setMpeEnabled(bool enabled, bool notifyHost)
{
    if (mpeEnabled.exchange(enabled, std::memory_order_relaxed) != enabled) {
        markProtocolContextChanged();
        if (notifyHost)
            updateHostDisplay(ChangeDetails{}.withNonParameterStateChanged(true));
    }
}

void Pfm2AudioProcessor::markProtocolContextChanged() noexcept {
    protocolContextChanged.store(true, std::memory_order_release);
    requestEditorStateUpdate();
}

void Pfm2AudioProcessor::setControlChannel(int channel) noexcept {
    channel = jlimit(1, 16, channel);
    if (currentMidiChannel.exchange(channel, std::memory_order_relaxed) != channel)
        markProtocolContextChanged();
}

void Pfm2AudioProcessor::invalidateProtocolContext() {
    if (transactionToken != Pfm2MidiDevice::invalidTransactionToken)
        abandonTransactionForDeviceChange();
    editorProtocolState = EditorProtocolState::unknown;
    editorProtocolVersion = editorCapabilities = 0;
    capabilityVersionSeen = false;
    reportedBank = reportedPreset = 0;
    reportedPositionValid = false;
    noteProtocolChange("MIDI routing changed. Checking hardware protocol again...");
}

void Pfm2AudioProcessor::processBlockBypassed(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) {
    buffer.clear();
    releaseResources();
    midiMessages.clear();
}


void Pfm2AudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{

    buffer.clear();

    const int outputChannel = jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed));
    const auto send = [this](const uint8_t* data, int size, int channel) {
        return pfm2MidiDevice->queueMidiMessage(data, size, channel,
            pfmType.load(std::memory_order_relaxed) == 1);
    };
    if (performanceRouter.configure(isMpeEnabled(), outputChannel,
            pfm2MidiDevice->getDeviceGeneration(), send)) {
        for (const auto metadata : midiMessages) {
            performanceRouter.forward(metadata.data, metadata.numBytes, send);
        }
    }

    midiMessages.clear();
}

//==============================================================================
bool Pfm2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

void Pfm2AudioProcessor::editorClosed(Pfm2AudioProcessorEditor* editor) {
    if (editor == nullptr || pfm2Editor != editor) {
        return;
    }

    editorResized(editor->getWidth(), editor->getHeight());

    pfm2Editor = nullptr;
}

uint64_t Pfm2AudioProcessor::getDroppedOutputEventCount() const noexcept {
    return pfm2MidiDevice->getDroppedOutputEventCount();
}

void Pfm2AudioProcessor::editorResized(int width, int height) noexcept {
    if (width > 0 && height > 0) {
        editorWidth.store(width, std::memory_order_relaxed);
        editorHeight.store(height, std::memory_order_relaxed);
    }
}

AudioProcessorEditor* Pfm2AudioProcessor::createEditor()
{

    pfm2Editor = new Pfm2AudioProcessorEditor(this);
    pfm2Editor->setPfmType(pfmType);
    pfm2Editor->setMidiChannel(currentMidiChannel);
    pfm2Editor->setPresetName(getPresetName());
    const int savedEditorWidth = editorWidth.load(std::memory_order_relaxed);
    const int savedEditorHeight = editorHeight.load(std::memory_order_relaxed);
    if (savedEditorWidth > 0 && savedEditorHeight > 0) {
        pfm2Editor->setSize(jmax(savedEditorWidth, Pfm2AudioProcessorEditor::minimumWidth),
            jmax(savedEditorHeight, Pfm2AudioProcessorEditor::minimumHeight));
    }

    return pfm2Editor;
}


void Pfm2AudioProcessor::setPfmType(int pt) {
    pt = jlimit(1, 2, pt);
    if (pfmType.exchange(pt, std::memory_order_relaxed) != pt)
        markProtocolContextChanged();
    auto* messageManager = MessageManager::getInstanceWithoutCreating();
    if (messageManager != nullptr && messageManager->isThisTheMessageThread()
        && pfm2Editor != nullptr) {
        pfm2Editor->setPfmType(pfmType);
    }
    else {
        requestEditorStateUpdate();
    }
}


void Pfm2AudioProcessor::setHardwarePresetTarget(
    int bankNumber, int presetNumber) noexcept {
    // Only 64 regular PreenFM patch banks are addressable
    // (NUMBEROFPREENFMBANKS), regardless of the theoretical width of CC32.
    // Clamping rather than wrapping keeps an old saved state that stored bank
    // 65..128 on bank 64 instead of silently folding it back onto bank 1.
    hardwarePresetBank.store(jlimit(1, PREENFM_EDITOR_BANK_COUNT, bankNumber),
        std::memory_order_relaxed);
    hardwarePresetNumber.store(
        jlimit(1, PREENFM_EDITOR_PRESET_COUNT, presetNumber),
        std::memory_order_relaxed);
}


void Pfm2AudioProcessor::queueHardwarePresetSelection(
    int bankNumber, int presetNumber) {
    const int channel = jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed));
    const int zeroBasedBank =
        jlimit(1, PREENFM_EDITOR_BANK_COUNT, bankNumber) - 1;
    const int zeroBasedPreset =
        jlimit(1, PREENFM_EDITOR_PRESET_COUNT, presetNumber) - 1;

    // CC#0 selects a native PreenFM patch bank. CC#32 and Program Change
    // select the zero-based bank and patch positions documented by the
    // firmware MIDI protocol.
    pfm2MidiDevice->queueMidiMessage(
        MidiMessage::controllerEvent(channel, 0, 0));
    pfm2MidiDevice->queueMidiMessage(
        MidiMessage::controllerEvent(channel, 32, zeroBasedBank));
    pfm2MidiDevice->queueMidiMessage(
        MidiMessage::programChange(channel, zeroBasedPreset));
}


void Pfm2AudioProcessor::requestCurrentHardwarePreset() {
    pfm2MidiDevice->queueNrpn(jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed)),
        0x3fff, 0);
}


void Pfm2AudioProcessor::loadHardwarePreset(
    int bankNumber, int presetNumber) {
    if (isHardwareBusy()) {
        noteProtocolChange("A hardware operation is still running.");
        return;
    }

    // Claim first, send afterwards. Bank Select and Program Change replace the
    // hardware edit buffer, so they must never slip into another instance's
    // store batch. If the claim fails, nothing mutating goes out at all and no
    // apparent legacy success is produced.
    if (!beginEditorTransaction()) {
        noteProtocolChange("Another plugin instance is talking to the "
            "hardware. Nothing was sent. Try again in a moment.");
        return;
    }

    setHardwarePresetTarget(bankNumber, presetNumber);
    loadTargetBank = getHardwarePresetBank();
    loadTargetPreset = getHardwarePresetNumber();
    queueHardwarePresetSelection(loadTargetBank, loadTargetPreset);

    // The firmware deliberately sends nothing after a program change, so the
    // editor has to find out by itself whether the load actually happened.
    // With the 3.00 alpha protocol we ask for the position and only pull once
    // it matches; a missing bank otherwise looks exactly like a success.
    if (isPositionQuerySupported()) {
        positionQueryForLoad = true;
        positionQueryPending = true;
        pendingBankType = pendingBank = pendingPreset = pendingValid = -1;
        positionDeadline = Time::getMillisecondCounter() + editorQueryTimeoutMs;
        sendEditorRequest(PREENFM_EDITOR_REQ_POSITION, 0);
        noteProtocolChange("Loading bank " + String(loadTargetBank)
            + ", preset " + String(loadTargetPreset)
            + ": confirming hardware position...");
    }
    else {
        // Legacy path for firmware without the editor protocol. Loading from
        // USB completes in firmware before the following MIDI messages are
        // handled, but a short delay also keeps the subsequent full dump
        // separate from the selection messages on slower hosts/interfaces.
        hardwarePresetPullPending.store(true, std::memory_order_release);
        hardwarePresetPullDeadline =
            Time::getMillisecondCounter() + 250;
        noteProtocolChange("Loading bank " + String(loadTargetBank)
            + ", preset " + String(loadTargetPreset) + " and pulling...");
    }

    startTimer(editorTimerIntervalMs);
}


// Wrap-safe deadline test: comparing the raw uint32 millisecond counter with
// >= breaks once every 49.7 days of uptime.
static inline bool hasDeadlinePassed(uint32 now, uint32 deadline) noexcept {
    return static_cast<int32>(now - deadline) >= 0;
}


void Pfm2AudioProcessor::timerCallback() {
    const uint32 now = Time::getMillisecondCounter();

    // A device change invalidates everything still queued, so an open
    // transaction can never complete on the device it was started for.
    if (transactionToken != Pfm2MidiDevice::invalidTransactionToken
        && !isTransactionDeviceCurrent()) {
        abandonTransactionForDeviceChange();
        stopTimer();
        return;
    }

    if (hardwarePresetPullPending.load(std::memory_order_acquire)
        && hasDeadlinePassed(now, hardwarePresetPullDeadline)) {
        if (hardwarePresetPullPending.exchange(false,
            std::memory_order_acq_rel)) {
            requestCurrentHardwarePreset();
            // The legacy load path holds the claim until the pull is issued.
            finishEditorTransaction();
        }
    }

    serviceEditorProtocolTimeouts(now);

    if (!hardwarePresetPullPending.load(std::memory_order_acquire)
        && editorProtocolState != EditorProtocolState::querying
        && !positionQueryPending
        && storeState == HardwareStoreState::idle) {
        stopTimer();
    }
}


//==============================================================================
// Editor remote protocol, firmware 3.00 alpha.
// Everything below runs on the message thread only.
//==============================================================================

void Pfm2AudioProcessor::sendEditorRequest(int requestId, int value) {
    pfm2MidiDevice->queueNrpn(jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed)),
        PREENFM_EDITOR_NRPN_FIRST + requestId, value);
}


void Pfm2AudioProcessor::noteProtocolChange(const String& operationText) {
    if (operationText.isNotEmpty()) {
        lastOperationText = operationText;
    }
    ++protocolRevision;
}


bool Pfm2AudioProcessor::beginEditorTransaction() {
    const uint32_t token = pfm2MidiDevice->tryBeginEditorTransaction();
    if (token == Pfm2MidiDevice::invalidTransactionToken) {
        return false;
    }

    transactionToken = token;
    transactionDeviceGeneration = pfm2MidiDevice->getDeviceGeneration();
    return true;
}


void Pfm2AudioProcessor::finishEditorTransaction() {
    pfm2MidiDevice->endEditorTransaction(transactionToken);
    transactionToken = Pfm2MidiDevice::invalidTransactionToken;
}


bool Pfm2AudioProcessor::isTransactionDeviceCurrent() const noexcept {
    return transactionDeviceGeneration == pfm2MidiDevice->getDeviceGeneration();
}


void Pfm2AudioProcessor::abandonTransactionForDeviceChange() {
    // Everything still queued for the previous device is discarded by the
    // device layer, so an open transaction can never be completed. A store in
    // flight has an unknown outcome and is latched as such.
    if (storeState != HardwareStoreState::idle) {
        storeOutcomeUnknown = true;
        unknownStoreBank = storeTargetBank;
        unknownStorePreset = storeTargetPreset;
        noteProtocolChange("MIDI routing/device changed during a store into bank "
            + String(storeTargetBank) + ", preset " + String(storeTargetPreset)
            + ". Result unknown - check the hardware before storing again.");
    }

    storeState = HardwareStoreState::idle;
    storeTargetWire = -1;
    positionQueryPending = false;
    positionQueryForLoad = false;
    hardwarePresetPullPending.store(false, std::memory_order_release);
    if (editorProtocolState == EditorProtocolState::querying) {
        editorProtocolState = EditorProtocolState::unknown;
    }
    finishEditorTransaction();
}


bool Pfm2AudioProcessor::buildStoreBatch(int storeTargetValue,
    std::vector<Pfm2MidiDevice::NrpnItem>& items) const {
    items.clear();

    // The snapshot is frozen here, before anything is sent. Reading the
    // parameters again while the batch is being transmitted would let host
    // automation slip a different value into the middle of the patch even
    // though the MIDI output itself is exclusive.
    const String currentPresetName = getPresetName();
    char nameToSend[maximumPresetNameLength + 1] = {};
    for (int k = 0; k < currentPresetName.length()
        && k < maximumPresetNameLength; k++) {
        nameToSend[k] = static_cast<char>(currentPresetName[k]);
    }
    for (int k = 0; k < maximumPresetNameLength; k++) {
        items.push_back({ static_cast<uint16_t>(PREENFM2_NRPN_LETTER1 + k),
            static_cast<uint16_t>(static_cast<unsigned char>(nameToSend[k])) });
    }

    const auto& parameterSet = getParameters();
    const int firstInternalParameterIndex = nrpmIndex[PREENFM_NRPN_PFMTYPE];
    const int currentPfmType = pfmType.load(std::memory_order_relaxed);

    for (int p = 0; p < parameterSet.size(); p++) {
        if (firstInternalParameterIndex >= 0 && p >= firstInternalParameterIndex) {
            break;
        }

        auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[p]);
        if (midifiedFP == nullptr) {
            continue;
        }

        const int nrpnParam = midifiedFP->getNrpnParam();
        if (nrpnParam < 0 || nrpnParam >= nrpnLookupSize) {
            continue;
        }
        if (currentPfmType == 1 && nrpmIndex[nrpnParam] == -1) {
            continue;
        }
        if (currentPfmType == 1 && midifiedFP == playModeParam) {
            continue;
        }
        if (currentPfmType == 2 && midifiedFP == voicesParam) {
            continue;
        }

        const int nrpnValue = midifiedFP->getNrpnValue();
        if (nrpnValue < 0 || nrpnValue > 0x3fff) {
            return false;
        }
        items.push_back({ static_cast<uint16_t>(nrpnParam),
            static_cast<uint16_t>(nrpnValue) });
    }

    // The store request is the last element of the same batch, so nothing can
    // modify the hardware edit buffer between the snapshot and the write.
    items.push_back({
        static_cast<uint16_t>(PREENFM_EDITOR_NRPN_FIRST + PREENFM_EDITOR_REQ_STORE),
        static_cast<uint16_t>(storeTargetValue) });
    // Disarm Store even for a controller on another physical MIDI input.
    // The firmware ignores page-4 response addresses, including LSB 127.
    items.push_back({ static_cast<uint16_t>(PREENFM_EDITOR_NRPN_LAST), 0 });
    return true;
}


void Pfm2AudioProcessor::requestEditorCapabilities() {
    if (protocolContextChanged.exchange(false, std::memory_order_acq_rel))
        invalidateProtocolContext();
    if (editorProtocolState == EditorProtocolState::querying) {
        return;
    }

    if (!beginEditorTransaction()) {
        noteProtocolChange("Another plugin instance is talking to the "
            "hardware. Try again in a moment.");
        return;
    }

    editorProtocolState = EditorProtocolState::querying;
    editorProtocolVersion = 0;
    editorCapabilities = 0;
    capabilityVersionSeen = false;
    capabilityDeadline = Time::getMillisecondCounter() + editorQueryTimeoutMs;
    sendEditorRequest(PREENFM_EDITOR_REQ_CAPABILITY, 0);
    noteProtocolChange("Checking hardware protocol...");
    startTimer(editorTimerIntervalMs);
}


void Pfm2AudioProcessor::requestHardwarePosition() {
    if (positionQueryPending) {
        return;
    }

    if (!isPositionQuerySupported()) {
        noteProtocolChange("Position query needs editor protocol v1 firmware. "
            "Run Check protocol first.");
        return;
    }

    if (!beginEditorTransaction()) {
        noteProtocolChange("Another plugin instance is talking to the "
            "hardware. Try again in a moment.");
        return;
    }

    positionQueryForLoad = false;
    positionQueryPending = true;
    pendingBankType = pendingBank = pendingPreset = pendingValid = -1;
    positionDeadline = Time::getMillisecondCounter() + editorQueryTimeoutMs;
    sendEditorRequest(PREENFM_EDITOR_REQ_POSITION, 0);
    noteProtocolChange("Reading hardware position...");
    startTimer(editorTimerIntervalMs);
}


bool Pfm2AudioProcessor::beginHardwareStore(int bankNumber, int presetNumber) {
    if (!isStoreSupported()) {
        noteProtocolChange("Store needs editor protocol v1 firmware. "
            "Run Check protocol first.");
        return false;
    }

    if (isHardwareBusy()) {
        noteProtocolChange("A hardware operation is still running.");
        return false;
    }

    const int bank = jlimit(1, PREENFM_EDITOR_BANK_COUNT, bankNumber);
    const int preset = jlimit(1, PREENFM_EDITOR_PRESET_COUNT, presetNumber);
    if (bank != bankNumber || preset != presetNumber) {
        noteProtocolChange("Invalid store target.");
        return false;
    }

    if (!beginEditorTransaction()) {
        noteProtocolChange("Another plugin instance is talking to the "
            "hardware. Try again in a moment.");
        return false;
    }

    const int targetWire = ((bank - 1) << 7) | (preset - 1);

    // Freeze the snapshot first, then hand the whole thing to the device as
    // one atomic batch: name letters, every parameter, and the store request.
    std::vector<Pfm2MidiDevice::NrpnItem> batch;
    if (!buildStoreBatch(targetWire, batch)) {
        finishEditorTransaction();
        noteProtocolChange("Store aborted: the patch snapshot is invalid.");
        return false;
    }

    // Either the whole run is reserved or nothing is enqueued at all, so a
    // rejected batch can never leave a half transmitted patch on the wire.
    if (!pfm2MidiDevice->queueNrpnBatch(jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed)),
        batch.data(), batch.size())) {
        finishEditorTransaction();
        noteProtocolChange("Store aborted: the MIDI output queue could not "
            "take the complete patch. Nothing was sent.");
        return false;
    }

    storeTargetBank = bank;
    storeTargetPreset = preset;
    storeTargetWire = targetWire;
    storeState = HardwareStoreState::awaitingEcho;
    storeDeadline = Time::getMillisecondCounter() + editorStoreTimeoutMs;
    noteProtocolChange("Storing into bank " + String(bank) + ", preset "
        + String(preset) + "...");
    startTimer(editorTimerIntervalMs);
    return true;
}


void Pfm2AudioProcessor::applyReportedPosition() {
    // Only a complete group for a regular patch bank counts. VALID = 0 simply
    // means no patch bank is selected on the hardware; that is not an error
    // and must not overwrite the browser target.
    const bool complete = pendingBankType >= 0 && pendingBank >= 0
        && pendingPreset >= 0 && pendingValid >= 0;
    if (!complete) {
        return;
    }

    positionQueryPending = false;
    finishEditorTransaction();

    const bool usable = pendingBankType == PREENFM_EDITOR_BANKTYPE_PATCH
        && pendingValid == 1
        && pendingBank >= 0 && pendingBank < PREENFM_EDITOR_BANK_COUNT
        && pendingPreset >= 0 && pendingPreset < PREENFM_EDITOR_PRESET_COUNT;

    if (!usable) {
        reportedPositionValid = false;
        reportedBank = 0;
        reportedPreset = 0;

        if (positionQueryForLoad) {
            positionQueryForLoad = false;
            noteProtocolChange("Load failed: the hardware reports no selected "
                "patch bank. Bank " + String(loadTargetBank)
                + " is probably missing on the SD card.");
        }
        else {
            noteProtocolChange("Hardware position unknown "
                "(no patch bank selected).");
        }
        return;
    }

    reportedBank = pendingBank + 1;
    reportedPreset = pendingPreset + 1;
    reportedPositionValid = true;

    // An explicit, successful position read is the resynchronisation that
    // clears a latched unknown store outcome.
    if (storeOutcomeUnknown && !positionQueryForLoad) {
        storeOutcomeUnknown = false;
        unknownStoreBank = 0;
        unknownStorePreset = 0;
    }

    if (positionQueryForLoad) {
        positionQueryForLoad = false;
        if (reportedBank == loadTargetBank
            && reportedPreset == loadTargetPreset) {
            requestCurrentHardwarePreset();
            noteProtocolChange("Loaded bank " + String(reportedBank)
                + ", preset " + String(reportedPreset) + ", pulling...");
        }
        else {
            // Never claim a successful load. The usual cause is a bank that
            // does not exist, in which case the hardware stays where it was.
            noteProtocolChange("Load failed: hardware is at bank "
                + String(reportedBank) + ", preset " + String(reportedPreset)
                + ", not at the requested bank " + String(loadTargetBank)
                + ", preset " + String(loadTargetPreset)
                + ". The bank is probably missing.");
        }
    }
    else {
        noteProtocolChange("Hardware is at bank " + String(reportedBank)
            + ", preset " + String(reportedPreset) + ".");
    }
}


void Pfm2AudioProcessor::handleEditorProtocolResponse(int responseId,
    int value) {
    switch (responseId) {
    case PREENFM_EDITOR_RSP_PROTOCOL_VERSION:
        if (editorProtocolState != EditorProtocolState::querying) {
            return;   // unsolicited or late, ignore
        }
        editorProtocolVersion = value;
        capabilityVersionSeen = true;
        return;

    case PREENFM_EDITOR_RSP_CAPABILITIES:
        if (editorProtocolState != EditorProtocolState::querying
            || !capabilityVersionSeen) {
            return;
        }
        editorCapabilities = value;
        finishEditorTransaction();
        if (editorProtocolVersion == PREENFM_EDITOR_PROTOCOL_VERSION) {
            editorProtocolState = EditorProtocolState::supported;
            noteProtocolChange("Protocol version "
                + String(editorProtocolVersion) + " detected"
                + (isStoreSupported() ? ", Store available." : "."));
        }
        else {
            editorProtocolState = EditorProtocolState::unsupported;
            noteProtocolChange("Unsupported protocol version "
                + String(editorProtocolVersion)
                + ". This editor speaks version "
                + String(PREENFM_EDITOR_PROTOCOL_VERSION) + ".");
        }
        return;

    case PREENFM_EDITOR_RSP_POSITION_BANKTYPE:
        if (positionQueryPending) { pendingBankType = value; applyReportedPosition(); }
        return;
    case PREENFM_EDITOR_RSP_POSITION_BANK:
        if (positionQueryPending) { pendingBank = value; applyReportedPosition(); }
        return;
    case PREENFM_EDITOR_RSP_POSITION_PRESET:
        if (positionQueryPending) { pendingPreset = value; applyReportedPosition(); }
        return;
    case PREENFM_EDITOR_RSP_POSITION_VALID:
        if (positionQueryPending) { pendingValid = value; applyReportedPosition(); }
        return;

    case PREENFM_EDITOR_RSP_STORE_TARGET:
        if (storeState != HardwareStoreState::awaitingEcho) {
            return;
        }
        if (value != storeTargetWire) {
            // A target we did not ask for. Do not treat a later status 0 as
            // our success.
            storeState = HardwareStoreState::idle;
            storeTargetWire = -1;
            finishEditorTransaction();
            noteProtocolChange("Store failed: the hardware echoed a different "
                "target. Nothing was written by this editor.");
            return;
        }
        storeState = HardwareStoreState::awaitingStatus;
        return;

    case PREENFM_EDITOR_RSP_STORE_STATUS:
    {
        // A status without a matching target echo is never a success.
        if (storeState != HardwareStoreState::awaitingStatus) {
            if (storeState == HardwareStoreState::awaitingEcho) {
                storeState = HardwareStoreState::idle;
                storeTargetWire = -1;
                finishEditorTransaction();
                noteProtocolChange("Store failed: status arrived without a "
                    "target echo (protocol error).");
            }
            return;
        }

        const int bank = storeTargetBank;
        const int preset = storeTargetPreset;
        storeState = HardwareStoreState::idle;
        storeTargetWire = -1;
        finishEditorTransaction();

        switch (value) {
        case PREENFM_EDITOR_STATUS_OK:
            reportedBank = bank;
            reportedPreset = preset;
            reportedPositionValid = true;
            // Status 0 means the firmware saved its current edit buffer. It
            // does not prove that every preceding NRPN of the snapshot really
            // arrived; only a readback could show that.
            noteProtocolChange("Stored into bank " + String(bank)
                + ", preset " + String(preset)
                + " (firmware saved its edit buffer; verify by reloading).");
            break;
        case PREENFM_EDITOR_STATUS_BANK_NOT_FOUND:
            noteProtocolChange("Store failed: bank " + String(bank)
                + " is missing or read-only on the hardware. Nothing written.");
            break;
        case PREENFM_EDITOR_STATUS_INVALID_TARGET:
            noteProtocolChange("Store failed: bank " + String(bank)
                + ", preset " + String(preset)
                + " is not a valid slot. Nothing written.");
            break;
        case PREENFM_EDITOR_STATUS_AMBIGUOUS_CHANNEL:
            noteProtocolChange("Store failed: MIDI channel "
                + String(currentMidiChannel.load(std::memory_order_relaxed))
                + " does not address exactly one timbre. Give the timbre its "
                "own dedicated MIDI channel. Nothing written.");
            break;
        case PREENFM_EDITOR_STATUS_STORAGE_ERROR:
            // savePreenFMPatch() writes the payload and the padding in two
            // separate steps, so a failure of the second one leaves the slot
            // already changed. Do not claim that nothing was written.
            storeOutcomeUnknown = true;
            unknownStoreBank = bank;
            unknownStorePreset = preset;
            noteProtocolChange("Storage error at bank " + String(bank)
                + ", preset " + String(preset)
                + ". The slot may already be changed or incomplete - check it "
                "on the hardware before storing there again.");
            break;
        case PREENFM_EDITOR_STATUS_PROTOCOL_ERROR:
            noteProtocolChange("Store failed: the hardware reported a protocol "
                "error. Nothing written.");
            break;
        default:
            noteProtocolChange("Store failed: unknown status "
                + String(value) + ". Nothing written.");
            break;
        }
        return;
    }

    default:
        // Unknown response id: ignore safely, a later firmware may add some.
        return;
    }
}


void Pfm2AudioProcessor::serviceEditorProtocolTimeouts(uint32 now) {
    if (editorProtocolState == EditorProtocolState::querying
        && hasDeadlinePassed(now, capabilityDeadline)) {
        editorProtocolState = EditorProtocolState::unsupported;
        finishEditorTransaction();
        noteProtocolChange("Protocol not detected. Load and Pull still work. "
            "For Store, the hardware needs editor protocol v1 firmware and "
            "Receives set to NRPN or CC & NRPN.");
    }

    if (positionQueryPending && hasDeadlinePassed(now, positionDeadline)) {
        positionQueryPending = false;
        finishEditorTransaction();

        if (positionQueryForLoad) {
            positionQueryForLoad = false;
            // Fall back to the legacy behaviour so a load still works if the
            // position answer got lost.
            hardwarePresetPullPending.store(true, std::memory_order_release);
            hardwarePresetPullDeadline = now + 250;
            noteProtocolChange("No position answer, pulling without "
                "confirmation.");
        }
        else {
            noteProtocolChange("No position answer from the hardware.");
        }
    }

    if (storeState != HardwareStoreState::idle
        && hasDeadlinePassed(now, storeDeadline)) {
        const int bank = storeTargetBank;
        const int preset = storeTargetPreset;
        storeState = HardwareStoreState::idle;
        storeTargetWire = -1;
        finishEditorTransaction();

        // The firmware writes before it answers, so a timeout does not mean
        // "nothing happened". The outcome is latched as unknown: no further
        // store is offered and nothing is retried automatically until the
        // position has been resynchronised.
        storeOutcomeUnknown = true;
        unknownStoreBank = bank;
        unknownStorePreset = preset;
        noteProtocolChange("Store result UNKNOWN for bank " + String(bank)
            + ", preset " + String(preset)
            + ". The slot may already have been written. Do not overwrite it "
            "again - resynchronise with Position and check the hardware first.");
    }
}


String Pfm2AudioProcessor::getEditorProtocolStatusText() const {
    switch (editorProtocolState) {
    case EditorProtocolState::unknown:
        return "Protocol: not checked";
    case EditorProtocolState::querying:
        return "Protocol: checking...";
    case EditorProtocolState::unsupported:
        return "Protocol: not available (Load/Pull only)";
    case EditorProtocolState::supported:
        return "Protocol: v" + String(editorProtocolVersion)
            + (isStoreSupported() ? " (Store, Position)" : " (limited)");
    }
    return {};
}


//==============================================================================
void Pfm2AudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml("PreenFM2AppStatus");

    xml.setAttribute("presetName", getPresetName().trim());
    xml.setAttribute("HardwarePresetBank", getHardwarePresetBank());
    xml.setAttribute("HardwarePresetNumber", getHardwarePresetNumber());
    xml.setAttribute("MpeEnabled", isMpeEnabled());

    // add some attributes to it..
    const auto& parameterSet = getParameters();
    for (int p = 0; p < parameterSet.size(); p++) {

        auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[p]);
		const int nrpnParam = midifiedFP->getNrpnParam();
		const bool hasHardwareNrpn = nrpnParam >= 0 && nrpnParam < nrpnLookupSize;

        // if pfm2 and no param, don't send value
        if (hasHardwareNrpn && pfmType == 1 && nrpmIndex[nrpnParam] == -1) {
            continue;
        }
        // if pfm3 and this is the pfm2 equivalent  although we have a specific pfm3 value (voices for example!)
        if (hasHardwareNrpn && pfmType == 2 && nrpmIndexPfm3[nrpnParam] != -1 && nrpmIndex[nrpnParam] == midifiedFP->getParamIndex()) {
            continue;
        }


        float realValue = midifiedFP->getRealValue();
        // Let's keep only 2 digit after comma
        const int iRealValue = static_cast<int>(realValue * 100.0f
            + (realValue < 0.0f ? -0.001f : 0.001f));
        realValue = (float)iRealValue / 100.0f;
            
        xml.setAttribute(midifiedFP->getNameForXML(), realValue);
        // DBG("GET > " << String(p) << " '" << midifiedFP->getNameForXML() << "'  value " << realValue << " param adress : " << (int)midifiedFP);

        // DBG(String(p) << " '" << midifiedFP->getNameForXML() << "'  value " << (midifiedFP->getRealValue()));
    }
    const int savedEditorWidth = editorWidth.load(std::memory_order_relaxed);
    const int savedEditorHeight = editorHeight.load(std::memory_order_relaxed);
    xml.setAttribute("EditorWidth", savedEditorWidth > 0 ? savedEditorWidth : 1000);
    xml.setAttribute("EditorHeight", savedEditorHeight > 0 ? savedEditorHeight : 750);

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}


void Pfm2AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    setStateInformation(data, sizeInBytes, true);
}

void Pfm2AudioProcessor::setStateInformation(const void* data, int sizeInBytes, bool flushNewStateToPreenfm) {
   
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    MidifiedFloatParameter* midifiedFP;

    if (xmlState != nullptr)
    {
        setPresetName(xmlState->getStringAttribute("presetName").trim());


        if (xmlState->hasTagName("PreenFM2AppStatus")) {
            // Older sessions always retain the original single-channel routing.
            setMpeEnabled(xmlState->getBoolAttribute("MpeEnabled", false), false);
            const auto& parameterSet = getParameters();

            setHardwarePresetTarget(
                xmlState->getIntAttribute("HardwarePresetBank", 1),
                xmlState->getIntAttribute("HardwarePresetNumber", 1));

            float value;
            for (int p = 0; p < parameterSet.size(); p++) {
                // End ?
                if (p == nrpmIndex[2046]) break;

                midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[p]);

                // Special case to fix compatilibty that was broken between 2.11.4 and 2.12
                // If "Mtx*Destination1" not found, seach for "Mtx*Destination" 
                String nameForXML = midifiedFP->getNameForXML();

                if (!xmlState->hasAttribute(nameForXML)) {
                    if (nameForXML.startsWith("Mtx") && nameForXML.endsWith("Destination1")) {
                        nameForXML = nameForXML.substring(0, nameForXML.length() - 1);
                    }
                }

                if (xmlState->hasAttribute(nameForXML)) {

                    value = jlimit(midifiedFP->getMin(), midifiedFP->getMax(),
                        (float)xmlState->getDoubleAttribute(nameForXML));
                    midifiedFP->setRealValueNoNotification(value);
                    DBG("SET > " << String(p) << " '" << nameForXML << "'  value " << (midifiedFP->getRealValue()) << " param adress : " <<(int)midifiedFP);
                }
            }

            if (xmlState->hasAttribute("EditorWidth")) {
                editorWidth.store(xmlState->getIntAttribute("EditorWidth"),
                    std::memory_order_relaxed);
            }
            if (xmlState->hasAttribute("EditorHeight")) {
                editorHeight.store(xmlState->getIntAttribute("EditorHeight"),
                    std::memory_order_relaxed);
            }

            if (xmlState->hasAttribute("pfmType")) {
                midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[nrpmIndex[2044]]);
                const int restoredPfmType = jlimit(1, 2,
                    roundToInt(midifiedFP->getRealValue()));
                midifiedFP->setRealValueNoNotification((float)restoredPfmType);
                setPfmType(restoredPfmType);
            }

            // (If pfmType (old preset) or preenfm2) AND NO PlayModepfm2
            // => we force playmode to poly
            if ((!xmlState->hasAttribute("pfmType") || pfmType == 1) && !xmlState->hasAttribute("PlayModepfm2")) {
                midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[nrpmIndex[PREENFM2_NRPN_GLIDE_TYPE]]);
                midifiedFP->setRealValueNoNotification(1.0f);
            }

            midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[nrpmIndex[2045]]);
            const int restoredMidiChannel = jlimit(1, 16,
                roundToInt(midifiedFP->getRealValue()));
            midifiedFP->setRealValueNoNotification((float)restoredMidiChannel);
            setControlChannel(restoredMidiChannel);

            requestEditorStateUpdate();

            // REDRAW UI
            for (int p = 0; p < parameterSet.size(); p++) {
                parameterUpdatedForUI(p);
            }

            // Start Flushing NRPN
            if (flushNewStateToPreenfm) {
                flushAllParametrsToNrpn();
            }
        }
    }
}

void Pfm2AudioProcessor::parameterUpdatedForUI(int p) {
    if (p < 0 || p >= maximumPendingUiParameters) {
        return;
    }

    const size_t wordIndex = static_cast<size_t>(p / 64);
    const uint64_t bit = uint64_t { 1 } << (p % 64);
    pendingUiParameterUpdates[wordIndex].fetch_or(bit, std::memory_order_release);
}

void Pfm2AudioProcessor::clearPendingUiParameterUpdate(int parameterIndex) noexcept {
    if (parameterIndex < 0 || parameterIndex >= maximumPendingUiParameters) {
        return;
    }

    const size_t wordIndex = static_cast<size_t>(parameterIndex / 64);
    const uint64_t bit = uint64_t { 1 } << (parameterIndex % 64);
    pendingUiParameterUpdates[wordIndex].fetch_and(~bit, std::memory_order_acq_rel);
}

void Pfm2AudioProcessor::consumePendingUiParameterUpdates(
    std::unordered_set<String>& updates) {
    const auto& parameters = getParameters();

    for (size_t wordIndex = 0; wordIndex < pendingUiParameterUpdates.size(); ++wordIndex) {
        const uint64_t pending = pendingUiParameterUpdates[wordIndex].exchange(
            0, std::memory_order_acq_rel);

        for (int bitIndex = 0; bitIndex < 64; ++bitIndex) {
            if ((pending & (uint64_t { 1 } << bitIndex)) == 0) {
                continue;
            }

            const int parameterIndex = static_cast<int>(wordIndex * 64)
                + bitIndex;
            if (parameterIndex >= parameters.size()) {
                continue;
            }

            auto* parameter = dynamic_cast<MidifiedFloatParameter*>(
                parameters[parameterIndex]);
            if (parameter != nullptr) {
                updates.insert(parameter->getName());
            }
        }
    }
}

void Pfm2AudioProcessor::flushAllParametrsToNrpn() {
    sendNrpnPresetName();

    const auto& parameterSet = getParameters();
    int p;
    for (p = 0; p < parameterSet.size(); p++) {
        // Pull/push button midi channel
		const int firstInternalParameterIndex = nrpmIndex[PREENFM_NRPN_PFMTYPE];
        if (firstInternalParameterIndex >= 0 && p >= firstInternalParameterIndex) {
			break;
        }
        auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameterSet[p]);

        if (midifiedFP != nullptr) {
			const int nrpnParam = midifiedFP->getNrpnParam();
			if (nrpnParam < 0 || nrpnParam >= nrpnLookupSize) {
				continue;
			}

            //if (midifiedFP->getNameForXML().startsWith("arp")) {
            //     DBG("ARP VALUE '" << midifiedFP->getNameForXML() << "'  value " << (midifiedFP->getRealValue()));
            //}

            // if pfm2 and no param, don't send value
            if (pfmType == 1 && nrpmIndex[nrpnParam] == -1) {
                continue;
            }

            // if pfm2 and this is the pfm3 equivalent (playMode for example!)
            // if (pfmType == 1 && nrpmIndexPfm3[midifiedFP->getNrpnParam()] == midifiedFP->getParamIndex()) {
            // Make it simple
            // If pfm2 and this is playMode don't send it
            if (pfmType == 1 && midifiedFP == playModeParam) {
                continue;
            }

            // if pfm3 and this is the pfm2 equivalent  although we have a specific pfm3 value (voices for example!)
            // Make it simple
            // If pfm3 and this is Voice : don't send it
            if (pfmType == 2 && midifiedFP == voicesParam) {
                continue;
            }

            queueParameterNrpn(midifiedFP);
        }
    }
}



bool Pfm2AudioProcessor::isRealtimePriority() const {
    return true;
}



/**
 * Values updated by the HOST
 * . Modify value
    . Send NRPN
    . Refresh UI
 */
void Pfm2AudioProcessor::hostParameterChanged(int index)
{
    const auto& parameters = getParameters();
    if (index < 0 || index >= parameters.size()) {
        return;
    }
    MidifiedFloatParameter* midifiedFP = static_cast<MidifiedFloatParameter*>(parameters[index]);

    if (index == nrpmIndex[2045]) {
        const int midiChannel = jlimit(1, 16,
            roundToInt(midifiedFP->getRealValue()));
        midifiedFP->setRealValueNoNotification((float)midiChannel);
        setControlChannel(midiChannel);
        parameterUpdatedForUI(index);
        return;
    }
    if (index == nrpmIndex[2044]) {
        setPfmType(roundToInt(midifiedFP->getRealValue()));
        parameterUpdatedForUI(index);
        return;
    }

    // send nrpn
    queueParameterNrpn(midifiedFP);
    // REDRAW UI
    parameterUpdatedForUI(index);
}


/**
 * Values updated by the PreenFM2 hardware
 * Here the values has to be modified
  . Modify Value
  . tell host
  . refresh UI
 */
void Pfm2AudioProcessor::handleIncomingNrpn(int param, int nrpnValue) {
    // NRPM from the preenFM2
	if (param < 0) {
		return;
	}

    // Editor remote protocol responses live on NRPN page 4 (parameters
    // 512..639) and are control answers, not plugin parameters. They are
    // intercepted before the parameter lookup so they can never be mistaken
    // for a synth parameter, a preset name letter or a step sequencer value.
    // This runs on the message thread: the MIDI callback only fed the value
    // into incomingNrpnFifo.
    if (param >= PREENFM_EDITOR_NRPN_FIRST
        && param <= PREENFM_EDITOR_NRPN_LAST) {
        handleEditorProtocolResponse(param - PREENFM_EDITOR_NRPN_FIRST,
            nrpnValue);
        return;
    }

	if (param >= nrpnLookupSize) {
		return;
	}

    if (param >= PREENFM2_NRPN_LETTER1 && param <= PREENFM2_NRPN_LETTER12) {
		setPresetName(presetNameWithCharacter(
			(int)(param - PREENFM2_NRPN_LETTER1), nrpnValue));
		return;
    }

    // Detect if we were in PFM2 mode and we received a PFM3 preset
    // Changes is late in the NRPN stream so there is some work to do
    if (param == PREENFM2_NRPN_VERSION) {
        // version have been multiplied by 100
        // 100 means version 1 means pfm3
        if (nrpnValue == 100 && pfmType == 1) {
			setPfmType(2);
            playModeParam->setValue(voicesParam->getValue());
        }
    }

    int index = nrpmIndex[param];
    if ((index == -1 || pfmType == 2) && nrpmIndexPfm3[param] != -1) {
        index = nrpmIndexPfm3[param];
    }

    if (index == -1) {
        // NRN Param not registered
        return;
    }
    sendMidiForParameter(index, nrpnValue);

}


/*
* Called from PfmPreset on a hidden PluginProcessor so no need to update UI
*/
void Pfm2AudioProcessor::setParameterWithNrpmParamAndRealValue(int param, float pfmValue) {
	if (param < 0 || param >= nrpnLookupSize) {
		return;
	}

    if (param >= PREENFM2_NRPN_LETTER1 && param <= PREENFM2_NRPN_LETTER12) {
        const int characterValue = roundToInt(pfmValue);
		setPresetName(presetNameWithCharacter(
			(int)(param - PREENFM2_NRPN_LETTER1), characterValue));
		return;
    }


    int index = nrpmIndex[param];
    if (pfmType == 2 && nrpmIndexPfm3[param] != -1) {
        index = nrpmIndexPfm3[param];
    }

    if (index == -1) {
        DBG("setParameterWithNrpmParamAndRealValue ERROR with param" << param);
        return;
    }

    const auto& parameters = getParameters();
	if (index < 0 || index >= parameters.size()) {
		return;
	}

    if (param == PREENFM2_NRPN_LFO1_KSYN || param == PREENFM2_NRPN_LFO2_KSYN || param == PREENFM2_NRPN_LFO3_KSYN) {
		if (index == 0) {
			return;
		}
        auto* midifiedComboFP = static_cast<MidifiedFloatParameter*>(parameters[index - 1]);
        if (pfmValue > 0.0f) {
            // Modify combo sync
            midifiedComboFP->setRealValueNoNotification(2.0f);
        }
        else {
            midifiedComboFP->setRealValueNoNotification(1.0f);
        }
    }


    auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameters[index]);
    midifiedFP->setPfmBankValue(pfmValue);
}

float Pfm2AudioProcessor::getRealValueForPfmBank(int param) {
	if (param < 0 || param >= nrpnLookupSize) {
		return 0;
	}

    if (param >= PREENFM2_NRPN_LETTER1 && param <= PREENFM2_NRPN_LETTER12) {
        const String currentPresetName = getPresetName();
        return static_cast<float>(currentPresetName[
            (int)(param - PREENFM2_NRPN_LETTER1)]);
    }

    int index = nrpmIndex[param];
    if (pfmType == 2 && nrpmIndexPfm3[param] != -1) {
        index = nrpmIndexPfm3[param];
    }

    if (index == -1) {
        DBG("getRealValueFromNrpmParam ERROR with param" << param);
        return 0;
    }

    const auto& parameters = getParameters();
	if (index < 0 || index >= parameters.size()) {
		return 0;
	}
    auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameters[index]);
    return midifiedFP->getPfmBankValue();
}

void Pfm2AudioProcessor::sendMidiForParameter(int paramIndex, int nrpnValue) {
    const auto& parameters = getParameters();
	if (paramIndex < 0 || paramIndex >= parameters.size()) {
		return;
	}
    auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameters[paramIndex]);
    if (midifiedFP != nullptr) {
        const auto notifyParameter = [this, &parameters] (int index) {
            if (index < 0 || index >= parameters.size()) {
                return;
            }

            auto* parameter = static_cast<MidifiedFloatParameter*>(parameters[index]);
            parameter->sendValueChangedMessageToListeners(parameter->getValue());
            parameterUpdatedForUI(index);
        };

        const auto setRealValueAndNotify = [&parameters, &notifyParameter]
            (int index, float realValue) {
                if (index < 0 || index >= parameters.size()) {
                    return;
                }

                auto* parameter = static_cast<MidifiedFloatParameter*>(parameters[index]);
				if (parameter->getRealValue() == realValue) {
					return;
				}
                parameter->setRealValueNoNotification(realValue);
                notifyParameter(index);
            };

        const auto setNrpnValueAndNotify = [&parameters, &notifyParameter]
            (int index, int rawNrpnValue) {
                if (index < 0 || index >= parameters.size()) {
                    return;
                }

                auto* parameter = static_cast<MidifiedFloatParameter*>(parameters[index]);
				const float realValue = parameter->getValueFromNrpn(rawNrpnValue);
				if (parameter->getRealValue() == realValue) {
					return;
				}
                parameter->setValueFromNrpn(rawNrpnValue);
                notifyParameter(index);
            };

        if (paramIndex > 0) {
            const int nrpnParameter = midifiedFP->getNrpnParam();
            const bool isLfoFrequency =
                nrpnParameter == PREENFM2_NRPN_LFO1_FREQUENCY
                || nrpnParameter == PREENFM2_NRPN_LFO2_FREQUENCY
                || nrpnParameter == PREENFM2_NRPN_LFO3_FREQUENCY;
            const bool isLfoKeySync =
                nrpnParameter == PREENFM2_NRPN_LFO1_KSYN
                || nrpnParameter == PREENFM2_NRPN_LFO2_KSYN
                || nrpnParameter == PREENFM2_NRPN_LFO3_KSYN;
            const bool isStepSequencerBpm =
                nrpnParameter == PREENFM2_NRPN_STEPSEQ1_BPM
                || nrpnParameter == PREENFM2_NRPN_STEPSEQ2_BPM;

            if (isLfoFrequency) {
                constexpr int internalLfoClockValue = 9990;
                constexpr int firstExternalLfoClockValue = 10000;
                constexpr int lastExternalLfoClockValue = 10080;
                constexpr int externalLfoClockStep = 10;
                if (nrpnValue >= firstExternalLfoClockValue) {
					const int clampedValue = jlimit(firstExternalLfoClockValue,
						lastExternalLfoClockValue, nrpnValue);
					const int normalizedValue = firstExternalLfoClockValue
						+ roundToInt((float)(clampedValue - firstExternalLfoClockValue)
							/ externalLfoClockStep) * externalLfoClockStep;
					setNrpnValueAndNotify(paramIndex - 1, normalizedValue);
					clearPendingUiParameterUpdate(paramIndex);
					return;
                }

				// 9991..9999 are not valid firmware sync IDs. Treat them as
				// the maximum internal frequency instead of leaving the combo empty.
				setRealValueAndNotify(paramIndex - 1,
					(float)internalLfoClockValue);
            }
            else if (isLfoKeySync) {
                setRealValueAndNotify(paramIndex - 1,
                    nrpnValue > 0 ? 2.0f : 1.0f);
            }
            else if (isStepSequencerBpm) {
                constexpr int internalStepSequencerClockValue = 240;
                if (nrpnValue >= internalStepSequencerClockValue) {
                    setNrpnValueAndNotify(paramIndex - 1, nrpnValue);
                    if (nrpnValue > internalStepSequencerClockValue) {
                        clearPendingUiParameterUpdate(paramIndex);
                        return;
                    }
                }
                else {
                    setRealValueAndNotify(paramIndex - 1,
                        (float)internalStepSequencerClockValue);
                }
            }
            else {
                const float unclampedValue =
                    midifiedFP->getUnclampedValueFromNrpn(nrpnValue);
                if (unclampedValue < midifiedFP->getMin()
                    || unclampedValue > midifiedFP->getMax()) {
					auto* previousParameter = static_cast<MidifiedFloatParameter*>(
						parameters[paramIndex - 1]);
					if (previousParameter->getNrpnParam() == nrpnParameter) {
						clearPendingUiParameterUpdate(paramIndex);
						setNrpnValueAndNotify(paramIndex - 1, nrpnValue);
						return;
					}
                }
            }
        }

        midifiedFP->setValueFromNrpn(nrpnValue);
        notifyParameter(paramIndex);
    }
}

/**
 * Values updated by the UI
 * Here values are already up to date
 . tell host
 . Send NRPN
 */
void Pfm2AudioProcessor::onParameterUpdated(AudioProcessorParameter *parameter) {


    auto* midifiedFP = static_cast<MidifiedFloatParameter*>(parameter);
    if (midifiedFP != nullptr) {
        int index = midifiedFP->getParamIndex();

        if (index == nrpmIndex[2046]) {
            // Push button
            flushAllParametrsToNrpn();
        }
        else if (index == nrpmIndex[2045]) {
            // Midi Channel changed
            const int midiChannel = jlimit(1, 16,
                roundToInt(midifiedFP->getRealValue()));
            midifiedFP->setRealValueNoNotification((float)midiChannel);
            setControlChannel(midiChannel);
        }
        else if (index == nrpmIndex[2044]) {
            // pfm type
            setPfmType((int)midifiedFP->getRealValue());
        }
        else if (index == nrpmIndex[2047]) {
            // Don't notify host
            // send nrpn
            queueParameterNrpn(midifiedFP);
        }
        else {
            // Notify host
            midifiedFP->sendValueChangedMessageToListeners(midifiedFP->getValue());

            // send nrpn
            queueParameterNrpn(midifiedFP);
            DBG("onParameterUpdated '" << midifiedFP->getNameForXML() << "'  value " << (midifiedFP->getRealValue()));
        }
    }
}

void Pfm2AudioProcessor::queueParameterNrpn(const MidifiedFloatParameter* parameter) {
    if (parameter == nullptr) {
        return;
    }

    pfm2MidiDevice->queueNrpn(jlimit(1, 16,
        currentMidiChannel.load(std::memory_order_relaxed)),
        parameter->getNrpnParam(), parameter->getNrpnValue());
}

void Pfm2AudioProcessor::setPresetName(String newName) {
    String asciiName;
    for (int index = 0;
        index < newName.length() && index < maximumPresetNameLength;
        ++index) {
        const juce_wchar character = newName[index];
        asciiName += String::charToString(
            character >= 32 && character <= 126 ? character : ' ');
    }

    {
        const ScopedLock lock(presetNameLock);
        presetName = asciiName;
    }

    auto* messageManager = MessageManager::getInstanceWithoutCreating();
    if (messageManager != nullptr && messageManager->isThisTheMessageThread()
        && pfm2Editor != nullptr) {
        pfm2Editor->setPresetName(asciiName);
    }
    else {
        requestEditorStateUpdate();
    }
}


String Pfm2AudioProcessor::getPresetName() const {
    const ScopedLock lock(presetNameLock);
    return presetName;
}


String Pfm2AudioProcessor::presetNameWithCharacter(
    int characterIndex, int characterValue) const {
	if (characterIndex < 0 || characterIndex >= maximumPresetNameLength) {
		return getPresetName();
	}

	const String currentName = getPresetName();
	String updatedName;
	for (int index = 0; index < maximumPresetNameLength; ++index) {
		juce_wchar character = index < currentName.length()
			? currentName[index] : ' ';
		if (index == characterIndex) {
			character = characterValue >= 32 && characterValue <= 126
				? (juce_wchar)characterValue : ' ';
		}
		else if (character < 32 || character > 126) {
			character = ' ';
		}

		updatedName += String::charToString(character);
	}

	return updatedName.trimEnd();
}


void Pfm2AudioProcessor::sendNrpnPresetName() {
    char nameToSend[maximumPresetNameLength + 1] = {};
    const String currentPresetName = getPresetName();
    for (int k = 0; k < currentPresetName.length() && k < maximumPresetNameLength; k++) {
        nameToSend[k] = static_cast<char>(currentPresetName[k]);
    }
    for (int k = 0; k < maximumPresetNameLength; k++) {
        const int letter = static_cast<unsigned char>(nameToSend[k]);
        pfm2MidiDevice->queueNrpn(jlimit(1, 16,
            currentMidiChannel.load(std::memory_order_relaxed)),
            PREENFM2_NRPN_LETTER1 + k, letter);
    }

}

void Pfm2AudioProcessor::addMidifiedParameter(MidifiedFloatParameter *param) {
    addParameter(param);
    param->setProcessor(this);
}


void Pfm2AudioProcessor::handleIncomingMidiMessage(MidiInput*, const MidiMessage &midiMessage) {

    if (midiMessage.isController()
        && midiMessage.getChannel() == jlimit(1, 16,
            currentMidiChannel.load(std::memory_order_relaxed))) {
        // DBG("Midi message " << midiMessage.getControllerNumber() << " , " << midiMessage.getControllerValue() << "\n");
        switch (midiMessage.getControllerNumber()) {
        case 99:
            currentNrpn.paramMSB = static_cast<uint8>(midiMessage.getControllerValue());
			currentNrpn.hasParamMSB = true;
            break;
        case 98:
            currentNrpn.paramLSB = static_cast<uint8>(midiMessage.getControllerValue());
			currentNrpn.hasParamLSB = true;
            break;
        case 6:
            currentNrpn.valueMSB = static_cast<uint8>(midiMessage.getControllerValue());
			currentNrpn.hasValueMSB = true;
            break;
        case 38:
        {
            currentNrpn.valueLSB = static_cast<uint8>(midiMessage.getControllerValue());
			if (!currentNrpn.hasParamMSB || !currentNrpn.hasParamLSB || !currentNrpn.hasValueMSB) {
				break;
			}
            int param = (int)(currentNrpn.paramMSB << 7) + currentNrpn.paramLSB;
            int value = (int)(currentNrpn.valueMSB << 7) + currentNrpn.valueLSB;

            queueIncomingNrpn(param, value);

            // Editor page 4 commands are consumed, exactly as the firmware
            // consumes them: a second lone CC38 must not repeat the same
            // response. The historic behaviour of the ordinary parameter
            // pages, where the address and value state deliberately survive
            // several data entry messages, is left untouched.
            if (currentNrpn.paramMSB == PREENFM_EDITOR_NRPN_PAGE) {
                currentNrpn.hasValueMSB = false;
            }
            break;
        }
        }
    }
}

void Pfm2AudioProcessor::queueIncomingNrpn(int parameter, int value) noexcept {
    {
        const auto writeScope = incomingNrpnFifo.write(1);
        const int index = writeScope.blockSize1 > 0
            ? writeScope.startIndex1
            : (writeScope.blockSize2 > 0 ? writeScope.startIndex2 : -1);

        if (index < 0) {
            ++droppedIncomingNrpnEvents;
            return;
        }

        incomingNrpnQueue[static_cast<size_t>(index)] = { parameter, value };
    }

    triggerAsyncUpdate();
}

void Pfm2AudioProcessor::handleAsyncUpdate() {
    const bool changedContext = protocolContextChanged.exchange(false, std::memory_order_acq_rel);
    if (changedContext) invalidateProtocolContext();
    for (;;) {
        IncomingNrpnEvent event;
        bool hasEvent = false;
        {
            const auto readScope = incomingNrpnFifo.read(1);
            const int index = readScope.blockSize1 > 0
                ? readScope.startIndex1
                : (readScope.blockSize2 > 0 ? readScope.startIndex2 : -1);
            if (index >= 0) {
                event = incomingNrpnQueue[static_cast<size_t>(index)];
                hasEvent = true;
            }
        }

        if (!hasEvent) {
            break;
        }

        // Queued answers belonged to the previous channel/context.
        if (!changedContext) handleIncomingNrpn(event.parameter, event.value);
    }

    if (pendingEditorStateUpdate.exchange(false, std::memory_order_acq_rel)
        && pfm2Editor != nullptr) {
        const int savedEditorWidth = editorWidth.load(std::memory_order_relaxed);
        const int savedEditorHeight = editorHeight.load(std::memory_order_relaxed);
        if (savedEditorWidth > 0 && savedEditorHeight > 0) {
            pfm2Editor->setSize(
                jmax(savedEditorWidth, Pfm2AudioProcessorEditor::minimumWidth),
                jmax(savedEditorHeight, Pfm2AudioProcessorEditor::minimumHeight));
        }
        pfm2Editor->setPfmType(pfmType.load(std::memory_order_relaxed));
        pfm2Editor->setMidiChannel(jlimit(1, 16,
            currentMidiChannel.load(std::memory_order_relaxed)));
        pfm2Editor->setPresetName(getPresetName());
    }
    if (changedContext) requestEditorCapabilities();
}

void Pfm2AudioProcessor::requestEditorStateUpdate() noexcept {
    pendingEditorStateUpdate.store(true, std::memory_order_release);
    triggerAsyncUpdate();
}

void Pfm2AudioProcessor::handlePartialSysexMessage(MidiInput*, const uint8*, int, double) {

}

void Pfm2AudioProcessor::choseNewMidiDevice() {
    pfm2MidiDevice->forceChoseNewDevices();

    // Anything in flight belonged to the previous device generation and has
    // been discarded by the device layer; a store in flight becomes unknown.
    if (transactionToken != Pfm2MidiDevice::invalidTransactionToken) {
        abandonTransactionForDeviceChange();
    }

    // A different port may lead to a different instrument, so anything we
    // knew about the protocol and the hardware position is stale now.
    editorProtocolState = EditorProtocolState::unknown;
    editorProtocolVersion = 0;
    editorCapabilities = 0;
    reportedBank = 0;
    reportedPreset = 0;
    reportedPositionValid = false;
    requestEditorCapabilities();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfm2AudioProcessor();
}
