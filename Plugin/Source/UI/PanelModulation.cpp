/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

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

#include "JuceHeader.h"
#include "SliderPfm2.h"
#include "../MidifiedFloatParameter.h"
//[/Headers]

#include "PanelModulation.h"
#include "PreenTheme.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...




struct NameAndId sourceNameInit[] = { { "None",	0, 0},
	{ "lfo 1",	1, 0 },
	{ "lfo 2",	2, 0 },
	{ "lfo 3",	3, 0 },
	{ "Free Env 1",	4, 0 },
	{ "Free Env 2",	5, 0 },
	{ "Step Seq 1",	6, 0 },
	{ "Step Seq 2",	7, 0 },
	{ "Mod Wheel",	8, 0 },
	{ "Pitch Bend",	9, 0 },
	{ "After touch",	10, 0 },
	{ "Velocity",	11, 0 },
	{ "Note1",	12, 0 },
	{ "Note2",	17, 0 },
	{ "Breath CC2",	18, 0 },
	{ "Performance 1",	13, 0 },
	{ "Performance 2",	14, 0 },
	{ "Performance 3",	15, 0 },
	{ "Performance 4",	16, 0 },
	{ "MPE CC74",	19, 0 },
	{ "Random",	20, 0 },
	{ "Polyphonic Aftertouch", 21, PROPERTY_PREENFM3 },
	{ "User CC1", 22, PROPERTY_PREENFM3 },
	{ "User CC2", 23, PROPERTY_PREENFM3 },
	{ "User CC3", 24, PROPERTY_PREENFM3 },
	{ "User CC4", 25, PROPERTY_PREENFM3 },
	{ "-- pfm3 only", 21, PROPERTY_PREENFM2 },
	{ "-- pfm3 only", 22, PROPERTY_PREENFM2 },
	{ "-- pfm3 only", 23, PROPERTY_PREENFM2 },
	{ "-- pfm3 only", 24, PROPERTY_PREENFM2 },
	{ "-- pfm3 only", 25, PROPERTY_PREENFM2 },
	{ "", 0}
};


struct NameAndId destNameInit[] = {
	{ "None", 0, 0},
	{ "Gate",1, 0 },
	{ "Index Modulation ",2, 0 },
	{ "Index Modulation 2",3, 0 },
	{ "Index Modulation 3",4, 0 },
	{ "Index Modulation 4",5, 0 },
	{ "Feedback Modulation",48, PROPERTY_PREENFM3 },
	{ "All Mod. Indexes",6, 0 },
	{ "Mix 1",7 , 0 },
	{ "Pan 1",8 , 0 },
	{ "Mix 2",9, 0 },
	{ "Pan 2",10, 0 },
	{ "Mix 3",11, 0 },
	{ "Pan 3",12, 0 },
	{ "Mix 4",13, 0 },
	{ "Pan 4",14, 0 },
	{ "All Mixes",15, 0 },
	{ "All Pans",16, 0 },
	{ "Op1 Frequency",17, 0 },
	{ "Op2 Frequency",18, 0 },
	{ "Op3 Frequency",19, 0 },
	{ "Op4 Frequency",20, 0 },
	{ "Op5 Frequency",21, 0 },
	{ "Op6 Frequency",22, 0 },
	{ "All Op Frequencies",23, 0 },
	{ "All Op Freq Harmonic",43, 0 },
	{ "Op1 Attack",24, 0 },
	{ "Op2 Attack",25, 0 },
	{ "Op3 Attack",26, 0 },
	{ "Op4 Attack",27, 0 },
	{ "Op5 Attack",28, 0 },
	{ "Op6 Attack",29, 0 },
	{ "Carrier Attacks",30, 0 },
	{ "Carrier Decays",44, 0 },
	{ "Carrier Releases",31, 0 },
	{ "Modulator Attacks",45, 0 },
	{ "Modulator Decays",46, 0 },
	{ "Modulator Releases",47, 0 },
	{ "Mtx Multiplier 1",32, 0 },
	{ "Mtx Multiplier 2",33, 0 },
	{ "Mtx Multiplier 3",34, 0 },
	{ "Mtx Multiplier 4",35, 0 },
	{ "lfo 1 Frequency",36, 0 },
	{ "lfo 2 Frequency",37, 0 },
	{ "lfo 3 Frequency",38, 0 },
	{ "Free Env2 Silence",39, 0 },
	{ "Step Seq 1 gate",40, 0 },
	{ "Step Seq 1 start",48, PROPERTY_PREENFM2 },
	{ "Step Seq 2 gate",41, 0 },
	{ "Step Seq 2 start",49, PROPERTY_PREENFM2 },
	{ "Filter frequency", 42, 0 },
	{ "-- pfm2 only", 49, PROPERTY_PREENFM3 },
	{ "", 0 }
};



//[/MiscUserDefs]

