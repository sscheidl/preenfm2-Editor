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

#include "Pfm2MidiDevice.h"
#include "MidiPerformanceRouter.h"

#define MIDI_INPUT "midiInput"
#define MIDI_OUTPUT "midiOutput"

Pfm2MidiDevice::Pfm2MidiDevice()
	: Thread("PreenFM MIDI output")
{
	for (size_t index = 0; index < outputQueue.size(); ++index) {
		outputQueue[index].sequence.store(index, std::memory_order_relaxed);
	}

	String pfm2InputDevice = "PreenFM mk2";
	String pfm2OutputDevice = "PreenFM mk2";

	PropertiesFile::Options options;
	options.commonToAllUsers = false;
	options.applicationName = ProjectInfo::projectName;
	options.osxLibrarySubFolder = "Application Support/preenfm2 Editor";
	options.filenameSuffix = ".midi.xml";
	options.storageFormat = PropertiesFile::StorageFormat::storeAsXML;

	midiPropertyFile = new PropertiesFile(options);

	if (midiPropertyFile->getFile().exists()) {
		if (midiPropertyFile->containsKey(MIDI_INPUT)) {
			pfm2InputDevice = midiPropertyFile->getValue(MIDI_INPUT);
		}

		if (midiPropertyFile->containsKey(MIDI_OUTPUT)) {
			pfm2OutputDevice = midiPropertyFile->getValue(MIDI_OUTPUT);
		}
	}

	pfm2MidiOutput = nullptr;
	pfm2MidiInput = nullptr;

	const auto outputDevices = MidiOutput::getAvailableDevices();

	for (const auto& device : outputDevices) {
		DBG("Output : " << device.name);
		if (device.name == pfm2OutputDevice) {
			pfm2MidiOutput = MidiOutput::openDevice(device.identifier);
			if (pfm2MidiOutput.get() != nullptr) {
				DBG("Output found :)");
				currentMidiOutputDevice = device.name;
			}
			else {
				DBG("Output could not be open)");
			}
			break;
		}
	}

	const auto inputDevices = MidiInput::getAvailableDevices();
	for (const auto& device : inputDevices) {
		DBG("Input : " << device.name);
		if (device.name == pfm2InputDevice) {
			pfm2MidiInput = MidiInput::openDevice(device.identifier, this);
			if (pfm2MidiInput.get() != nullptr) {
				pfm2MidiInput->start();
				currentMidiInputDevice = device.name;
				DBG("Input found :)");
			}
			else {
				DBG("Input could not be open)");
			}
			break;
		}
	}

	startThread(Thread::Priority::normal);
}

Pfm2MidiDevice::~Pfm2MidiDevice() {
	stopThread(2000);
	// A host deactivates the last processor immediately before destroying this
	// shared resource. Deliver its queued releases before invalidating the port.
	drainOutputQueue([this](const OutputEvent& event) { sendOutputEvent(event); });
	resetDevices();
	delete midiPropertyFile;
}

void Pfm2MidiDevice::resetDevices() {
	// Anything still queued belongs to the device we are closing. Bumping the
	// generation makes the worker discard those events instead of replaying
	// them into whatever is opened next.
	deviceGeneration.fetch_add(1, std::memory_order_acq_rel);

	if (pfm2MidiInput) {
		pfm2MidiInput->stop();
		pfm2MidiInput.reset();
	}
	if (pfm2MidiOutput) {
		pfm2MidiOutput.reset();
	}
}


bool Pfm2MidiDevice::claimOutputQueueCell(size_t& position, OutputQueueCell*& cell) noexcept {
	position = outputEnqueuePosition.load(std::memory_order_relaxed);

	for (;;) {
		cell = &outputQueue[position % outputQueueCapacity];
		const size_t sequence = cell->sequence.load(std::memory_order_acquire);
		const auto difference = static_cast<std::intptr_t>(sequence)
			- static_cast<std::intptr_t>(position);

		if (difference == 0) {
			if (outputEnqueuePosition.compare_exchange_weak(position, position + 1,
				std::memory_order_relaxed)) {
				return true;
			}
		}
		else if (difference < 0) {
			return false;
		}
		else {
			position = outputEnqueuePosition.load(std::memory_order_relaxed);
		}
	}
}

void Pfm2MidiDevice::finishEnqueue(size_t position, OutputQueueCell& cell) noexcept {
	cell.sequence.store(position + 1, std::memory_order_release);
}

bool Pfm2MidiDevice::queueMidiMessage(const MidiMessage& message) noexcept {
	return queueMidiMessage(message.getRawData(), message.getRawDataSize());
}

