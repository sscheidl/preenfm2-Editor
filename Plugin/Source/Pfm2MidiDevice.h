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

#ifndef PFM2MIDIDEVICE_H_INCLUDED
#define PFM2MIDIDEVICE_H_INCLUDED

#include <array>
#include <atomic>
#include <cstdint>
#include "JuceHeader.h"
#include "MidiPerformanceRouter.h"

// This class must be used with a SharedResourcePointer so that multiple instance of a plugin use the same port
class Pfm2MidiDevice : public MidiInputCallback, private Thread
{
public:
	// There's no need to ever create an instance of this class directly yourself,
	// but it does need a public constructor that does the initialisation.
	Pfm2MidiDevice();
	~Pfm2MidiDevice();

	void resetDevices();
	void choseNewDevices();
	void forceChoseNewDevices();
	bool queueMidiMessage(const MidiMessage& message) noexcept;
	bool queueMidiMessage(const uint8_t* messageData, int messageSize,
		int midiChannelOverride = 0, bool protectPreenfmProtocol = false) noexcept;
	bool queueNrpn(int midiChannel, int parameter, int value) noexcept;
	uint64_t getSuppressedConfigurationCount() const noexcept {
		return suppressedConfigurationMessages.load(std::memory_order_relaxed);
	}
	uint64_t getDroppedOutputEventCount() const noexcept {
		return droppedOutputEvents.load(std::memory_order_relaxed);
	}

	//==========================================================================
	// Editor remote protocol (firmware 3.00 alpha).
	//
	// Two separate concerns, deliberately not merged into one global lock:
	//
	//  1. Response correlation - who may complete a transaction. The wire
	//     protocol carries no request id, so transactions are serialised here
	//     and identified by a monotonically increasing token. A late answer
	//     belonging to an older generation can therefore never complete a
	//     newer transaction.
	//
	//  2. MIDI exclusivity - see queueNrpnBatch(). That is an ordering
	//     property of the output queue and has nothing to do with who owns a
	//     transaction.
	//==========================================================================
	static constexpr uint32_t invalidTransactionToken = 0;

	// Returns invalidTransactionToken when another transaction is open. Not
	// re-entrant on purpose: a second claim by the same caller would let a
	// partial completion release the shared claim too early.
	uint32_t tryBeginEditorTransaction() noexcept;
	// Only the holder of this exact token can release, so a late release from
	// a transaction that already timed out cannot free a newer one.
	void endEditorTransaction(uint32_t token) noexcept;
	bool ownsEditorTransaction(uint32_t token) const noexcept {
		return token != invalidTransactionToken
			&& editorTransactionToken.load(std::memory_order_acquire) == token;
	}

	//==========================================================================
	// Atomic output batch.
	//
	// The firmware stores the live edit buffer, so a store is only correct if
	// nothing modifies that buffer between the first byte of the pushed
	// snapshot and the store request itself. The output queue is a shared MPMC
	// ring: normally every producer claims one cell at a time, so a foreign
	// event could land in the middle of a push.
	//
	// queueNrpnBatch() reserves a contiguous run of cells with a single CAS on
	// the enqueue position. Competing producers therefore always end up either
	// completely before or completely after the batch, never inside it, and
	// they are never blocked while doing so - they simply get a later
	// position. The consumer emits strictly in position order.
	//
	// Returns false without enqueueing anything when the run does not fit, so
	// the caller can abort the store instead of writing a partial patch.
	struct NrpnItem
	{
		uint16_t parameter = 0;
		uint16_t value = 0;
	};
	bool queueNrpnBatch(int midiChannel, const NrpnItem* items,
		size_t itemCount) noexcept;

	//==========================================================================
	// Device generation.
	//
	// Incremented on every device change. Events carry the generation they
	// were created for and are discarded instead of being sent to a device
	// the caller never addressed. A store batch built for generation N must
	// not reach generation N+1 after a reconnect.
	//==========================================================================
	uint32_t getDeviceGeneration() const noexcept {
		return deviceGeneration.load(std::memory_order_acquire);
	}
	// Events that were dequeued but could not be delivered: no open device, or
	// a stale generation. Queue success is not send success, so this has to be
	// visible separately from droppedOutputEvents.
	uint64_t getUndeliveredOutputEventCount() const noexcept {
		return undeliveredOutputEvents.load(std::memory_order_relaxed);
	}