//==============================================================================
PanelModulation::PanelModulation ()
{
    //[Constructor_pre] You can add your own custom stuff here..
	sourceList = std::make_unique<ListProperty>("sources", ".sources.xml");
	sourceList->init(sourceNameInit);
	NameAndId* sourcesNameAndId = sourceList->getList();

	destList = std::make_unique<ListProperty>("dest", ".dest.xml");
	destList->init(destNameInit);
	NameAndId* destNameAndId = destList->getList();

    //[/Constructor_pre]

    matrixGroup.reset (new juce::GroupComponent ("matrix group",
                                                 TRANS("Matrix")));
    addAndMakeVisible (matrixGroup.get());

    lfoGroup.reset (new juce::GroupComponent ("lfo group",
                                              juce::String()));
    addAndMakeVisible (lfoGroup.get());

    env1Group.reset (new juce::GroupComponent ("env 1 group",
                                               TRANS("Free Enveloppe 1")));
    addAndMakeVisible (env1Group.get());

    env2Group.reset (new juce::GroupComponent ("env 2 group",
                                               TRANS("Free Enveloppe 2")));
    addAndMakeVisible (env2Group.get());

    stepSeqGroup.reset (new juce::GroupComponent ("step sequencer group",
                                                  juce::String()));
    addAndMakeVisible (stepSeqGroup.get());


    //[UserPreSize]

	// LFO
	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		addAndMakeVisible((lfoButton[k] = std::make_unique<TextButton>("lfo button")).get());
		lfoButton[k]->setButtonText("LFO " + String(k + 1));
		lfoButton[k]->addListener(this);
		lfoButton[k]->setClickingTogglesState(true);
		lfoButton[k]->setRadioGroupId(4243);
		lfoButton[k]->setConnectedEdges((k != 0 ? Button::ConnectedOnLeft : 0) | (k != NUMBER_OF_LFO - 1 ? Button::ConnectedOnRight : 0));

		addAndMakeVisible((lfoShape[k] = std::make_unique<ComboBox>("LFO" + String(k + 1) + " Shape")).get());
		lfoShape[k]->setEditableText(false);
		lfoShape[k]->setJustificationType(Justification::left);
		lfoShape[k]->addItem("Sin", 1);
		lfoShape[k]->addItem("Saw", 2);
		lfoShape[k]->addItem("Triangle", 3);
		lfoShape[k]->addItem("Square", 4);
		lfoShape[k]->addItem("Random", 5);
		lfoShape[k]->addItem("Brownian", 6);
		lfoShape[k]->addItem("Wandering", 7);
		lfoShape[k]->addItem("Flow", 8);
		lfoShape[k]->setScrollWheelEnabled(true);
		lfoShape[k]->setSelectedId(1);
		lfoShape[k]->addListener(this);

		addAndMakeVisible((lfoPhase[k] = std::make_unique<SliderPfm2>("LFO" + String(k + 1) + " Phase")).get());
		lfoPhase[k]->setRange(0, 1.0f, .01f);
		lfoPhase[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		lfoPhase[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		lfoPhase[k]->setDoubleClickReturnValue(true, 0.0f);
		lfoPhase[k]->setValue(0, dontSendNotification);
		lfoPhase[k]->addListener(this);


		addAndMakeVisible((lfoExtMidiSync[k] = std::make_unique<ComboBox>("LFO" + String(k + 1) + " External Sync")).get());
		lfoExtMidiSync[k]->setEditableText(false);
		lfoExtMidiSync[k]->setJustificationType(Justification::left);
		lfoExtMidiSync[k]->addItem("Internal", 9990);
		lfoExtMidiSync[k]->addItem("MC/16", 10000);
		lfoExtMidiSync[k]->addItem("MC/8", 10010);
		lfoExtMidiSync[k]->addItem("MC/4", 10020);
		lfoExtMidiSync[k]->addItem("MC/2", 10030);
		lfoExtMidiSync[k]->addItem("MC", 10040);
		lfoExtMidiSync[k]->addItem("MC*2", 10050);
		lfoExtMidiSync[k]->addItem("MC*3", 10060);
		lfoExtMidiSync[k]->addItem("MC*4", 10070);
		lfoExtMidiSync[k]->addItem("MC*8", 10080);
		lfoExtMidiSync[k]->setScrollWheelEnabled(true);
		lfoExtMidiSync[k]->setSelectedId(9990);
		lfoExtMidiSync[k]->addListener(this);

		addAndMakeVisible((lfoFrequency[k] = std::make_unique<SliderPfm2>("LFO" + String(k + 1) + " Frequency")).get());
		lfoFrequency[k]->setRange(0, 99.9f, .01f);
		lfoFrequency[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		lfoFrequency[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		lfoFrequency[k]->setDoubleClickReturnValue(true, 3.0f);
		lfoFrequency[k]->setValue(3.0f, dontSendNotification);
		lfoFrequency[k]->addListener(this);

		addAndMakeVisible((lfoBias[k] = std::make_unique<SliderPfm2>("LFO" + String(k + 1) + " Bias")).get());
		lfoBias[k]->setRange(-1.0f, 1.0f, .01f);
		lfoBias[k]->setSliderStyle(Slider::LinearVertical);
		lfoBias[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		lfoBias[k]->setDoubleClickReturnValue(true, 0.0f);
		lfoBias[k]->setValue(0.0f, dontSendNotification);
		lfoBias[k]->addListener(this);

		addAndMakeVisible((lfoKSync[k] = std::make_unique<SliderPfm2>("LFO" + String(k + 1) + " KeySync time")).get());
		lfoKSync[k]->setRange(0.0f, 16.0f, .01f);
		lfoKSync[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		lfoKSync[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		lfoKSync[k]->setDoubleClickReturnValue(true, 0.0f);
		lfoKSync[k]->setValue(0.0f, dontSendNotification);
		lfoKSync[k]->addListener(this);

		addAndMakeVisible((lfoKsynOnOff[k] = std::make_unique<ComboBox>("LFO" + String(k + 1) + " KeySync")).get());
		lfoKsynOnOff[k]->setEditableText(false);
		lfoKsynOnOff[k]->setJustificationType(Justification::left);
		lfoKsynOnOff[k]->addItem("Off", 1);
		lfoKsynOnOff[k]->addItem("On", 2);
		lfoKsynOnOff[k]->setScrollWheelEnabled(true);
		lfoKsynOnOff[k]->setSelectedId(2);
		lfoKsynOnOff[k]->addListener(this);

	}

	lfoButton[0]->setToggleState(true, sendNotification);

	addAndMakeVisible((lfoPhaseLabel = std::make_unique<Label>("LFO phase label", "Phase")).get());
	lfoPhaseLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((lfoFrequencyLabel = std::make_unique<Label>("LFO freq label", "Frequency")).get());
	lfoFrequencyLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((lfoBiasLabel = std::make_unique<Label>("LFO bias label", "Bias")).get());
	lfoBiasLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((lfoKSynLabel = std::make_unique<Label>("LFO ksyn label", "Note Sync")).get());
	lfoKSynLabel->setJustificationType(Justification::centredTop);


	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		addAndMakeVisible((stepSeqExtMidiSync[k] = std::make_unique<ComboBox>("Step Seq " + String(k + 1) + " External Sync")).get());
		stepSeqExtMidiSync[k]->setEditableText(false);
		stepSeqExtMidiSync[k]->setJustificationType(Justification::left);
		stepSeqExtMidiSync[k]->addItem("Internal", 240);
		stepSeqExtMidiSync[k]->addItem("MC/4", 241);
		stepSeqExtMidiSync[k]->addItem("MC/2", 242);
		stepSeqExtMidiSync[k]->addItem("MC", 243);
		stepSeqExtMidiSync[k]->addItem("MC*2", 244);
		stepSeqExtMidiSync[k]->addItem("MC*4", 245);
		stepSeqExtMidiSync[k]->setScrollWheelEnabled(true);
		stepSeqExtMidiSync[k]->setSelectedId(1);
		stepSeqExtMidiSync[k]->addListener(this);

		addAndMakeVisible((stepSeqBPM[k] = std::make_unique<SliderPfm2>("Step Seq " + String(k + 1) + " BPM")).get());
		stepSeqBPM[k]->setRange(10, 240.0f, 1.0f);
		stepSeqBPM[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		stepSeqBPM[k]->setTextBoxStyle(Slider::TextBoxLeft, false, 35, 16);
		stepSeqBPM[k]->setDoubleClickReturnValue(true, 3.0f);
		stepSeqBPM[k]->setValue(3.0f, dontSendNotification);
		stepSeqBPM[k]->addListener(this);

		addAndMakeVisible((stepSeqGate[k] = std::make_unique<SliderPfm2>("Step Seq " + String(k + 1) + " Gate")).get());
		stepSeqGate[k]->setRange(0.0f, 1.0f, 0.01f);
		stepSeqGate[k]->setSliderStyle(Slider::LinearHorizontal);
		stepSeqGate[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 35, 16);
		stepSeqGate[k]->setDoubleClickReturnValue(true, 0.5f);
		stepSeqGate[k]->setValue(0.5f, dontSendNotification);
		stepSeqGate[k]->addListener(this);

		addAndMakeVisible((stepSequencer[k] = std::make_unique<StepSequencer>(16, 15)).get());
		stepSequencer[k]->setBounds(16, 368, 500, 120);
		stepSequencer[k]->setName("Step Seq " + String(k + 1));
		stepSequencer[k]->addListener(this);

		stepSeqButton[k] = std::make_unique<TextButton>("step sequencer button");
		stepSeqButton[k]->setBounds(16 + 80 * k, 336, 80, 20);
		stepSeqButton[k]->setButtonText(TRANS("Sequencer " + String(k + 1)));
		stepSeqButton[k]->addListener(this);
		stepSeqButton[k]->setClickingTogglesState(true);
		stepSeqButton[k]->setRadioGroupId(4242);
		stepSeqButton[k]->setConnectedEdges((k != 0 ? Button::ConnectedOnLeft : 0) | (k != NUMBER_OF_STEP_SEQ - 1 ? Button::ConnectedOnRight : 0));
		addAndMakeVisible(stepSeqButton[k].get());
	}
	stepSeqButton[0]->setToggleState(true, sendNotification);

	addAndMakeVisible((stepSeqBPMLabel = std::make_unique<Label>("step seq label", "BPM")).get());
	stepSeqBPMLabel->setJustificationType(Justification::centred);

	addAndMakeVisible((stepSeqGateLabel = std::make_unique<Label>("step seq get label", "Gate")).get());
	stepSeqBPMLabel->setJustificationType(Justification::centred);


	for (int r = 0; r < NUMBER_OF_MATRIX_ROW; r++) {
		addAndMakeVisible((matrixRowLabel[r] = std::make_unique<Label>(String("matrix label ") + String(r + 1), String(r + 1))).get());
		matrixRowLabel[r]->setJustificationType(Justification::centred);

		addAndMakeVisible((matrixMultipler[r] = std::make_unique<SliderPfm2>("Mtx" + String(r + 1) + " Multiplier")).get());
		matrixMultipler[r]->setRange(-10.0f, 10.0f, .01f);
		matrixMultipler[r]->setSliderStyle(Slider::RotaryVerticalDrag);
		matrixMultipler[r]->setTextBoxStyle(Slider::TextBoxLeft, false, 36, 16);
		matrixMultipler[r]->setDoubleClickReturnValue(true, 0.0f);
		matrixMultipler[r]->setValue(0.0f, dontSendNotification);
		matrixMultipler[r]->addListener(this);

		addAndMakeVisible((matrixSource[r] = std::make_unique<ComboBox>("Mtx" + String(r + 1) + " Source")).get());
		matrixSource[r]->setEditableText(false);
		matrixSource[r]->setJustificationType(Justification::centred);
		for (int i = 0; sourcesNameAndId[i].name != ""; i++) {
			if (sourcesNameAndId[i].preenfmTarget == 0 || sourcesNameAndId[i].preenfmTarget == PROPERTY_PREENFM2) {
				matrixSource[r]->addItem(sourcesNameAndId[i].name, (sourcesNameAndId[i].id + 1));
			}
		}
		matrixSource[r]->setSelectedId(1);
		matrixSource[r]->setScrollWheelEnabled(true);
		matrixSource[r]->addListener(this);

		addAndMakeVisible((matrixDestination1[r] = std::make_unique<ComboBox>("Mtx" + String(r + 1) + " Destination1")).get());
		matrixDestination1[r]->setEditableText(false);
		matrixDestination1[r]->setJustificationType(Justification::centred);
		for (int i = 0; destNameAndId[i].name != ""; i++) {
			if (destNameAndId[i].preenfmTarget == 0 || destNameAndId[i].preenfmTarget == PROPERTY_PREENFM2) {
				matrixDestination1[r]->addItem(destNameAndId[i].name, (destNameAndId[i].id + 1));
			}
		}
		matrixDestination1[r]->setSelectedId(1);
		matrixDestination1[r]->setScrollWheelEnabled(true);
		matrixDestination1[r]->addListener(this);

        addAndMakeVisible((matrixDestination2[r] = std::make_unique<ComboBox>("Mtx" + String(r + 1) + " Destination2")).get());
        matrixDestination2[r]->setEditableText(false);
        matrixDestination2[r]->setJustificationType(Justification::centred);
        for (int i = 0; destNameAndId[i].name != ""; i++) {
			if (destNameAndId[i].preenfmTarget == 0 || destNameAndId[i].preenfmTarget == PROPERTY_PREENFM2) {
				matrixDestination2[r]->addItem(destNameAndId[i].name, (destNameAndId[i].id + 1));
			}
        }
        matrixDestination2[r]->setSelectedId(1);
        matrixDestination2[r]->setScrollWheelEnabled(true);
        matrixDestination2[r]->addListener(this);
    }

	addAndMakeVisible((enveloppeFree1 = std::make_unique<EnveloppeFree1>(127)).get());
	enveloppeFree1->setName(TRANS("Free Env 1"));

	addAndMakeVisible((enveloppeFree2 = std::make_unique<EnveloppeFree2>(127)).get());
	enveloppeFree2->setName(TRANS("Free Env 2"));

	addAndMakeVisible((enveloppeFree2LoopLabel = std::make_unique<Label>("Freen Env 2 Loop label", "Loop")).get());

	addAndMakeVisible((enveloppeFree2Loop = std::make_unique<ComboBox>("Free Env 2 Loop")).get());
	enveloppeFree2Loop->setEditableText(false);
	enveloppeFree2Loop->setJustificationType(Justification::centred);
	enveloppeFree2Loop->addItem("None", 1);
	enveloppeFree2Loop->addItem("Silence", 2);
	enveloppeFree2Loop->addItem("Attack", 3);
	enveloppeFree2Loop->setSelectedId(1);
	enveloppeFree2Loop->addListener(this);
    //[/UserPreSize]

    setSize (900, 700);


    //[Constructor] You can add your own custom stuff here..
	eventsToAdd = nullptr;
	initialized = false;
    //[/Constructor]
}

PanelModulation::~PanelModulation()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    matrixGroup = nullptr;
    lfoGroup = nullptr;
    env1Group = nullptr;
    env2Group = nullptr;
    stepSeqGroup = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PanelModulation::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (PreenTheme::colour(*this, PreenTheme::backgroundId));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PanelModulation::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    auto content = getLocalBounds().reduced (10, 9);
    constexpr int moduleGap = 8;
    auto leftColumn = content.removeFromLeft (roundToInt (content.getWidth() * 0.585f));
    content.removeFromLeft (moduleGap);
    auto matrixBounds = content;

    const int lfoHeight = roundToInt (leftColumn.getHeight() * 0.225f);
    const int envelopeHeight = roundToInt (leftColumn.getHeight() * 0.185f);
    auto lfoBounds = leftColumn.removeFromTop (lfoHeight);
    leftColumn.removeFromTop (moduleGap);
    auto env1Bounds = leftColumn.removeFromTop (envelopeHeight);
    leftColumn.removeFromTop (moduleGap);
    auto env2Bounds = leftColumn.removeFromTop (envelopeHeight);
    leftColumn.removeFromTop (moduleGap);
    auto stepBounds = leftColumn;

    matrixGroup->setBounds (matrixBounds);
    lfoGroup->setBounds (lfoBounds);
    env1Group->setBounds (env1Bounds);
    env2Group->setBounds (env2Bounds);
    stepSeqGroup->setBounds (stepBounds);
    //[UserResized] Add your own custom resize handling here..

	const auto lfoColumnX = [&lfoBounds] (float proportion)
	{
		return lfoBounds.getX() + roundToInt (lfoBounds.getWidth() * proportion);
	};
	const int lfoLabelY = lfoBounds.getY() + 31;
	const int lfoControlY = lfoBounds.getY() + 63;
	const int lfoColumnWidth = jmax (48, roundToInt (lfoBounds.getWidth() * 0.105f));
	const int lfoKnobHeight = jmax (42, lfoBounds.getBottom() - lfoControlY - 8);
	lfoPhaseLabel->setBounds(lfoColumnX (0.82f), lfoLabelY, lfoColumnWidth, 20);
	lfoFrequencyLabel->setBounds(lfoColumnX (0.34f), lfoLabelY, lfoColumnWidth, 20);
	lfoBiasLabel->setBounds(lfoColumnX (0.50f), lfoLabelY, lfoColumnWidth, 20);
	lfoKSynLabel->setBounds(lfoColumnX (0.66f), lfoLabelY, lfoColumnWidth, 20);
	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		lfoButton[k]->setBounds(lfoBounds.getX() + 2 + 60 * k, lfoBounds.getY() + 9, 60, 26);
		lfoShape[k]->setBounds(lfoColumnX (0.11f), lfoControlY + 8, lfoColumnWidth + 10, 20);
		lfoPhase[k]->setBounds(lfoColumnX (0.82f), lfoControlY, lfoColumnWidth, lfoKnobHeight);
		lfoExtMidiSync[k]->setBounds(lfoColumnX (0.34f), lfoLabelY + 20, lfoColumnWidth, 18);
		lfoFrequency[k]->setBounds(lfoColumnX (0.34f), lfoControlY, lfoColumnWidth, lfoKnobHeight);
		lfoBias[k]->setBounds(lfoColumnX (0.50f), lfoLabelY + 20, lfoColumnWidth, lfoBounds.getBottom() - lfoLabelY - 28);
		lfoKsynOnOff[k]->setBounds(lfoColumnX (0.66f), lfoLabelY + 20, lfoColumnWidth, 18);
		lfoKSync[k]->setBounds(lfoColumnX (0.66f), lfoControlY, lfoColumnWidth, lfoKnobHeight);
	}

	stepSeqBPMLabel->setBounds(stepBounds.getX() + roundToInt (stepBounds.getWidth() * 0.43f), stepBounds.getY() + 42, 40, 20);
	stepSeqGateLabel->setBounds(stepBounds.getX() + roundToInt (stepBounds.getWidth() * 0.76f), stepBounds.getY() + 42, 60, 20);
	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		stepSeqButton[k]->setBounds(stepBounds.getX() + 2 + 100 * k, stepBounds.getY() + 9, 100, 26);
		stepSequencer[k]->setBounds(stepBounds.getX() + 14, stepBounds.getY() + 108,
			stepBounds.getWidth() - 28, jmax (40, stepBounds.getHeight() - 118));
		stepSeqExtMidiSync[k]->setBounds(stepBounds.getX() + roundToInt (stepBounds.getWidth() * 0.25f), stepBounds.getY() + 43, 80, 20);
		stepSeqBPM[k]->setBounds(stepBounds.getX() + roundToInt (stepBounds.getWidth() * 0.40f), stepBounds.getY() + 62, 80, 43);
		stepSeqGate[k]->setBounds(stepBounds.getX() + roundToInt (stepBounds.getWidth() * 0.72f), stepBounds.getY() + 66, 90, 36);
	}

	const int matrixTop = matrixBounds.getY() + 25;
	const int matrixBottom = matrixBounds.getBottom() - 13;
	const float matrixRowHeight = static_cast<float> (matrixBottom - matrixTop) / NUMBER_OF_MATRIX_ROW;
	const int sourceX = matrixBounds.getX() + 34;
	const int sourceWidth = roundToInt (matrixBounds.getWidth() * 0.33f);
	const int multiplierX = sourceX + sourceWidth + 5;
	const int multiplierWidth = roundToInt (matrixBounds.getWidth() * 0.25f);
	const int destinationX = multiplierX + multiplierWidth + 5;
	const int destinationWidth = jmax (70, matrixBounds.getRight() - 10 - destinationX);
	for (int r = 0; r < NUMBER_OF_MATRIX_ROW; r++) {
		const int rowCentre = roundToInt (matrixTop + matrixRowHeight * (r + 0.5f));
		const int knobHeight = jmin (46, jmax (36, roundToInt (matrixRowHeight - 3.0f)));
		matrixRowLabel[r]->setBounds(matrixBounds.getX() + 6, rowCentre - 10, 25, 20);
		matrixSource[r]->setBounds(sourceX, rowCentre - 10, sourceWidth, 20);
		matrixMultipler[r]->setBounds(multiplierX, rowCentre - knobHeight / 2, multiplierWidth, knobHeight);
		matrixDestination1[r]->setBounds(destinationX, rowCentre - 20, destinationWidth, 18);
        matrixDestination2[r]->setBounds(destinationX, rowCentre + 2, destinationWidth, 18);
	}
	enveloppeFree1->setBounds(env1Bounds.reduced (14, 20));
	enveloppeFree2->setBounds(env2Bounds.reduced (14, 20));

	enveloppeFree2LoopLabel->setBounds(env2Bounds.getRight() - 150, env2Bounds.getY() + 4, 55, 20);
	enveloppeFree2Loop->setBounds(env2Bounds.getRight() - 92, env2Bounds.getY() + 4, 80, 20);

    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void PanelModulation::buttonClicked(Button* buttonThatWasClicked) {
	bool lfoButtonClicked = false;
	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		if (buttonThatWasClicked == lfoButton[k].get()) {
			lfoButtonClicked = true;
		}
	}
	if (lfoButtonClicked) {
		for (int k = 0; k < NUMBER_OF_LFO; k++) {
			if (buttonThatWasClicked == lfoButton[k].get()) {
				lfoShape[k]->setVisible(true);
				lfoPhase[k]->setVisible(true);
				lfoExtMidiSync[k]->setVisible(true);
				lfoFrequency[k]->setVisible(true);
				lfoBias[k]->setVisible(true);
				lfoKSync[k]->setVisible(true);
				lfoKsynOnOff[k]->setVisible(true);

			}
			else {
				lfoShape[k]->setVisible(false);
				lfoPhase[k]->setVisible(false);
				lfoExtMidiSync[k]->setVisible(false);
				lfoFrequency[k]->setVisible(false);
				lfoBias[k]->setVisible(false);
				lfoKSync[k]->setVisible(false);
				lfoKsynOnOff[k]->setVisible(false);
			}
		}
		return;
	}
	bool stepButtonClicked = false;
	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		if (buttonThatWasClicked == stepSeqButton[k].get()) {
			stepButtonClicked = true;
		}
	}
	if (stepButtonClicked) {
		for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
			if (buttonThatWasClicked == stepSeqButton[k].get()) {
				stepSequencer[k]->setVisible(true);
				stepSeqBPM[k]->setVisible(true);
				stepSeqExtMidiSync[k]->setVisible(true);
				stepSeqGate[k]->setVisible(true);
			}
			else {
				stepSequencer[k]->setVisible(false);
				stepSeqBPM[k]->setVisible(false);
				stepSeqExtMidiSync[k]->setVisible(false);
				stepSeqGate[k]->setVisible(false);
			}
		}
		return;
	}


}
void PanelModulation::sliderValueChanged(Slider* sliderThatWasMoved) {
	sliderValueChanged(sliderThatWasMoved, true);
}

void PanelModulation::sliderValueChanged(Slider* sliderThatWasMoved, bool fromPluginUI)
{
	// Update the value if the change comes from the UI
	if (fromPluginUI) {
		AudioProcessorParameter * parameterReady = parameterMap[sliderThatWasMoved->getName()];
		if (parameterReady != nullptr) {
			float value = (float)sliderThatWasMoved->getValue();
			static_cast<MidifiedFloatParameter*>(parameterReady)->setRealValue(value);
		}
	}
	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		if (sliderThatWasMoved == lfoFrequency[k].get() && lfoExtMidiSync[k]->getSelectedId() != 9990) {
			lfoFrequency[k]->setEnabled(true);
			lfoExtMidiSync[k]->setSelectedId(9990, dontSendNotification);
		}

		if (sliderThatWasMoved == lfoKSync[k].get() && lfoKsynOnOff[k]->getSelectedId() != 2) {
			lfoKSync[k]->setEnabled(true);
			lfoKsynOnOff[k]->setSelectedId(2, dontSendNotification);
		}
	}

	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		if (sliderThatWasMoved == stepSeqBPM[k].get() && stepSeqExtMidiSync[k]->getSelectedId() != 240) {
			stepSeqBPM[k]->setEnabled(true);
			stepSeqExtMidiSync[k]->setSelectedId(240, dontSendNotification);
		}
	}
}