bool Pfm2MidiDevice::queueMidiMessage(const uint8_t* messageData,
	int messageSize, int midiChannelOverride, bool protectPreenfmProtocol) noexcept {
	if (messageData == nullptr || messageSize <= 0
		|| messageSize > static_cast<int>(maximumQueuedMidiMessageBytes)) {
		++droppedOutputEvents;
		return false;
	}
	if (midiChannelOverride < 0 || midiChannelOverride > 16) {
		++droppedOutputEvents;
		return false;
	}

	size_t position = 0;
	OutputQueueCell* cell = nullptr;
	if (!claimOutputQueueCell(position, cell)) {
		++droppedOutputEvents;
		return false;
	}

	cell->event.type = OutputEventType::midiMessage;
	cell->event.protectPreenfmProtocol = protectPreenfmProtocol;
	cell->event.messageSize = static_cast<uint16_t>(messageSize);
	std::memcpy(cell->event.messageData.data(), messageData,
		static_cast<size_t>(messageSize));

	const uint8_t statusByte = cell->event.messageData[0];
	if (midiChannelOverride > 0 && statusByte >= 0x80 && statusByte < 0xf0) {
		cell->event.messageData[0] = static_cast<uint8_t>(
			(statusByte & 0xf0) | (midiChannelOverride - 1));
	}

	cell->event.deviceGeneration = deviceGeneration.load(std::memory_order_acquire);
	finishEnqueue(position, *cell);
	return true;
}

bool Pfm2MidiDevice::queueNrpn(int midiChannel, int parameter, int value) noexcept {
	if (midiChannel < 1 || midiChannel > 16
		|| parameter < 0 || parameter > 0x3fff
		|| value < 0 || value > 0x3fff) {
		++droppedOutputEvents;
		return false;
	}

	size_t position = 0;
	OutputQueueCell* cell = nullptr;
	if (!claimOutputQueueCell(position, cell)) {
		++droppedOutputEvents;
		return false;
	}

	cell->event.type = OutputEventType::nrpn;
	cell->event.midiChannel = static_cast<uint8_t>(midiChannel);
	cell->event.nrpnParameter = static_cast<uint16_t>(parameter);
	cell->event.nrpnValue = static_cast<uint16_t>(value);
	cell->event.deviceGeneration = deviceGeneration.load(std::memory_order_acquire);
	finishEnqueue(position, *cell);
	return true;
}

bool Pfm2MidiDevice::dequeueOutputEvent(OutputEvent& event) noexcept {
	size_t position = outputDequeuePosition.load(std::memory_order_relaxed);
	OutputQueueCell* cell = nullptr;

	for (;;) {
		cell = &outputQueue[position % outputQueueCapacity];
		const size_t sequence = cell->sequence.load(std::memory_order_acquire);
		const auto difference = static_cast<std::intptr_t>(sequence)
			- static_cast<std::intptr_t>(position + 1);

		if (difference == 0) {
			if (outputDequeuePosition.compare_exchange_weak(position, position + 1,
				std::memory_order_relaxed)) {
				break;
			}
		}
		else if (difference < 0) {
			return false;
		}
		else {
			position = outputDequeuePosition.load(std::memory_order_relaxed);
		}
	}

	event.type = cell->event.type;
	event.protectPreenfmProtocol = cell->event.protectPreenfmProtocol;
	event.deviceGeneration = cell->event.deviceGeneration;
	if (event.type == OutputEventType::midiMessage) {
		event.messageSize = cell->event.messageSize;
		std::memcpy(event.messageData.data(), cell->event.messageData.data(),
			event.messageSize);
	}
	else {
		event.midiChannel = cell->event.midiChannel;
		event.nrpnParameter = cell->event.nrpnParameter;
		event.nrpnValue = cell->event.nrpnValue;
	}

	cell->sequence.store(position + outputQueueCapacity, std::memory_order_release);
	return true;
}

void Pfm2MidiDevice::sendOutputEvent(const OutputEvent& event) {
	const ScopedLock lock(messageLock);

	if (pfm2MidiOutput == nullptr || pfm2MidiInput == nullptr) {
		// Queue success is not send success. Counting this makes an
		// incomplete transfer visible instead of silently dropping it.
		++undeliveredOutputEvents;
		return;
	}

	emitOutputEvent(event, [this](const uint8_t* bytes, int size) {
		pfm2MidiOutput->sendMessageNow(MidiMessage(bytes, size, 0.0));
	});
}

