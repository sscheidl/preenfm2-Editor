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
		int midiChannelOverride = 0) noexcept;
	bool queueNrpn(int midiChannel, int parameter, int value) noexcept;
	uint64_t getDroppedOutputEventCount() const noexcept {
		return droppedOutputEvents.load(std::memory_order_relaxed);
	}
	// == MidiInputCallback
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage &message);
	void handlePartialSysexMessage(MidiInput* source, const uint8 *messageData, int numBytesSoFar, double timestamp);
	// Listeners
	void addListener(MidiInputCallback *listener);
	void removeListener(MidiInputCallback *listener);


private:
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
	};

	struct OutputQueueCell
	{
		std::atomic<size_t> sequence { 0 };
		OutputEvent event;
	};

	bool claimOutputQueueCell(size_t& position, OutputQueueCell*& cell) noexcept;
	bool dequeueOutputEvent(OutputEvent& event) noexcept;
	void finishEnqueue(size_t position, OutputQueueCell& cell) noexcept;
	void sendOutputEvent(const OutputEvent& event);
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
};




#endif