void PanelModulation::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
	comboBoxChanged(comboBoxThatHasChanged, true);
}

void PanelModulation::comboBoxChanged(ComboBox* comboBoxThatHasChanged, bool fromPluginUI) {
	// Update the value if the change comes from the UI
	if (fromPluginUI) {
		AudioProcessorParameter * parameterReady = parameterMap[comboBoxThatHasChanged->getName()];
		if (parameterReady != nullptr) {
			float value = (float)comboBoxThatHasChanged->getSelectedId();
			static_cast<MidifiedFloatParameter*>(parameterReady)->setRealValue(value);
		}
	}

	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		if (comboBoxThatHasChanged == lfoExtMidiSync[k].get()) {
			// Refresh Ksyn frequency on pfm2
			if (comboBoxThatHasChanged->getSelectedId() == 9990) {
				lfoFrequency[k]->setEnabled(true);
				// Force sending new value
				float value = (float)lfoFrequency[k]->getValue();
				lfoFrequency[k]->setValue(99.9f);
				lfoFrequency[k]->setValue(value);
			}
			else {
				lfoFrequency[k]->setEnabled(false);
			}
		}
		if (comboBoxThatHasChanged == lfoKsynOnOff[k].get()) {
			if (lfoKsynOnOff[k]->getSelectedId() == 2) {
				lfoKSync[k]->setEnabled(true);
				// Refresh Ksyn frequency on pfm2
				float value = (float)lfoKSync[k]->getValue();
				lfoKSync[k]->setValue(.0f);
				lfoKSync[k]->setValue(value);
			}
			else {
				lfoKSync[k]->setEnabled(false);
			}
		}
	}
	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		if (comboBoxThatHasChanged == stepSeqExtMidiSync[k].get()) {
			if (comboBoxThatHasChanged->getSelectedId() == 240) {
				stepSeqBPM[k]->setEnabled(true);
				float value = (float)stepSeqBPM[k]->getValue();
				stepSeqBPM[k]->setValue(10.0f);
				stepSeqBPM[k]->setValue(value);
			}
			else {
				stepSeqBPM[k]->setEnabled(false);
			}
		}
	}

}



