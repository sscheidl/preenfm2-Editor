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
#include <vector>
#include "JuceHeader.h"
#include "PreenNrpn.h"
#include "Pfm2MidiDevice.h"
#include "MidifiedFloatParameter.h"
#include "MidiPerformanceRouter.h"




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
class Pfm2AudioProcessor : public AudioProcessor,
	public MidiInputCallback,
	private AsyncUpdater,
	private Timer
{
public:
	//==============================================================================
	Pfm2AudioProcessor();
	~Pfm2AudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock);
	void releaseResources();

	void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
	void processBlockBypassed(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;

	//==============================================================================
	AudioProcessorEditor* createEditor();
	bool hasEditor() const;

	//==============================================================================
	const String getName() const;

	void hostParameterChanged(int index);

	bool acceptsMidi() const;
	bool producesMidi() const;
	bool supportsMPE() const override { return true; }
	bool isMpeEnabled() const noexcept { return mpeEnabled.load(std::memory_order_relaxed); }
	void setMpeEnabled(bool enabled, bool notifyHost = true);
	uint64_t getSuppressedConfigurationCount() const noexcept {
		return pfm2MidiDevice->getSuppressedConfigurationCount();
	}
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
	void choseNewMidiDevice(Component* dialogParent = nullptr);
	void setHardwarePresetTarget(int bankNumber, int presetNumber) noexcept;
	void loadHardwarePreset(int bankNumber, int presetNumber);

	//==========================================================================
	// Editor remote protocol, firmware 3.00 alpha.
	//
	// Threading invariant: every member below is touched only on the message
	// thread. Responses arrive on the MIDI thread but are handed over through
	// the existing incomingNrpnFifo and are decoded in handleAsyncUpdate();
	// the timeouts run on the JUCE Timer and the editor polls from its own
	// timer. No lock is therefore required, and no GUI work happens in the
	// MIDI callback.
	//==========================================================================
	enum class EditorProtocolState {
		unknown,      // never queried
		querying,     // capability request in flight
		unsupported,  // no answer: old firmware, or Receives is not NRPN
		supported     // protocol version and capability bits confirmed
	};

	enum class HardwareStoreState {
		idle,
		awaitingEcho,    // store sent, waiting for STORE_TARGET
		awaitingStatus   // target echo matched, waiting for STORE_STATUS
	};

	// Sends a capability query. Safe to call repeatedly; the UI offers this as
	// an explicit re-check so the plugin does not have to be reloaded.
	void requestEditorCapabilities();
	// Sends a position query (request LSB 1).
	void requestHardwarePosition();
	// Pushes the full patch and then stores it into the given one-based slot.
	// Returns false when the protocol is unsupported, another transaction is
	// open, or the target is invalid; the UI then reports why.
	bool beginHardwareStore(int bankNumber, int presetNumber);

	EditorProtocolState getEditorProtocolState() const noexcept {
		return editorProtocolState;
	}
	bool isStoreSupported() const noexcept {
		return !protocolContextChanged.load(std::memory_order_acquire)
			&& editorProtocolState == EditorProtocolState::supported
			&& (editorCapabilities & PREENFM_EDITOR_CAPABILITY_STORE) != 0;
	}
	bool isPositionQuerySupported() const noexcept {
		return !protocolContextChanged.load(std::memory_order_acquire)
			&& editorProtocolState == EditorProtocolState::supported
			&& (editorCapabilities & PREENFM_EDITOR_CAPABILITY_POSITION_QUERY) != 0;
	}
	bool isHardwareBusy() const noexcept {
		return storeState != HardwareStoreState::idle || positionQueryPending
			|| editorProtocolState == EditorProtocolState::querying
			|| hardwarePresetPullPending.load(std::memory_order_acquire);
	}
	// A timed-out store leaves the target in an unknown condition. Storing is
	// latched off until an explicit position resync, and never retried
	// automatically.
	bool isStoreBlockedByUnknownOutcome() const noexcept {
		return storeOutcomeUnknown;
	}
	bool canStartStore() const noexcept {
		return isStoreSupported() && !isHardwareBusy() && !storeOutcomeUnknown;
	}
	// One-based reported hardware position, 0 when unknown or VALID was 0.
	int getReportedHardwareBank() const noexcept { return reportedBank; }
	int getReportedHardwarePreset() const noexcept { return reportedPreset; }
	bool isReportedHardwarePositionValid() const noexcept {
		return reportedPositionValid;
	}
	String getEditorProtocolStatusText() const;
	String getLastOperationText() const { return lastOperationText; }
	// Incremented on every protocol state change so the editor can refresh
	// without polling every field.
	int getProtocolRevision() const noexcept { return protocolRevision; }
	int getHardwarePresetBank() const noexcept {
		return hardwarePresetBank.load(std::memory_order_relaxed);
	}
	int getHardwarePresetNumber() const noexcept {
		return hardwarePresetNumber.load(std::memory_order_relaxed);
	}

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
	friend struct MidiRoutingTestAccess;
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
	void timerCallback() override;
	void clearPendingUiParameterUpdate(int parameterIndex) noexcept;
	void requestEditorStateUpdate() noexcept;
	void setControlChannel(int channel) noexcept;
	void invalidateProtocolContext();
	void markProtocolContextChanged() noexcept;
	void sendMidiForParameter(int paramIndex, int nrpnValue);
	void queueHardwarePresetSelection(int bankNumber, int presetNumber);
	void requestCurrentHardwarePreset();
	String presetNameWithCharacter(int characterIndex, int characterValue) const;
	void queueParameterNrpn(const MidifiedFloatParameter* parameter);
	static constexpr int nrpnLookupSize = 2048;
	int nrpmIndex[nrpnLookupSize];
	int nrpmIndexPfm3[nrpnLookupSize];
	mutable CriticalSection presetNameLock;
	String presetName;
	std::atomic<bool> pendingEditorStateUpdate { false };
	std::atomic<int> currentMidiChannel { 1 };
	std::atomic<bool> mpeEnabled { false };
	MidiPerformanceRouter performanceRouter;
	std::atomic<bool> protocolContextChanged { false };
	std::atomic<int> pfmType { 1 };
	std::atomic<int> hardwarePresetBank { 1 };
	std::atomic<int> hardwarePresetNumber { 1 };
	std::atomic<bool> hardwarePresetPullPending { false };
	uint32 hardwarePresetPullDeadline = 0;

	//==========================================================================
	// Editor remote protocol state. Message thread only, see the note above.
	//==========================================================================
	// Decodes one page-4 response. Called before the normal parameter lookup.
	void handleEditorProtocolResponse(int responseId, int value);
	void sendEditorRequest(int requestId, int value);
	// Claims the shared transaction and records the token plus the device
	// generation it belongs to. Returns false when another one is open.
	bool beginEditorTransaction();
	void finishEditorTransaction();
	// True while the recorded device generation still matches the device.
	bool isTransactionDeviceCurrent() const noexcept;
	void abandonTransactionForDeviceChange();
	// Builds the frozen snapshot: 12 name letters, every hardware parameter,
	// and finally the store request, as one contiguous batch.
	bool buildStoreBatch(int storeTargetValue,
		std::vector<Pfm2MidiDevice::NrpnItem>& items) const;
	void serviceEditorProtocolTimeouts(uint32 now);
	void noteProtocolChange(const String& operationText);
	void applyReportedPosition();

	// Short timeout for queries. The firmware answers immediately; this only
	// has to cover MIDI transport latency.
	static constexpr int editorQueryTimeoutMs = 700;
	// The firmware sends the store status only after savePreenFMPatch() has
	// returned, so a file system write is inside this window.
	static constexpr int editorStoreTimeoutMs = 6000;
	static constexpr int editorTimerIntervalMs = 40;

	EditorProtocolState editorProtocolState = EditorProtocolState::unknown;
	int editorProtocolVersion = 0;
	int editorCapabilities = 0;
	bool capabilityVersionSeen = false;
	uint32 capabilityDeadline = 0;

	bool positionQueryPending = false;
	uint32 positionDeadline = 0;
	// Accumulated position response group. Only applied once bank type, bank,
	// preset and the valid flag have all arrived.
	int pendingBankType = -1;
	int pendingBank = -1;
	int pendingPreset = -1;
	int pendingValid = -1;
	int reportedBank = 0;      // one-based, 0 = unknown
	int reportedPreset = 0;    // one-based, 0 = unknown
	bool reportedPositionValid = false;
	// Set while a load waits for a position confirmation before pulling.
	bool positionQueryForLoad = false;
	int loadTargetBank = 0;
	int loadTargetPreset = 0;

	HardwareStoreState storeState = HardwareStoreState::idle;
	int storeTargetWire = -1;   // requested (bank << 7) | preset, zero based
	int storeTargetBank = 0;    // one-based, for messages
	int storeTargetPreset = 0;  // one-based, for messages
	uint32 storeDeadline = 0;

	// Token of the transaction this instance currently holds, 0 when none.
	uint32_t transactionToken = Pfm2MidiDevice::invalidTransactionToken;
	// Device generation the open transaction was started for.
	uint32_t transactionDeviceGeneration = 0;

	// A store that timed out has an unknown outcome: the firmware may have
	// written the slot already. The state is latched so no further store can
	// be started, and no automatic retry ever happens, until an explicit
	// position resync clears it.
	bool storeOutcomeUnknown = false;
	int unknownStoreBank = 0;
	int unknownStorePreset = 0;

	String lastOperationText;
	int protocolRevision = 0;
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
