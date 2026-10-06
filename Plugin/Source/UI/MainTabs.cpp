/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
/*
* Copyright 2014 Xavier Hosxe
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
#include "JuceHeader.h"
#include "../PluginProcessor.h"
#include "PanelEngine.h"
#include "PanelModulation.h"
#include "PanelArpAndFilter.h"
//[/Headers]

#include "MainTabs.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace {

class HardwarePresetBrowser final : public Component,
                                    private Timer
{
public:
    explicit HardwarePresetBrowser(Pfm2AudioProcessor& processorToUse)
        : processor(processorToUse)
    {
        titleLabel.setText(TRANS("Hardware"), dontSendNotification);
        titleLabel.setFont(Font(FontOptions(16.0f, Font::bold)));
        titleLabel.setJustificationType(Justification::centredLeft);
        addAndMakeVisible(titleLabel);

        configureCaption(bankLabel, TRANS("Bank"));
        configureCaption(presetLabel, TRANS("Preset"));

        // Only 64 regular patch banks are addressable on the hardware
        // (NUMBEROFPREENFMBANKS); bank 65..128 can never exist.
        for (int number = 1; number <= PREENFM_EDITOR_BANK_COUNT; ++number) {
            bankCombo.addItem(String(number), number);
        }
        for (int number = 1; number <= PREENFM_EDITOR_PRESET_COUNT; ++number) {
            presetCombo.addItem(String(number), number);
        }

        bankCombo.setTooltip(TRANS(
            "Native PreenFM bank number 1-64 (the MIDI protocol sends this zero-based)"));
        presetCombo.setTooltip(TRANS(
            "Preset number 1-128 (the MIDI protocol sends this zero-based)"));
        bankCombo.setSelectedId(processor.getHardwarePresetBank(),
            dontSendNotification);
        presetCombo.setSelectedId(processor.getHardwarePresetNumber(),
            dontSendNotification);
        bankCombo.onChange = [this] { targetChanged(); };
        presetCombo.onChange = [this] { targetChanged(); };
        addAndMakeVisible(bankCombo);
        addAndMakeVisible(presetCombo);

        previousButton.setButtonText(TRANS("<"));
        previousButton.setTooltip(TRANS("Load the previous preset"));
        previousButton.onClick = [this] { loadRelativePreset(-1); };
        addAndMakeVisible(previousButton);

        nextButton.setButtonText(TRANS(">"));
        nextButton.setTooltip(TRANS("Load the next preset"));
        nextButton.onClick = [this] { loadRelativePreset(1); };
        addAndMakeVisible(nextButton);

        loadButton.setButtonText(TRANS("Load"));
        loadButton.setTooltip(TRANS(
            "Select this bank/preset on the hardware and pull it into the editor"));
        loadButton.onClick = [this] { loadSelectedPreset(); };
        addAndMakeVisible(loadButton);

        storeButton.setButtonText(TRANS("Store"));
        storeButton.setEnabled(false);
        storeButton.setTooltip(TRANS(
            "Write the current patch into the selected slot (editor protocol v1 firmware)"));
        storeButton.onClick = [this] { confirmAndStore(); };
        addAndMakeVisible(storeButton);

        positionButton.setButtonText(TRANS("Position"));
        positionButton.setTooltip(TRANS(
            "Ask the hardware which bank and preset it is currently on"));
        positionButton.onClick = [this] { processor.requestHardwarePosition(); };
        addAndMakeVisible(positionButton);

        mpeToggle.setButtonText("MPE");
        mpeToggle.setTooltip("Preserve performance MIDI channels for MPE. Enable the hardware zone first. "
            "Editor channel = MPE manager = MIDI channel of the MPE timbre. "
            "Configure zone and bend range on hardware/controller: host RPN/NRPN configuration is blocked for PreenFM2.");
        mpeToggle.setToggleState(processor.isMpeEnabled(), dontSendNotification);
        mpeToggle.onClick = [this] { processor.setMpeEnabled(mpeToggle.getToggleState()); };
        addAndMakeVisible(mpeToggle);

        configureInfo(targetInfoLabel);
        configureInfo(positionInfoLabel);
        configureInfo(protocolInfoLabel);
        configureInfo(operationLabel);
        // Running operation or last error, kept visually distinct.
        operationLabel.setColour(Label::textColourId, Colour(0xffffc46b));
        operationLabel.setJustificationType(Justification::topLeft);

        refreshFromProcessor();

        // The permanent header replaces the former Presets callout. Checking
        // once when it is created keeps the Store and Position state current
        // without requiring a separate Protocol button.
        processor.requestEditorCapabilities();

        setSize(900, 66);
        startTimerHz(12);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colour(0xff101c25));
        g.setColour(Colour(0xff29404f));
        g.drawHorizontalLine(getHeight() - 1, 0.0f,
            static_cast<float>(getWidth()));
    }

    void resized() override
    {
        constexpr int margin = 12;
        constexpr int gap = 6;
        constexpr int controlHeight = 24;
        int x = margin;

        titleLabel.setBounds(x, 5, 76, controlHeight);
        x += 80;
        bankLabel.setBounds(x, 5, 36, controlHeight);
        x += 36;
        bankCombo.setBounds(x, 5, 58, controlHeight);
        x += 58 + gap;
        presetLabel.setBounds(x, 5, 46, controlHeight);
        x += 46;
        previousButton.setBounds(x, 5, 28, controlHeight);
        x += 28 + 3;
        presetCombo.setBounds(x, 5, 58, controlHeight);
        x += 58 + 3;
        nextButton.setBounds(x, 5, 28, controlHeight);
        x += 28 + 10;
        loadButton.setBounds(x, 5, 58, controlHeight);
        x += 58 + gap;
        storeButton.setBounds(x, 5, 58, controlHeight);
        x += 58 + gap;
        positionButton.setBounds(x, 5, 72, controlHeight);
        mpeToggle.setBounds(x + 84, 5, 82, controlHeight);

        const int secondRowY = 34;
        const int available = getWidth() - margin * 2;
        const int targetWidth = jlimit(130, 180, available / 5);
        const int positionWidth = jlimit(160, 230, available / 4);
        const int protocolWidth = jlimit(150, 210, available / 5);
        x = margin;
        targetInfoLabel.setBounds(x, secondRowY, targetWidth, 22);
        x += targetWidth + gap;
        positionInfoLabel.setBounds(x, secondRowY, positionWidth, 22);
        x += positionWidth + gap;
        protocolInfoLabel.setBounds(x, secondRowY, protocolWidth, 22);
        x += protocolWidth + gap;
        operationLabel.setBounds(x, secondRowY,
            jmax(0, getWidth() - margin - x), 22);
    }

private:
    void configureCaption(Label& label, const String& text)
    {
        label.setText(text, dontSendNotification);
        label.setJustificationType(Justification::centredLeft);
        addAndMakeVisible(label);
    }

    void configureInfo(Label& label)
    {
        label.setJustificationType(Justification::centredLeft);
        label.setMinimumHorizontalScale(0.75f);
        label.setColour(Label::textColourId, Colour(0xffd6e4ec));
        addAndMakeVisible(label);
    }

    void targetChanged()
    {
        processor.setHardwarePresetTarget(bankCombo.getSelectedId(),
            presetCombo.getSelectedId());
        refreshFromProcessor();
    }

    void loadSelectedPreset()
    {
        targetChanged();
        processor.loadHardwarePreset(processor.getHardwarePresetBank(),
            processor.getHardwarePresetNumber());
        refreshFromProcessor();
    }

    void loadRelativePreset(int delta)
    {
        const int newPreset = jlimit(1, PREENFM_EDITOR_PRESET_COUNT,
            presetCombo.getSelectedId() + delta);
        presetCombo.setSelectedId(newPreset, dontSendNotification);
        loadSelectedPreset();
    }

    void confirmAndStore()
    {
        targetChanged();
        const int bank = processor.getHardwarePresetBank();
        const int preset = processor.getHardwarePresetNumber();

        // The firmware writes immediately and without any confirmation of its
        // own, so the only chance to warn about overwriting a slot is here.
        // Asynchronous, so no modal loop is entered.
        Component::SafePointer<HardwarePresetBrowser> safeThis(this);
        AlertWindow::showOkCancelBox(MessageBoxIconType::WarningIcon,
            TRANS("Overwrite hardware preset?"),
            TRANS("This overwrites bank ") + String(bank)
                + TRANS(", preset ") + String(preset)
                + TRANS(" on the PreenFM immediately. The current editor patch "
                    "is pushed first and then written. This cannot be undone."),
            TRANS("Store"), TRANS("Cancel"), this,
            ModalCallbackFunction::create(
                [safeThis, bank, preset](int result) {
                    if (result == 1 && safeThis != nullptr) {
                        safeThis->processor.beginHardwareStore(bank, preset);
                        safeThis->refreshFromProcessor();
                    }
                }));
    }

    void timerCallback() override
    {
        mpeToggle.setToggleState(processor.isMpeEnabled(), dontSendNotification);
        mpeToggle.setEnabled(!processor.isHardwareBusy());
        const int revision = processor.getProtocolRevision();
        const auto suppressed = processor.getSuppressedConfigurationCount();
        if (revision != lastSeenRevision || suppressed != lastSuppressedConfigurationCount) {
            lastSuppressedConfigurationCount = suppressed;
            lastSeenRevision = revision;
            refreshFromProcessor();
        }
    }

    void refreshFromProcessor()
    {
        lastSeenRevision = processor.getProtocolRevision();

        targetInfoLabel.setText("Target: bank "
            + String(processor.getHardwarePresetBank()) + ", preset "
            + String(processor.getHardwarePresetNumber()),
            dontSendNotification);

        if (processor.isReportedHardwarePositionValid()) {
            positionInfoLabel.setText("Hardware: bank "
                + String(processor.getReportedHardwareBank()) + ", preset "
                + String(processor.getReportedHardwarePreset()),
                dontSendNotification);
        }
        else {
            positionInfoLabel.setText(
                TRANS("Hardware: position unknown"), dontSendNotification);
        }

        protocolInfoLabel.setText(processor.getEditorProtocolStatusText(),
            dontSendNotification);

        const String operation = processor.getLastOperationText();
        operationLabel.setText(operation + (lastSuppressedConfigurationCount > 0
            ? " Host RPN/NRPN configuration blocked (set MPE on hardware)." : ""), dontSendNotification);

        const bool busy = processor.isHardwareBusy();
        // Store and Load are locked against double clicks and against each
        // other while a transaction is open. After a store with an unknown
        // outcome, Store stays off until Position has resynchronised.
        storeButton.setEnabled(processor.canStartStore());
        loadButton.setEnabled(!busy);
        previousButton.setEnabled(!busy);
        nextButton.setEnabled(!busy);
        positionButton.setEnabled(processor.isPositionQuerySupported() && !busy);

        if (processor.isStoreBlockedByUnknownOutcome()) {
            storeButton.setTooltip(TRANS(
                "Blocked: a previous store had an unknown outcome. Use Position "
                "to resynchronise and check the slot on the hardware first."));
        }
        else {
            storeButton.setTooltip(processor.isStoreSupported()
                ? TRANS("Write the current patch into the selected slot")
                : TRANS("Needs editor protocol v1 firmware with Receives: NRPN or CC & NRPN"));
        }
    }

    Pfm2AudioProcessor& processor;
    Label titleLabel;
    Label bankLabel;
    Label presetLabel;
    Label targetInfoLabel;
    Label positionInfoLabel;
    Label protocolInfoLabel;
    Label operationLabel;
    ComboBox bankCombo;
    ComboBox presetCombo;
    TextButton previousButton;
    TextButton nextButton;
    TextButton loadButton;
    TextButton storeButton;
    TextButton positionButton;
    ToggleButton mpeToggle;
    uint64_t lastSuppressedConfigurationCount = 0;
    int lastSeenRevision = -1;
};

}
//[/MiscUserDefs]

//==============================================================================
MainTabs::MainTabs ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    tabbedComponent.reset (new TabbedComponent (TabbedButtonBar::TabsAtTop));
    addAndMakeVisible (tabbedComponent.get());
    tabbedComponent->setTabBarDepth (38);
    tabbedComponent->addTab (TRANS("Engine"), Colour (0xff0c151d), new PanelEngine(), true);
    tabbedComponent->addTab (TRANS("Modulation"), Colour (0xff0d1620), new PanelModulation(), true);
    tabbedComponent->addTab (TRANS("Arp & Filter"), Colour (0xff0f1720), new PanelArpAndFilter(), true);
    tabbedComponent->setCurrentTabIndex (0);

    pullButton.reset (new TextButton ("pull button"));
    addAndMakeVisible (pullButton.get());
    pullButton->setTooltip (TRANS("Pull all parameters from the preenfm2 to this plugin"));
    pullButton->setButtonText (TRANS("Pull"));
    pullButton->addListener (this);

    presetNameLabel.reset (new Label ("preset name label",
                                      TRANS("preset_89ABC")));
    addAndMakeVisible (presetNameLabel.get());
    presetNameLabel->setTooltip (TRANS("Click to edit"));
    presetNameLabel->setFont (Font (FontOptions (20.00f, Font::plain)
                                      .withMetricsKind (TypefaceMetricsKind::legacy))
                                      .withTypefaceStyle ("Bold"));
    presetNameLabel->setJustificationType (Justification::centredLeft);
    presetNameLabel->setEditable (true, true, false);
    presetNameLabel->setColour (Label::textColourId, Colours::aliceblue);
    presetNameLabel->setColour (TextEditor::textColourId, Colours::aliceblue);
    presetNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    presetNameLabel->setColour (TextEditor::highlightColourId, Colours::coral);
    presetNameLabel->addListener (this);

    presetNameLabel->setBounds (366, 8, 183, 24);

    pushButton.reset (new TextButton ("push button"));
    addAndMakeVisible (pushButton.get());
    pushButton->setTooltip (TRANS("Push all parameters from plugin to preenfm2"));
    pushButton->setButtonText (TRANS("Push"));
    pushButton->addListener (this);

    midiChannelCombo.reset (new ComboBox ("Midi Channel"));
    addAndMakeVisible (midiChannelCombo.get());
    midiChannelCombo->setTooltip ("Editor/control channel. In normal mode, performance MIDI uses this channel too. "
        "With MPE enabled, performance channels are preserved. Editor channel must equal both "
        "the MPE manager and the MIDI channel of the MPE timbre.");
    midiChannelCombo->setEditableText (false);
    midiChannelCombo->setJustificationType (Justification::centred);
    midiChannelCombo->setTextWhenNothingSelected (TRANS("1"));
    midiChannelCombo->setTextWhenNoChoicesAvailable (TRANS("1"));
    midiChannelCombo->addItem (TRANS("1"), 1);
    midiChannelCombo->addItem (TRANS("2"), 2);
    midiChannelCombo->addItem (TRANS("3"), 3);
    midiChannelCombo->addItem (TRANS("4"), 4);
    midiChannelCombo->addItem (TRANS("5"), 5);
    midiChannelCombo->addItem (TRANS("6"), 6);
    midiChannelCombo->addItem (TRANS("7"), 7);
    midiChannelCombo->addItem (TRANS("8"), 8);
    midiChannelCombo->addItem (TRANS("9"), 9);
    midiChannelCombo->addItem (TRANS("10"), 10);
    midiChannelCombo->addItem (TRANS("11"), 11);
    midiChannelCombo->addItem (TRANS("12"), 12);
    midiChannelCombo->addItem (TRANS("13"), 13);
    midiChannelCombo->addItem (TRANS("14"), 14);
    midiChannelCombo->addItem (TRANS("15"), 15);
    midiChannelCombo->addItem (TRANS("16"), 16);
    midiChannelCombo->addSeparator();
    midiChannelCombo->addListener (this);

    deviceButton.reset (new TextButton ("Device Button"));
    addAndMakeVisible (deviceButton.get());
    deviceButton->setButtonText (TRANS("Midi"));
    deviceButton->addListener (this);

    versionButton.reset (new HyperlinkButton (TRANS("v?.?.?"),
                                              URL ("https://github.com/Ixox/preenfm2Controller")));
    addAndMakeVisible (versionButton.get());
    versionButton->setTooltip (TRANS("https://github.com/Ixox/preenfm2Controller"));
    versionButton->setButtonText (TRANS("v?.?.?"));

    pfmTypeCombo.reset (new ComboBox ("pfm Type"));
    addAndMakeVisible (pfmTypeCombo.get());
    pfmTypeCombo->setTooltip (TRANS("preenfm type"));
    pfmTypeCombo->setEditableText (false);
    pfmTypeCombo->setJustificationType (Justification::centred);
    pfmTypeCombo->setTextWhenNothingSelected (TRANS("pfm2"));
    pfmTypeCombo->setTextWhenNoChoicesAvailable (TRANS("pfm2"));
    pfmTypeCombo->addItem (TRANS("pfm2"), 1);
    pfmTypeCombo->addItem (TRANS("pfm3"), 2);
    pfmTypeCombo->addListener (this);

    pfmTypeCombo->setBounds (276, 8, 80, 24);


    //[UserPreSize]
    pfmTypeCombo->setSelectedId(2);

	midiChannelCombo->setSelectedId(1);
    pfmTypeCombo->setSelectedId(1);
    versionButton->setButtonText(String("v") + ProjectInfo::versionString);
    //[/UserPreSize]

    setSize (900, 710);


    //[Constructor] You can add your own custom stuff here..
	panelEngine = ((PanelEngine*)tabbedComponent->getTabContentComponent(0));
	panelModulation = ((PanelModulation*)tabbedComponent->getTabContentComponent(1));
	panelArpAndFilter = ((PanelArpAndFilter*)tabbedComponent->getTabContentComponent(2));
	pullButtonValue = 0;
	currentMidiChannel = 1;
	pullButtonValue = 0;
	pushButtonValue = 0;
	// SET null !
    //[/Constructor]
}

MainTabs::~MainTabs()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    hardwarePresetBrowser = nullptr;
    tabbedComponent = nullptr;
    pullButton = nullptr;
    presetNameLabel = nullptr;
    pushButton = nullptr;
    midiChannelCombo = nullptr;
    deviceButton = nullptr;
    versionButton = nullptr;
    pfmTypeCombo = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainTabs::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff0b1117));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainTabs::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    constexpr int presetHeaderHeight = 66;
    if (hardwarePresetBrowser != nullptr)
        hardwarePresetBrowser->setBounds (0, 0, getWidth(), presetHeaderHeight);
    tabbedComponent->setBounds (0, presetHeaderHeight, getWidth(),
        jmax (0, getHeight() - presetHeaderHeight));
    const int toolbarY = presetHeaderHeight + 8;
    pullButton->setBounds (getWidth() - 116, toolbarY, 50, 24);
    pushButton->setBounds (getWidth() - 172, toolbarY, 50, 24);
    midiChannelCombo->setBounds (getWidth() - 222, toolbarY, 44, 24);
    deviceButton->setBounds (getWidth() - 282, toolbarY - 2, 54, 28);
    versionButton->setBounds (getWidth() - 60, toolbarY + 1, 56, 20);
	pfmTypeCombo->setBounds (350, toolbarY, 80, 24);
	presetNameLabel->setBounds (440, toolbarY,
        jmax (90, getWidth() - 798), 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainTabs::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == pullButton.get())
    {
        //[UserButtonCode_pullButton] -- add your button handler code here..

		MidifiedFloatParameter* param = getParameterFromName("pull button");
		pullButtonValue = (pullButtonValue == 1.0f ? 0.0f : 1.0f);
		param->setRealValue(pullButtonValue);

        //[/UserButtonCode_pullButton]
    }
    else if (buttonThatWasClicked == pushButton.get())
    {
        //[UserButtonCode_pushButton] -- add your button handler code here..

		MidifiedFloatParameter* param = getParameterFromName("push button");
		pushButtonValue = (pushButtonValue == 1.0f ? 0.0f : 1.0f);
		param->setRealValue(pushButtonValue);

        //[/UserButtonCode_pushButton]
    }
    else if (buttonThatWasClicked == deviceButton.get())
    {
        //[UserButtonCode_deviceButton] -- add your button handler code here..
		Pfm2AudioProcessor* pfm2Processor = dynamic_cast<Pfm2AudioProcessor*>(audioProcessor);
		if (pfm2Processor) {
			pfm2Processor->choseNewMidiDevice();
		}
        //[/UserButtonCode_deviceButton]
    }
    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MainTabs::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == presetNameLabel.get())
    {
        //[UserLabelCode_presetNameLabel] -- add your label text handling code here..
		Pfm2AudioProcessor* pfm2Processor = dynamic_cast<Pfm2AudioProcessor*>(audioProcessor);
		if (pfm2Processor) {
			pfm2Processor->setPresetName(presetNameLabel->getText());
			pfm2Processor->sendNrpnPresetName();
		}

        //[/UserLabelCode_presetNameLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void MainTabs::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == midiChannelCombo.get())
    {
        //[UserComboBoxCode_midiChannelCombo] -- add your combo box handling code here..

		MidifiedFloatParameter* param = getParameterFromName("Midi Channel");

		if (currentMidiChannel != midiChannelCombo->getSelectedId()) {
			currentMidiChannel = midiChannelCombo->getSelectedId();
			param->setRealValue((float)currentMidiChannel);
		}

        //[/UserComboBoxCode_midiChannelCombo]
    }
    else if (comboBoxThatHasChanged == pfmTypeCombo.get())
    {
        //[UserComboBoxCode_pfmTypeCombo] -- add your combo box handling code here..
        pfmType = pfmTypeCombo->getSelectedId();
        MidifiedFloatParameter* param = getParameterFromName("pfm Type");
        param->setRealValue((float)pfmType);

        panelEngine->setPfmType(pfmTypeCombo->getSelectedId());
        panelEngine->resized();
        panelEngine->repaint();
        panelModulation->setPfmType(pfmTypeCombo->getSelectedId());
        panelModulation->resized();
        panelModulation->repaint();
        panelArpAndFilter->setPfmType(pfmTypeCombo->getSelectedId());
        panelArpAndFilter->resized();
        panelArpAndFilter->repaint();


        //[/UserComboBoxCode_pfmTypeCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

MidifiedFloatParameter* MainTabs::getParameterFromName(String requestedName) {
	const auto& parameters = audioProcessor->getParameters();
	for (int p = 0; p < parameters.size(); p++) {
		auto* midiFP = static_cast<MidifiedFloatParameter*>(parameters[p]);
		if (midiFP->getName() == requestedName) {
			return midiFP;
		}
	}
	return nullptr;
}

void MainTabs::setMidiQueueWarning(uint64_t droppedOutput, uint64_t droppedInput) {
	if (droppedOutput == 0 && droppedInput == 0) {
		deviceButton->setButtonText(TRANS("Midi"));
		return;
	}

	deviceButton->setButtonText(TRANS("Midi !"));
	deviceButton->setTooltip(
		"MIDI data loss: " + String(droppedOutput)
		+ " outgoing and " + String(droppedInput)
		+ " incoming queue events were rejected. Click to check the MIDI devices.");
}

void MainTabs::buildParameters(AudioProcessor *processor) {
	audioProcessor = processor;

	if (auto* pfm2Processor = dynamic_cast<Pfm2AudioProcessor*>(processor)) {
		hardwarePresetBrowser = std::make_unique<HardwarePresetBrowser>(
			*pfm2Processor);
		addAndMakeVisible(*hardwarePresetBrowser);
		resized();
	}

	panelEngine->setParameterSet(processor);
	panelEngine->buildParameters();

	panelModulation->setParameterSet(processor);
	panelModulation->buildParameters();

	panelArpAndFilter->setParameterSet(processor);
	panelArpAndFilter->buildParameters();
}

void MainTabs::updateUI(std::unordered_set<String> &paramSet) {

    std::unordered_set<String>::const_iterator pfmTypeUpdate = paramSet.find("pfm Type");
    if (pfmTypeUpdate != paramSet.end()) {
        MidifiedFloatParameter* param = getParameterFromName("pfm Type");
        pfmTypeCombo->setSelectedId((int)param->getRealValue());
    }


	std::unordered_set<String>::const_iterator midiChannel = paramSet.find("Midi Channel");
	if (midiChannel != paramSet.end()) {
		MidifiedFloatParameter* param = getParameterFromName("Midi Channel");
		midiChannelCombo->setSelectedId((int)param->getRealValue());
	}

	panelEngine->updateUI(paramSet);
	panelModulation->updateUI(paramSet);
	panelArpAndFilter->updateUI(paramSet);
}


void MainTabs::setPresetName(String presetName) {
	presetNameLabel->setText(presetName, dontSendNotification);
}

void MainTabs::setPresetNamePtr(char* nameBuffer) {
	presetNamePtr = nameBuffer;
}

void MainTabs::setMidiChannel(int newMidiChannel) {
	midiChannelCombo->setSelectedId(newMidiChannel);
}

void MainTabs::setPfmType(int newPfmType) {
    pfmTypeCombo->setSelectedId(newPfmType);
    panelEngine->setPfmType(newPfmType);
    panelModulation->setPfmType(newPfmType);
    panelArpAndFilter->setPfmType(newPfmType);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainTabs" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="900" initialHeight="710">
  <BACKGROUND backgroundColour="ff0b232a"/>
  <TABBEDCOMPONENT name="new tabbed component" id="f175981f6c34a740" memberName="tabbedComponent"
                   virtualName="TabbedComponent" explicitFocusOrder="0" pos="0 10 0M 0M"
                   orientation="top" tabBarDepth="30" initialTab="0">
    <TAB name="Engine" colour="ff083543" useJucerComp="0" contentClassName="PanelEngine"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Modulation" colour="ff083543" useJucerComp="0" contentClassName="PanelModulation"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Arp &amp; Filter" colour="ff083543" useJucerComp="0" contentClassName="PanelArpAndFilter"
         constructorParams="" jucerComponentFile=""/>
  </TABBEDCOMPONENT>
  <TEXTBUTTON name="pull button" id="9da85c0691256028" memberName="pullButton"
              virtualName="" explicitFocusOrder="0" pos="116R 8 55 24" tooltip="Pull all parameters from the preenfm2 to this plugin"
              buttonText="Pull" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="preset name label" id="4201f054ae2edbe" memberName="presetNameLabel"
         virtualName="" explicitFocusOrder="0" pos="366 8 183 24" tooltip="Click to edit"
         textCol="fff0f8ff" edTextCol="fff0f8ff" edBkgCol="0" hiliteCol="ffff7f50"
         labelText="preset_89ABC" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default font" fontsize="20.0"
         kerning="0.0" bold="1" italic="0" justification="33" typefaceStyle="Bold"/>
  <TEXTBUTTON name="push button" id="52c3034a926a2609" memberName="pushButton"
              virtualName="" explicitFocusOrder="0" pos="184R 8 55 24" tooltip="Push all parameters from plugin to preenfm2"
              buttonText="Push" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="Midi Channel" id="a2c1c2de24e3a5a3" memberName="midiChannelCombo"
            virtualName="" explicitFocusOrder="0" pos="254R 8 55 24" tooltip="Midi Channel"
            editable="0" layout="36" items="1&#10;2&#10;3&#10;4&#10;5&#10;6&#10;7&#10;8&#10;9&#10;10&#10;11&#10;12&#10;13&#10;14&#10;15&#10;16&#10;"
            textWhenNonSelected="1" textWhenNoItems="1"/>
  <TEXTBUTTON name="Device Button" id="69cb4ea6d744571b" memberName="deviceButton"
              virtualName="" explicitFocusOrder="0" pos="330R 6 67 28" bgColOff="5c5da4"
              bgColOn="ff000000" buttonText="Midi" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <HYPERLINKBUTTON name="Version Button" id="9900f519cfc9db3b" memberName="versionButton"
                   virtualName="" explicitFocusOrder="0" pos="60R 9 56 20" tooltip="https://github.com/Ixox/preenfm2Controller"
                   textCol="fff5f5dc" buttonText="v?.?.?" connectedEdges="0" needsCallback="0"
                   radioGroupId="0" url="https://github.com/Ixox/preenfm2Controller"/>
  <COMBOBOX name="pfm Type" id="22e9370428be2245" memberName="pfmTypeCombo"
            virtualName="" explicitFocusOrder="0" pos="276 8 80 24" tooltip="preenfm type"
            editable="0" layout="36" items="pfm2&#10;pfm3" textWhenNonSelected="pfm2"
            textWhenNoItems="pfm2"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