void PanelModulation::buildParameters() {
	for (int k = 0; k < NUMBER_OF_MATRIX_ROW; k++) {
		updateComboFromParameter(matrixSource[k].get());
		updateSliderFromParameter(matrixMultipler[k].get());
        updateComboFromParameter(matrixDestination1[k].get());
		updateComboFromParameter(matrixDestination2[k].get());
	}
	for (int k = 0; k < NUMBER_OF_LFO; k++) {
		updateComboFromParameter(lfoShape[k].get());
		updateComboFromParameter(lfoExtMidiSync[k].get());
		updateSliderFromParameter(lfoPhase[k].get());
		updateSliderFromParameter(lfoFrequency[k].get());
		updateSliderFromParameter(lfoBias[k].get());
		updateComboFromParameter(lfoKsynOnOff[k].get());
		updateSliderFromParameter(lfoKSync[k].get());
	}

	updateComboFromParameter(enveloppeFree2Loop.get());

	for (int k = 0; k < NUMBER_OF_STEP_SEQ; k++) {
		updateStepSeqParameter(stepSequencer[k].get());
		updateComboFromParameter(stepSeqExtMidiSync[k].get());
		updateSliderFromParameter(stepSeqBPM[k].get());
		updateSliderFromParameter(stepSeqGate[k].get());
	}
	updateUIEnveloppe("");

	// Let listen to enveloppe
	if (!initialized) {
		enveloppeFree1->addListener((EnveloppeListener*)this);
		enveloppeFree2->addListener((EnveloppeListener*)this);
	}

	initialized = true;
}


