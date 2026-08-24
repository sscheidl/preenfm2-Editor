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

#pragma once

//[Headers]     -- You can add your own extra header files here --

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
#include "Enveloppe.h"
#include "PanelOfComponents.h"

#define NUMBER_OF_OPERATORS 6
#define NUMBER_OF_IM 6
#define NUMBER_OF_MIX 6
#define NUMBER_OF_ALGO 32

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
	An auto-generated component, created by the Introjucer.

	Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PanelEngine  : public Component,
                     public Slider::Listener,
                     public Button::Listener,
                     public ComboBox::Listener,
                     public PanelOfComponents
{
public:
    //==============================================================================
    PanelEngine ();
    ~PanelEngine() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void buttonClicked(Button* buttonThatWasClicked) override;
	void sliderDragStarted(Slider* slider)	override;
	void sliderDragEnded(Slider* slider) override;
	void sliderValueChanged(Slider* sliderThatWasMoved) override;
	void sliderValueChanged(Slider* sliderThatWasMoved, bool fromPluginUI);
	void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	void comboBoxChanged(ComboBox* comboBoxThatHasChanged, bool fromPluginUI);
	void newAlgo(int algoNumber);
	void resizeAlgoDrawableImage();
	// Panel of parameters
	void buildParameters() override;
	void updateSliderFromParameter_hook(Slider* slider) override;
	void updateComboFromParameter_hook(ComboBox* combo) override;
	void updateUIEnveloppe(String paramName) override;
	bool containsThisParameterAsEnveloppe(String name) override;
	void setPfmType(int type) override;
	void enableComponent(Component* comp, bool enable);
	void hideTotallyComponent(Component* comp, bool enable);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	std::unique_ptr<Enveloppe> enveloppe[NUMBER_OF_OPERATORS];
	std::unique_ptr<TextButton> envCopyButton, envPasteButton;
	int envToCopy, envSelected;
	std::unique_ptr<TextButton> enveloppeButton[NUMBER_OF_OPERATORS];
	std::unique_ptr<Slider> mixKnob[NUMBER_OF_MIX];
	std::unique_ptr<Slider> panKnob[NUMBER_OF_MIX];
	std::unique_ptr<Label> mixLabel[NUMBER_OF_MIX];
	std::unique_ptr<Label> IMLabel;
	std::unique_ptr<Label> IMVelocityLabel;
	std::unique_ptr<Label> IMNumber[NUMBER_OF_IM];
	std::unique_ptr<Slider> IMKnob[NUMBER_OF_IM];
	std::unique_ptr<Slider> IMVelocityKnob[NUMBER_OF_IM];
	std::unique_ptr<Label> algoChooserLabel;
	std::unique_ptr<Slider> algoChooser;
	std::unique_ptr<Component> algoDrawableImage;
	std::unique_ptr<Label> velocityLabel;
	std::unique_ptr<Slider> velocity;
	std::unique_ptr<Label> voicesLabel;
	std::unique_ptr<Slider> voices;
	std::unique_ptr<ComboBox> playModePfm3;
	std::unique_ptr<ComboBox> playModePfm2;
	std::unique_ptr<Label> glideLabel;
	std::unique_ptr<Slider> glide;
	std::unique_ptr<Label> glideTypeLabel;
	std::unique_ptr<ComboBox> glideType;

	std::unique_ptr<Label> unisonSpreadLabel;
	std::unique_ptr<Slider> unisonSpread;
	std::unique_ptr<Label> unisonDetuneLabel;
	std::unique_ptr<Slider> unisonDetune;

	std::unique_ptr<Label> opShapeLabel;
	std::unique_ptr<ComboBox> opShape[NUMBER_OF_OPERATORS];

	std::unique_ptr<Label> opFrequencyTypeLabel;
	std::unique_ptr<ComboBox> opFrequencyType[NUMBER_OF_OPERATORS];

	std::unique_ptr<Label> opFrequencyLabel;
	std::unique_ptr<Slider> opFrequency[NUMBER_OF_OPERATORS];

	std::unique_ptr<Label> opFrequencyFineTuneLabel;
	std::unique_ptr<Slider> opFrequencyFineTune[NUMBER_OF_OPERATORS];

	MidiBuffer* eventsToAdd;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::GroupComponent> operatorGroup;
    std::unique_ptr<juce::GroupComponent> mixerGroup;
    std::unique_ptr<juce::GroupComponent> imGroup;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelEngine)
};

//[EndFile] You can add extra defines here...
struct AlgoInformation {
	unsigned char osc;
	unsigned char im;
	unsigned char mix;
};
//[/EndFile]