	// == MidiInputCallback
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage &message);
	void handlePartialSysexMessage(MidiInput* source, const uint8 *messageData, int numBytesSoFar, double timestamp);
	// Listeners
	void addListener(MidiInputCallback *listener);
	void removeListener(MidiInputCallback *listener);


private:
	friend struct MidiRoutingTestAccess;
	enum class OutputEventType : uint8_t
	{
		midiMessage,
		nrpn
	};

	static constexpr size_t outputQueueCapacity = 2048;
	static constexpr size_t maximumQueuedMidiMessageBytes = 4096;

	struct OutputEvent
	{
		OutputEventType type = OutputEventType::midiMessage;
		uint16_t messageSize = 0;
		std::array<uint8_t, maximumQueuedMidiMessageBytes> messageData;
		uint8_t midiChannel = 1;
		uint16_t nrpnParameter = 0;
		uint16_t nrpnValue = 0;
		bool protectPreenfmProtocol = false;
		// Device generation this event was created for.
		uint32_t deviceGeneration = 0;
	};

	struct OutputQueueCell
	{
		std::atomic<size_t> sequence { 0 };
		OutputEvent event;
	};

	bool claimOutputQueueCell(size_t& position, OutputQueueCell*& cell) noexcept;
	// Reserves itemCount contiguous cells with one CAS, see queueNrpnBatch().
	bool claimOutputQueueRun(size_t itemCount, size_t& firstPosition) noexcept;
	bool dequeueOutputEvent(OutputEvent& event) noexcept;
	void finishEnqueue(size_t position, OutputQueueCell& cell) noexcept;
	void sendOutputEvent(const OutputEvent& event);
	// Called only after the output worker has stopped, or by the worker itself.
	template <typename Sink> void drainOutputQueue(Sink&& send) {
		OutputEvent event;
		while (dequeueOutputEvent(event)) send(event);
	}
	template <typename Sink> void emitOutputEvent(const OutputEvent& event, Sink&& send) {
		if (event.deviceGeneration != deviceGeneration.load(std::memory_order_acquire)) {
			++undeliveredOutputEvents;
			return;
		}
		if (event.type == OutputEventType::midiMessage) {
			if (event.protectPreenfmProtocol && isPreenfmConfigurationMessage(
				event.messageData.data(), event.messageSize)) {
				++suppressedConfigurationMessages;
				return;
			}
			send(event.messageData.data(), event.messageSize);
		} else {
			emitEditorNrpn(event.midiChannel, event.nrpnParameter, event.nrpnValue,
				[&send](int channel, int cc, int value) {
					const uint8_t bytes[] = {static_cast<uint8_t>(0xb0 | (channel - 1)),
						static_cast<uint8_t>(cc), static_cast<uint8_t>(value)};
					send(bytes, 3);
				});
		}
	}
	void run() override;

	PropertiesFile* midiPropertyFile;
	std::atomic<bool> showErrorMEssage { false };
	CriticalSection messageLock;
	std::unique_ptr<MidiOutput> pfm2MidiOutput;
	String currentMidiOutputDevice;
	std::unique_ptr<MidiInput> pfm2MidiInput;
	String currentMidiInputDevice;
	ThreadSafeListenerList<MidiInputCallback> listeners;
	std::array<OutputQueueCell, outputQueueCapacity> outputQueue;
	alignas(64) std::atomic<size_t> outputEnqueuePosition { 0 };
	alignas(64) std::atomic<size_t> outputDequeuePosition { 0 };
	std::atomic<uint64_t> droppedOutputEvents { 0 };
	std::atomic<uint64_t> suppressedConfigurationMessages { 0 };
	// Dequeued but not delivered: no open device, or stale generation.
	std::atomic<uint64_t> undeliveredOutputEvents { 0 };
	// Token of the open editor-protocol transaction, 0 when free.
	std::atomic<uint32_t> editorTransactionToken { 0 };
	std::atomic<uint32_t> nextEditorTransactionToken { 1 };
	// Bumped on every device change so stale events can be discarded.
	std::atomic<uint32_t> deviceGeneration { 1 };
};




#endif