void PanelModulation::updateUIEnveloppe(String paramName) {
	const char** pointName = enveloppeFree1->getPointSuffix();
	String pString = enveloppeFree1->getName();

	for (int p = 1; p < enveloppeFree1->getNumberOfPoints(); p++) {
		String name = pString + String(pointName[p - 1]);

		MidifiedFloatParameter* param = checkParamExistence(name);

		if (param == nullptr || (paramName.length() > 0 && name != String(paramName))) {
			continue;
		}

		// And let's update the value and update the UI Without sending modification !!!
		// No modification : we dont want sliderValueChanged to be called in the different panels

		if (p == 3) {
			if (param->getRealValue() != enveloppeFree1->getY(p)) {
				enveloppeFree1->setY(p, param->getRealValue());
				enveloppeFree1->repaint();
			}
		}
		else {
			if (param->getRealValue() != enveloppeFree1->getX(p)) {
				enveloppeFree1->setX(p, param->getRealValue());
				enveloppeFree1->repaint();
			}
		}
	}

	pointName = enveloppeFree2->getPointSuffix();
	pString = enveloppeFree2->getName();

	for (int p = 1; p < enveloppeFree2->getNumberOfPoints(); p++) {
		String name = String(pString) + String(pointName[p - 1]);

		MidifiedFloatParameter* param = checkParamExistence(name);

		if (param == nullptr || (paramName.length() > 0 && name != String(paramName))) {
			continue;
		}

		// And let's update the value and update the UI Without sending modification !!!
		// No modification : we dont want sliderValueChanged to be called in the different panels
		if (param->getRealValue() != enveloppeFree2->getX(p)) {
			enveloppeFree2->setX(p, param->getRealValue());
			enveloppeFree2->repaint();
		}
	}
}

