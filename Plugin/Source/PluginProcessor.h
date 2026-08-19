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

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <map>
#include <mutex>
#include <unordered_set>
#include "JuceHeader.h"
#include "PreenNrpn.h"
#include "Pfm2MidiDevice.h"
#include "MidifiedFloatParameter.h"




struct Nrpn {
	uint8 paramMSB = 0;
	uint8 paramLSB = 0;
	uint8 valueMSB = 0;
	uint8 valueLSB = 0;
	bool hasParamMSB = false;
	bool hasParamLSB = false;
	bool hasValueMSB = false;
};



class Pfm2AudioProcessorEditor;
//==============================================================================
/**
*/
class Pfm2AudioProcessor : public AudioProcessor, public MidiInputCallback, private AsyncUpdater
{
public:
	//==============================================================================
	Pfm2AudioProcessor();
	~Pfm2AudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock);
	void releaseResources();

	void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

	//==============================================================================
	AudioProcessorEditor* createEditor();
	bool hasEditor() const;

	//==============================================================================
	const String getName() const;

	void hostParameterChanged(int index);

	bool acceptsMidi() const;
	bool producesMidi() const;
	double getTailLengthSeconds() const;

	//==============================================================================
	int getNumPrograms();
	int getCurrentProgram();
	void setCurrentProgram(int index);
	const String getProgramName(int index);
	void changeProgramName(int index, const String& newName);

	//==============================================================================
	void getStateInformation(MemoryBlock& destData);
	void setStateInformation(const void* data, int sizeInBytes, bool flushNewStateToPreenfm);
	void setStateInformation(const void* data, int sizeInBytes);


	void handleIncomingNrpn(int param, int value);
	// Parameter observer
	bool isRealtimePriority() const;
	void onParameterUpdated(AudioProcessorParameter *parameter);
	// accessed from editor so must be public
	MidiMessageCollector midiMessageCollector;
	struct Nrpn currentNrpn;
	void flushAllParametrsToNrpn();
	void sendNrpnPresetName();
	void setPresetName(String newName);
	String getPresetName() const;
	void editorClosed(Pfm2AudioProcessorEditor* editor);
	void editorResized(int width, int height) noexcept;
	LookAndFeel* getEditorLookAndFeel() const { return myLookAndFeel; }

	void addMidifiedParameter(MidifiedFloatParameter *param);

	void parameterUpdatedForUI(int p);
	void consumePendingUiParameterUpdates(std::unordered_set<String>& updates);
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage &message);
	void handlePartialSysexMessage(MidiInput* source, const uint8 *messageData, int numBytesSoFar, double timestamp);
	void choseNewMidiDevice();

	// Called from PfmPreset in standalone version only
	void setParameterWithNrpmParamAndRealValue(int param, float pfmValue);
	float getRealValueForPfmBank(int param);
	void setPfmType(int pt);
	int getPfmType() { return pfmType;  }
	uint64_t getDroppedOutputEventCount() const noexcept;
	uint64_t getDroppedIncomingNrpnEventCount() const noexcept {
		return droppedIncomingNrpnEvents.load(std::memory_order_relaxed);
	}

private:
	static constexpr int maximumPresetNameLength = 12;
	struct IncomingNrpnEvent
	{
		int parameter = 0;
		int value = 0;
	};

	static constexpr int incomingNrpnQueueCapacity = 4096;
	static constexpr int maximumPendingUiParameters = 1024;
	static constexpr int pendingUiWordCount = maximumPendingUiParameters / 64;
	void queueIncomingNrpn(int parameter, int value) noexcept;
	void handleAsyncUpdate() override;
	void clearPendingUiParameterUpdate(int parameterIndex) noexcept;
	void requestEditorStateUpdate() noexcept;
	void sendMidiForParameter(int paramIndex, int nrpnValue);
	String presetNameWithCharacter(int characterIndex, int characterValue) const;
	void queueParameterNrpn(const MidifiedFloatParameter* parameter);
	static constexpr int nrpnLookupSize = 2048;
	int nrpmIndex[nrpnLookupSize];
	int nrpmIndexPfm3[nrpnLookupSize];
	mutable CriticalSection presetNameLock;
	String presetName;
	std::atomic<bool> pendingEditorStateUpdate { false };
	std::atomic<int> currentMidiChannel { 1 };
	std::atomic<int> pfmType { 1 };
	AbstractFifo incomingNrpnFifo { incomingNrpnQueueCapacity };
	std::array<IncomingNrpnEvent, incomingNrpnQueueCapacity> incomingNrpnQueue;
	std::atomic<uint64_t> droppedIncomingNrpnEvents { 0 };
	std::array<std::atomic<uint64_t>, pendingUiWordCount> pendingUiParameterUpdates;
	// Shared by all plugin instances
	SharedResourcePointer<Pfm2MidiDevice> pfm2MidiDevice;
    std::atomic<int> editorWidth { 0 };
    std::atomic<int> editorHeight { 0 };

	// Those ones are important
	MidifiedFloatParameter *playModeParam, *voicesParam;

	Pfm2AudioProcessorEditor* pfm2Editor;
	LookAndFeel* myLookAndFeel;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pfm2AudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
