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


#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include <cstdint>
#include <unordered_set>
#include "JuceHeader.h"
#include "PluginProcessor.h"

class MainTabs;

//==============================================================================
/**
*/
class Pfm2AudioProcessorEditor : public AudioProcessorEditor, public Timer
{
	friend class Pfm2AudioProcessor;
public:
	static constexpr int minimumWidth = 900;
	static constexpr int minimumHeight = 710;
	static constexpr int defaultWidth = 1000;
	static constexpr int defaultHeight = 750;

	Pfm2AudioProcessorEditor(Pfm2AudioProcessor* ownerFilter);
	~Pfm2AudioProcessorEditor();

	//==============================================================================
	// This is just a standard Juce paint method...
	void paint(Graphics& g);
	void resized();
	void timerCallback();
	void setMidiChannel(int newMidiChannel);
	void setPfmType(int pfmType);
	void setPresetName(String presetName);

private:
	bool uiOutOfSync = false;
	uint64_t lastDroppedOutputEventCount = 0;
	uint64_t lastDroppedIncomingNrpnEventCount = 0;
	MainTabs* mainTabs = nullptr;
	Pfm2AudioProcessor* ownerFilter = nullptr;
};


#endif  // PLUGINEDITOR_H_INCLUDED