void PanelModulation::updateUIStepSequencer(String paramName) {
	if (paramName.startsWith("Step Seq 1")) {
		updateStepSeqParameter(stepSequencer[0].get());
	}
	else {
		updateStepSeqParameter(stepSequencer[1].get());
	}
}


bool PanelModulation::containsThisParameterAsStepSequencer(String name) {
	return (name.startsWith("Step Seq") && name.indexOf(" Step ") == 10);
}

bool PanelModulation::containsThisParameterAsEnveloppe(String name) {
	return name.startsWith("Free Env ");
}


void PanelModulation::updateSliderFromParameter_hook(Slider* slider) {
	sliderValueChanged(slider, false);
}

void PanelModulation::updateComboFromParameter_hook(ComboBox* combo) {
	comboBoxChanged(combo, false);
}

void PanelModulation::sliderDragStarted(Slider* slider) {
	AudioProcessorParameter * param = parameterMap[slider->getName()];
	if (param != nullptr) {
		param->beginChangeGesture();
	}
}
void PanelModulation::sliderDragEnded(Slider* slider) {
	AudioProcessorParameter * param = parameterMap[slider->getName()];
	if (param != nullptr) {
		param->endChangeGesture();
	}
}


void PanelModulation::setPfmType(int typeComboId) {

	pfmType = typeComboId;
	// Combo = 1 => preenfm2
	// Combo = 2 => preenfm3
	int preenfmPropertyVersion = (pfmType == TYPE_PREENFM3) ? PROPERTY_PREENFM3 : PROPERTY_PREENFM2;

	NameAndId* sourcesNameAndId = sourceList->getList();
	NameAndId* destNameAndId = destList->getList();

	for (int r = 0; r < NUMBER_OF_MATRIX_ROW; r++) {
		int selectedSource = matrixSource[r]->getSelectedId();
		int selectedDest1 = matrixDestination1[r]->getSelectedId();
		int selectedDest2 = matrixDestination2[r]->getSelectedId();

		matrixSource[r]->clear(NotificationType::dontSendNotification);
		for (int i = 0; sourcesNameAndId[i].name != ""; i++) {
			if (sourcesNameAndId[i].preenfmTarget == 0 || sourcesNameAndId[i].preenfmTarget == preenfmPropertyVersion) {
				matrixSource[r]->addItem(sourcesNameAndId[i].name, (sourcesNameAndId[i].id + 1));
			}
			if (sourcesNameAndId[i].preenfmTarget == preenfmPropertyVersion && sourcesNameAndId[i].name.startsWith("--")) {
				matrixSource[r]->setItemEnabled((sourcesNameAndId[i].id + 1), false);
			}
		}
		matrixSource[r]->setSelectedId(selectedSource, NotificationType::dontSendNotification);

		matrixDestination1[r]->clear(NotificationType::dontSendNotification);
		matrixDestination2[r]->clear(NotificationType::dontSendNotification);

		for (int i = 0; destNameAndId[i].name != ""; i++) {
			if (destNameAndId[i].preenfmTarget == 0 || destNameAndId[i].preenfmTarget == preenfmPropertyVersion) {
				matrixDestination1[r]->addItem(destNameAndId[i].name, (destNameAndId[i].id + 1));
				matrixDestination2[r]->addItem(destNameAndId[i].name, (destNameAndId[i].id + 1));
			}
			if (destNameAndId[i].preenfmTarget == preenfmPropertyVersion && destNameAndId[i].name.startsWith("--")) {
				matrixDestination1[r]->setItemEnabled((destNameAndId[i].id + 1), false);
				matrixDestination2[r]->setItemEnabled((destNameAndId[i].id + 1), false);
			}
		}
		matrixDestination1[r]->setSelectedId(selectedDest1, NotificationType::dontSendNotification);
		matrixDestination2[r]->setSelectedId(selectedDest2, NotificationType::dontSendNotification);
	}
}