void Pfm2MidiDevice::run() {
	while (!threadShouldExit()) {
		bool sentAnything = false;
		OutputEvent event;
		while (dequeueOutputEvent(event)) {
			sendOutputEvent(event);
			sentAnything = true;
			if (threadShouldExit()) {
				break;
			}
		}

		if (!sentAnything) {
			wait(2);
		}
	}
}


void Pfm2MidiDevice::forceChoseNewDevices() {
	showErrorMEssage.store(true);
	choseNewDevices();
}

void Pfm2MidiDevice::choseNewDevices() {
	if (showErrorMEssage.exchange(false)) {

		AlertWindow midiWindow("Where is your preenfm ?",
			"",
			AlertWindow::QuestionIcon);

		Label errorMessage("");
		errorMessage.setColour(Label::textColourId, Colour::fromRGB(200, 80, 80));
		errorMessage.setSize(400, 20);
		midiWindow.addCustomComponent(&errorMessage);

		const auto inputDevices = MidiInput::getAvailableDevices();
		StringArray devicesFrom;
		devicesFrom.insert(0, "<Select>");
		for (const auto& device : inputDevices) {
			devicesFrom.add(device.name);
		}
		midiWindow.addComboBox("From", devicesFrom, "Input from preenfm");
		int currentInput = devicesFrom.indexOf(currentMidiInputDevice);
		if (currentInput > -1) {
			midiWindow.getComboBoxComponent("From")->setSelectedId(currentInput + 1);
		}
		else {
			midiWindow.getComboBoxComponent("From")->setSelectedId(1);
		}

		const auto outputDevices = MidiOutput::getAvailableDevices();
		StringArray devicesTo;
		devicesTo.insert(0, "<Select>");
		for (const auto& device : outputDevices) {
			devicesTo.add(device.name);
		}
		midiWindow.addComboBox("To", devicesTo, "Output to preenfm");
		int currentOutput = devicesTo.indexOf(currentMidiOutputDevice);
		if (currentOutput > -1) {
			midiWindow.getComboBoxComponent("To")->setSelectedId(currentOutput + 1);
		}
		else {
			midiWindow.getComboBoxComponent("To")->setSelectedId(1);
		}

		//void addButton(const String &name, int returnValue, const KeyPress &shortcutKey1 = KeyPress(), const KeyPress &shortcutKey2 = KeyPress())
		midiWindow.addButton("Cancel", 0);
		midiWindow.addButton("OK", 1);

		int result = 1;
		do {

			do {
				result = midiWindow.runModalLoop();
				errorMessage.setText("You must select both input and output", NotificationType::sendNotification);
				midiWindow.repaint();
			} while ((midiWindow.getComboBoxComponent("From")->getSelectedId() == 1 || midiWindow.getComboBoxComponent("To")->getSelectedId() == 1) && result == 1);

			if (result == 1) {
				const ScopedLock deviceLock(messageLock);

				// Invalidate everything still queued for the old device before
				// any handle is replaced.
				deviceGeneration.fetch_add(1, std::memory_order_acq_rel);

				if (pfm2MidiOutput.get() != nullptr) {
					pfm2MidiOutput.reset();
				}
				if (pfm2MidiInput.get() != nullptr) {
					pfm2MidiInput->stop();
					pfm2MidiInput.reset();
				}

				// -2 because of the <Select>.
				int deviceFrom = midiWindow.getComboBoxComponent("From")->getSelectedId() - 2;
				currentMidiInputDevice = devicesFrom[deviceFrom + 1];
				int deviceTo = midiWindow.getComboBoxComponent("To")->getSelectedId() - 2;
				currentMidiOutputDevice = devicesTo[deviceTo + 1];

				pfm2MidiInput = MidiInput::openDevice(inputDevices[deviceFrom].identifier, this);
				if (pfm2MidiInput.get() != nullptr) {
					pfm2MidiInput->start();
				}
				else {
					errorMessage.setText("Input cannot be open", NotificationType::dontSendNotification);
				}
				// No need to test output if input did not work
				if (pfm2MidiInput.get() != nullptr) {
					pfm2MidiOutput = MidiOutput::openDevice(outputDevices[deviceTo].identifier);
					if (pfm2MidiOutput.get() == nullptr) {
						errorMessage.setText("Output cannot be open", NotificationType::dontSendNotification);
						// let's close input before rexiting
						resetDevices();
					}
					else {
						// We're good
						midiPropertyFile->setValue(MIDI_INPUT, currentMidiInputDevice);
						midiPropertyFile->setValue(MIDI_OUTPUT, currentMidiOutputDevice);
						midiPropertyFile->saveIfNeeded();
					}
				}
			}
		} while ((pfm2MidiInput.get() == nullptr || pfm2MidiOutput.get() == nullptr) && result == 1);
	}
}

