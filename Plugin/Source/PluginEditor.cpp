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

#include "UI/MainTabs.h"

//==============================================================================
Pfm2AudioProcessorEditor::Pfm2AudioProcessorEditor(Pfm2AudioProcessor* ownerFilter)
	: AudioProcessorEditor(ownerFilter)
{
	this->ownerFilter = ownerFilter;
	setLookAndFeel(ownerFilter->getEditorLookAndFeel());
	addAndMakeVisible(mainTabs = new MainTabs());
	mainTabs->buildParameters(getAudioProcessor());
	sendLookAndFeelChange();
	setResizable(true, true);
	setResizeLimits(minimumWidth, minimumHeight, 4096, 4096);
	// This is where our plugin's editor size is set.

	setSize(defaultWidth, defaultHeight);

	startTimer(100);
	uiOutOfSync = false;
}

Pfm2AudioProcessorEditor::~Pfm2AudioProcessorEditor()
{
	this->ownerFilter->editorClosed(this);
	setLookAndFeel(nullptr);
	delete mainTabs;
}

//==============================================================================
void Pfm2AudioProcessorEditor::paint(Graphics&)
{
}


void Pfm2AudioProcessorEditor::resized() {
	if (mainTabs != nullptr)
		mainTabs->setBounds(getLocalBounds());
	ownerFilter->editorResized(getWidth(), getHeight());
}


void Pfm2AudioProcessorEditor::timerCallback() {
	std::unordered_set<String> newSet;
	ownerFilter->consumePendingUiParameterUpdates(newSet);
	if (!newSet.empty()) {
		mainTabs->updateUI(newSet);
	}

	const uint64_t droppedOutput = ownerFilter->getDroppedOutputEventCount();
	const uint64_t droppedInput = ownerFilter->getDroppedIncomingNrpnEventCount();
	if (droppedOutput != lastDroppedOutputEventCount
		|| droppedInput != lastDroppedIncomingNrpnEventCount) {
		lastDroppedOutputEventCount = droppedOutput;
		lastDroppedIncomingNrpnEventCount = droppedInput;
		mainTabs->setMidiQueueWarning(droppedOutput, droppedInput);
	}
}


void Pfm2AudioProcessorEditor::setMidiChannel(int newMidiChannel) {
	mainTabs->setMidiChannel(newMidiChannel);
}

void Pfm2AudioProcessorEditor::setPfmType(int pfmType) {
	mainTabs->setPfmType(pfmType);
}

void Pfm2AudioProcessorEditor::setPresetName(String presetName) {
	mainTabs->setPresetName(presetName);
}




