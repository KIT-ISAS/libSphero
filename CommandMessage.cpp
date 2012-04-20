
/*
Copyright (C) 2012 Antonio Zea, akzeac@gmail.com, Karlsruhe Institute of Technology
This code borrows heavily from code by Nicklas Gavelin, nicklas.gavelin@gmail.com,
Luleå University of Technology

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "libSphero.h"

namespace LibSphero {

namespace Command {

/* Static indices */
static const uint8_t COMMAND_PREFIX = -1;
static const int CHECKSUM_LENGTH = 1;
static const int INDEX_START_1 = 0;
static const int INDEX_START_2 = 1;
static const int INDEX_DEVICE_ID = 2;
static const int INDEX_COMMAND = 3;
static const int INDEX_COMMAND_SEQUENCE_NO = 4;
static const int INDEX_COMMAND_DATA_LENGTH = 5;
static const int COMMAND_HEADER_LENGTH = 6;


Message::Message(MessageType _command) :
	command(_command) {
}

Message::Message(MessageType _command,
		const ByteArrayBuffer &_packetData) :
	command(_command),
	payload(_packetData) {
}

Message::~Message() {
}

static uint8_t getCommandLength(int dataLength) {
	return (uint8_t)(dataLength + 1);
}

static uint8_t getCommandId(MessageType command) {
	switch(command) {
	case MessageType::PING:
		return 0;
	case MessageType::VERSIONING:
		return 2;
	case MessageType::SET_BLUETOOTH_NAME:
		return 16;
	case MessageType::GET_BLUETOOTH_INFO:
		return 17;
	case MessageType::GO_TO_SLEEP:
		return 34;
	case MessageType::JUMP_TO_BOOTLOADER:
		return 48;
	case MessageType::LEVEL_1_DIAGNOSTICS:
		return 64;
	case MessageType::JUMP_TO_MAIN:
		return 4;
	case MessageType::CALIBRATE:
		return 1;
	case MessageType::STABILIZATION:
		return 2;
	case MessageType::ROTATION_RATE:
		return 3;
	case MessageType::RGB_LED_OUTPUT:
		return 32;
	case MessageType::FRONT_LED_OUTPUT:
		return 33;
	case MessageType::ROLL:
		return 48;
	case MessageType::BOOST:
		return 49;
	case MessageType::RAW_MOTOR:
		return 51;
	case MessageType::GET_CONFIGURATION_BLOCK:
		return 64;
	case MessageType::RUN_MACRO:
		return 80;
	case MessageType::MACRO:
		return 81;
	case MessageType::SAVE_MACRO:
		return 82;
	case MessageType::ABORT_MACRO:
		return 85;
	case MessageType::SET_DATA_STREAMING:
		return 17;
	case MessageType::SPIN_LEFT:
		return 51; // same as RAW_MOTOR
	case MessageType::SPIN_RIGHT:
		return 51; // same as RAW_MOTOR
	case MessageType::CUSTOM_PING:
		return 33; // same as FRONT_LED_OUTPUT
	default:
		return -1;
	}
}

static uint8_t getDeviceId(MessageType command) {
	switch(command) {
	case MessageType::PING:
	case MessageType::VERSIONING:
	case MessageType::SET_BLUETOOTH_NAME:
	case MessageType::GET_BLUETOOTH_INFO:
	case MessageType::GO_TO_SLEEP:
	case MessageType::JUMP_TO_BOOTLOADER:
	case MessageType::LEVEL_1_DIAGNOSTICS:
		return 0;
	case MessageType::JUMP_TO_MAIN:
		return 1;
	case MessageType::CALIBRATE:
	case MessageType::STABILIZATION:
	case MessageType::ROTATION_RATE:
	case MessageType::RGB_LED_OUTPUT:
	case MessageType::FRONT_LED_OUTPUT:
	case MessageType::ROLL:
	case MessageType::BOOST:
	case MessageType::RAW_MOTOR:
	case MessageType::GET_CONFIGURATION_BLOCK:
	case MessageType::RUN_MACRO:
	case MessageType::MACRO:
	case MessageType::SAVE_MACRO:
	case MessageType::ABORT_MACRO:
	case MessageType::SET_DATA_STREAMING:
	case MessageType::SPIN_LEFT:
	case MessageType::SPIN_RIGHT:
	case MessageType::CUSTOM_PING:
		return 2;
	default:
		return -1;
	}
}