uint32_t Pfm2MidiDevice::tryBeginEditorTransaction() noexcept {
	uint32_t expected = invalidTransactionToken;
	uint32_t token = nextEditorTransactionToken.fetch_add(1,
		std::memory_order_relaxed);
	// Never hand out the reserved "free" value, even after a wrap.
	if (token == invalidTransactionToken) {
		token = nextEditorTransactionToken.fetch_add(1,
			std::memory_order_relaxed);
	}

	if (editorTransactionToken.compare_exchange_strong(expected, token,
		std::memory_order_acq_rel, std::memory_order_acquire)) {
		return token;
	}

	// Deliberately not re-entrant: a second claim by the same caller would
	// let a partial completion release the shared claim too early.
	return invalidTransactionToken;
}

void Pfm2MidiDevice::endEditorTransaction(uint32_t token) noexcept {
	if (token == invalidTransactionToken) {
		return;
	}

	uint32_t expected = token;
	// Succeeds only while this exact token is still the open one, so a late
	// release from an already timed-out transaction cannot free a newer one.
	editorTransactionToken.compare_exchange_strong(expected,
		invalidTransactionToken,
		std::memory_order_acq_rel, std::memory_order_acquire);
}

bool Pfm2MidiDevice::claimOutputQueueRun(size_t itemCount,
	size_t& firstPosition) noexcept {
	if (itemCount == 0 || itemCount > outputQueueCapacity) {
		return false;
	}

	size_t position = outputEnqueuePosition.load(std::memory_order_relaxed);

	for (;;) {
		// Every cell of the run must be free before the range can be taken.
		bool runIsFree = true;
		for (size_t index = 0; index < itemCount; ++index) {
			const size_t slot = position + index;
			const auto& cell = outputQueue[slot % outputQueueCapacity];
			const size_t sequence = cell.sequence.load(std::memory_order_acquire);
			if (static_cast<std::intptr_t>(sequence)
				- static_cast<std::intptr_t>(slot) != 0) {
				runIsFree = false;
				break;
			}
		}

		if (!runIsFree) {
			const size_t current =
				outputEnqueuePosition.load(std::memory_order_relaxed);
			if (current == position) {
				// Not a lost race: the queue really cannot hold the run.
				return false;
			}
			position = current;
			continue;
		}

		// One CAS takes the whole range. A competing producer either wins and
		// pushes us to a later position, or lands entirely after the run.
		if (outputEnqueuePosition.compare_exchange_weak(position,
			position + itemCount, std::memory_order_relaxed)) {
			firstPosition = position;
			return true;
		}
	}
}

bool Pfm2MidiDevice::queueNrpnBatch(int midiChannel, const NrpnItem* items,
	size_t itemCount) noexcept {
	if (items == nullptr || itemCount == 0
		|| midiChannel < 1 || midiChannel > 16) {
		++droppedOutputEvents;
		return false;
	}

	for (size_t index = 0; index < itemCount; ++index) {
		if (items[index].parameter > 0x3fff || items[index].value > 0x3fff) {
			++droppedOutputEvents;
			return false;
		}
	}

	size_t firstPosition = 0;
	if (!claimOutputQueueRun(itemCount, firstPosition)) {
		// Nothing was enqueued, so the caller can abort cleanly instead of
		// leaving a half transmitted patch on the wire.
		++droppedOutputEvents;
		return false;
	}

	const uint32_t generation = deviceGeneration.load(std::memory_order_acquire);

	for (size_t index = 0; index < itemCount; ++index) {
		const size_t slot = firstPosition + index;
		auto& cell = outputQueue[slot % outputQueueCapacity];
		cell.event.type = OutputEventType::nrpn;
		cell.event.midiChannel = static_cast<uint8_t>(midiChannel);
		cell.event.nrpnParameter = items[index].parameter;
		cell.event.nrpnValue = items[index].value;
		cell.event.deviceGeneration = generation;
		cell.sequence.store(slot + 1, std::memory_order_release);
	}

	return true;
}

void Pfm2MidiDevice::addListener(MidiInputCallback *listener) {
	listeners.add(listener);
}

void Pfm2MidiDevice::removeListener(MidiInputCallback *listener) {
	listeners.remove(listener);
}

void Pfm2MidiDevice::handleIncomingMidiMessage(MidiInput * source, const MidiMessage &midiMessage) {
	listeners.call(&MidiInputCallback::handleIncomingMidiMessage, source, midiMessage);
}

void Pfm2MidiDevice::handlePartialSysexMessage(MidiInput*, const uint8*, int, double) {

}
