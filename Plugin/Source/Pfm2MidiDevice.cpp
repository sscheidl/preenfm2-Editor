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
	resetDevices();
	delete midiPropertyFile;
}

void Pfm2MidiDevice::resetDevices() {

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
	int messageSize, int midiChannelOverride) noexcept {
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
	cell->event.messageSize = static_cast<uint16_t>(messageSize);
	std::memcpy(cell->event.messageData.data(), messageData,
		static_cast<size_t>(messageSize));

	const uint8_t statusByte = cell->event.messageData[0];
	if (midiChannelOverride > 0 && statusByte >= 0x80 && statusByte < 0xf0) {
		cell->event.messageData[0] = static_cast<uint8_t>(
			(statusByte & 0xf0) | (midiChannelOverride - 1));
	}

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
		return;
	}

	if (event.type == OutputEventType::midiMessage) {
		pfm2MidiOutput->sendMessageNow(MidiMessage(event.messageData.data(),
			static_cast<int>(event.messageSize), 0.0));
		return;
	}

	MidiBuffer nrpnMessages;
	const int channel = event.midiChannel;
	const int parameter = event.nrpnParameter;
	const int value = event.nrpnValue;
	nrpnMessages.addEvent(MidiMessage::controllerEvent(channel, 99, parameter >> 7), 0);
	nrpnMessages.addEvent(MidiMessage::controllerEvent(channel, 98, parameter & 0x7f), 0);
	nrpnMessages.addEvent(MidiMessage::controllerEvent(channel, 6, value >> 7), 0);
	nrpnMessages.addEvent(MidiMessage::controllerEvent(channel, 38, value & 0x7f), 0);
	pfm2MidiOutput->sendBlockOfMessagesNow(nrpnMessages);
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