void Message::packetize(ByteArrayBuffer &buffer, int seqNum) const {
    const ByteArrayBuffer &data = getPayload();

    int data_length = data.size();
    int packet_length = data_length + COMMAND_HEADER_LENGTH + CHECKSUM_LENGTH;

    buffer.resize(packet_length);

    uint8_t checksum = 0;

    buffer[INDEX_START_1] = COMMAND_PREFIX;
    buffer[INDEX_START_2] = COMMAND_PREFIX;

    uint8_t device_id = getDeviceId(command);
    checksum = (uint8_t) (checksum + device_id);
    buffer[INDEX_DEVICE_ID] = device_id;

    uint8_t cmd = getCommandId(command);
    checksum = (uint8_t) (checksum + cmd);
    buffer[INDEX_COMMAND] = cmd;

    int sequenceNumber = seqNum;
    checksum = (uint8_t) (checksum + sequenceNumber);
    buffer[ INDEX_COMMAND_SEQUENCE_NO] = (uint8_t) (sequenceNumber);

    uint8_t response_length = getCommandLength( data_length );
    checksum = (uint8_t) (checksum + response_length);
    buffer[ INDEX_COMMAND_DATA_LENGTH] = response_length;

    // Calculate the checksum for the data (also add the data to the array)
	for (int i = 0; i < data_length; i++) {
		buffer[(i + COMMAND_HEADER_LENGTH)] = data[i];
		checksum = (uint8_t) (checksum + data[i]);
	}

	buffer[(packet_length - CHECKSUM_LENGTH)] = (uint8_t) (checksum ^ 0xFFFFFFFF);
}


std::ostream &operator<<(std::ostream &os, MessageType type) {
	switch(type) {
	case MessageType::PING:
		os << "PING";
		break;
	case MessageType::VERSIONING:
		os << "VERSIONING";
		break;
	case MessageType::SET_BLUETOOTH_NAME:
		os << "SET_BLUETOOTH_NAME";
		break;
	case MessageType::GET_BLUETOOTH_INFO:
		os << "GET_BLUETOOTH_INFO";
		break;
	case MessageType::GO_TO_SLEEP:
		os << "GO_TO_SLEEP";
		break;
	case MessageType::JUMP_TO_BOOTLOADER:
		os << "JUMP_TO_BOOTLOADER";
		break;
	case MessageType::LEVEL_1_DIAGNOSTICS:
		os << "LEVEL_1_DIAGNOSTICS";
		break;
	case MessageType::JUMP_TO_MAIN:
		os << "JUMP_TO_MAIN";
		break;
	case MessageType::CALIBRATE:
		os << "CALIBRATE";
		break;
	case MessageType::STABILIZATION:
		os << "STABILIZATION";
		break;
	case MessageType::ROTATION_RATE:
		os << "ROTATION_RATE";
		break;
	case MessageType::RGB_LED_OUTPUT:
		os << "RGB_LED_OUTPUT";
		break;
	case MessageType::FRONT_LED_OUTPUT:
		os << "FRONT_LED_OUTPUT";
		break;
	case MessageType::ROLL:
		os << "ROLL";
		break;
	case MessageType::BOOST:
		os << "BOOST";
		break;
	case MessageType::RAW_MOTOR:
		os << "RAW_MOTOR";
		break;
	case MessageType::GET_CONFIGURATION_BLOCK:
		os << "GET_CONFIGURATION_BLOCK";
		break;
	case MessageType::RUN_MACRO:
		os << "RUN_MACRO";
		break;
	case MessageType::MACRO:
		os << "MACRO";
		break;
	case MessageType::SAVE_MACRO:
		os << "SAVE_MACRO";
		break;
	case MessageType::ABORT_MACRO:
		os << "ABORT_MACRO";
		break;
	case MessageType::SET_DATA_STREAMING:
		os << "SET_DATA_STREAMING";
		break;
	case MessageType::SPIN_LEFT:
		os << "SPIN_LEFT";
		break;
	case MessageType::SPIN_RIGHT:
		os << "SPIN_RIGHT";
		break;
	case MessageType::CUSTOM_PING:
		os << "CUSTOM_PING";
		break;
	default:
		break;
	}
	return os;
}

}

}