//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelModulation" componentName=""
                 parentClasses="public Component, public Button::Listener, public Slider::Listener, public ComboBox::Listener, public PanelOfComponents"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="900"
                 initialHeight="700">
  <BACKGROUND backgroundColour="62934">
    <RECT pos="55% 2 44.071% 182M" fill=" radial: 90% 25%, 60% 25%, 0=ff155163, 1=ff083543"
          hasStroke="0"/>
  </BACKGROUND>
  <GROUPCOMPONENT name="matrix group" id="f5fd2d041b369fc" memberName="matrixGroup"
                  virtualName="" explicitFocusOrder="0" pos="59.742% 0.918% 39.485% 97.913%"
                  outlinecol="ff749fad" textcol="ff749fad" title="Matrix"/>
  <GROUPCOMPONENT name="lfo group" id="25551a3d7e81232d" memberName="lfoGroup"
                  virtualName="" explicitFocusOrder="0" pos="0% 0.918% 59.056% 22.955%"
                  outlinecol="ff749fad" textcol="ff749fad" title=""/>
  <GROUPCOMPONENT name="env 1 group" id="dc02178fe3e4a3e1" memberName="env1Group"
                  virtualName="" explicitFocusOrder="0" pos="0% 24.374% 59.056% 18.531%"
                  outlinecol="ff749fad" textcol="ff749fad" title="Free Enveloppe 1"/>
  <GROUPCOMPONENT name="env 2 group" id="c35474bb62378ab6" memberName="env2Group"
                  virtualName="" explicitFocusOrder="0" pos="0% 42.988% 59.056% 18.531%"
                  outlinecol="ff749fad" textcol="ff749fad" title="Free Enveloppe 2"/>
  <GROUPCOMPONENT name="step sequencer group" id="edf809d50c7eeefc" memberName="stepSeqGroup"
                  virtualName="" explicitFocusOrder="0" pos="0% 61.018% 59.056% 38.063%"
                  outlinecol="ff749fad" textcol="ff749fad" title=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

