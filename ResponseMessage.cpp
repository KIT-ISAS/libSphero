
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

namespace Response {

//static const size_t	INDEX_START_1 = 0;
//static const size_t	INDEX_START_2 = 1;
static const size_t RESPONSE_CODE_INDEX = 2;

static const size_t SEQUENCE_NUMBER_INDEX = 3;
static const size_t PAYLOAD_LENGTH_INDEX = 4;
static const size_t RESPONSE_HEADER_LENGTH = 5;

static const size_t INFORMATION_RESPONSE_TYPE_INDEX = 2;
static const size_t	INFORMATION_RESPONSE_CODE_INDEX = 3;

static Response::Code findResponseCode(uint8_t a, Response::Type type) {
	if (type == Type::REGULAR) {
		switch (a) {
		case 0:
			return Code::OK;
		case 1:
			return Code::ERROR_GENERAL;
		case 2:
			return Code::ERROR_CHECKSUM;
		case 3:
			return Code::ERROR_FRAGMENT;
		case 4:
			return Code::ERROR_BAD_COMMAND;
		case 5:
			return Code::ERROR_UNSUPPORTED;
		case 6:
			return Code::ERROR_BAD_MESSAGE;
		case 7:
			return Code::ERROR_PARAMETER;
		case 8:
			return Code::ERROR_EXECUTE;
		case 52:
			return Code::ERROR_MAIN_APP_CORRUPT;
		case 255:
			return Code::ERROR_TIME_OUT;
		case 53:
			return Code::ERROR_UNKNOWN;
		case 254:
			return Code::UNKNOWN_RESPONSE;
		default:
			return Code::INVALID;
		}
	} else if (type == Type::INFORMATION) {
		switch (a) {
		case 0:
			return Code::OK;
		default:
			return Code::INVALID;
		}
	} else {
		return Code::INVALID;
	}
}

static Response::Type findResponseType(uint8_t a, uint8_t b) {
	if (a == 255 && b == 255) {
		return Type::REGULAR;
	} else if (a == 255 && b == 254) {
		return Type::INFORMATION;
	} else {
		return Type::UNKNOWN;
	}
}

static Response::InformationCode findInformationResponseCode(uint8_t c) {
	if (c == 6) {
		return InformationCode::EMIT;
	} else if (c == 3) {
		return InformationCode::DATA;
	} else {
		return InformationCode::INVALID;
	}
}

Message::Message() {
	code = Code::INVALID;
	type = Type::UNKNOWN;
}

Message::Message(const ByteArrayBuffer &source) {
	type = findResponseType(source[0], source[1]);

	if (type == Type::UNKNOWN) {
		code = Code::ERROR_BAD_MESSAGE;
		packet.push_back(source[0]);
		packet.push_back(source[1]);
	} else {
		size_t responseIndex = (type == Type::INFORMATION) ?
				INFORMATION_RESPONSE_CODE_INDEX : RESPONSE_CODE_INDEX;
		code = findResponseCode(source[responseIndex], type);

		size_t totalLength = RESPONSE_HEADER_LENGTH + source[PAYLOAD_LENGTH_INDEX];
		packet.resize(totalLength);
		std::copy(source.begin(),
				source.begin() + totalLength,
				packet.begin());
	}
}

Message::~Message() {

}

bool Message::isCorrupt() const {
	return getClaimedChecksum() != getActualChecksum();
}

uint8_t Message::getClaimedChecksum() const {
	if (packet.empty()) {
		return 0;
	} else {
		return packet.back();
	}
}

size_t Message::getHeaderLength() const {
	return RESPONSE_HEADER_LENGTH;
}

size_t Message::getPayloadStart() const {
	return getHeaderLength();
}

size_t Message::getPayloadLength() const {
	if (packet.size() <= PAYLOAD_LENGTH_INDEX) {
		return 0;
	} else {
		return packet[PAYLOAD_LENGTH_INDEX];
	}
}

size_t Message::getTotalLength() const {
	return getHeaderLength() + getPayloadLength();
}

int Message::getSequenceNumber() const {
	if (type != Type::REGULAR || packet.size() < SEQUENCE_NUMBER_INDEX) {
		return -1;
	} else {
		return packet[SEQUENCE_NUMBER_INDEX];
	}
}

uint8_t Message::getActualChecksum() const {
	if (packet.empty()) {
		return 0;
	}

	int checksum = 0;
	for (size_t i = 2; i < packet.size() - 1; i++) {
		checksum += packet[i];
	}
	checksum ^= 0xFFFFFFFF;

	return checksum;
}

InformationCode Message::getInformationCode() const {
	if (type != Type::INFORMATION) {
		return InformationCode::INVALID;
	} else {
		return findInformationResponseCode(
				packet[INFORMATION_RESPONSE_TYPE_INDEX]);
	}
}

bool Message::containsValidPacket(const ByteArrayBuffer &data) {
	if (data.size() < RESPONSE_HEADER_LENGTH) {
		return false;
	}

	size_t dataLength = data[PAYLOAD_LENGTH_INDEX];
	if (data.size() < RESPONSE_HEADER_LENGTH + dataLength) {
		return false;
	}

	return true;
}

std::ostream &operator<<(std::ostream &os, Type type) {
	switch (type) {
	case Type::REGULAR:
		os << "REGULAR";
		break;
	case Type::INFORMATION:
		os << "INFORMATION";
		break;
	default:
		os << "UNKNOWN";
		break;
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, Code code) {
	switch (code) {
	case Code::OK:
		os << "OK";
		break;
	case Code::ERROR_GENERAL:
		os << "ERROR_GENERAL";
		break;
	case Code::ERROR_CHECKSUM:
		os << "ERROR_CHECKSUM";
		break;
	case Code::ERROR_FRAGMENT:
		os << "ERROR_FRAGMENT";
		break;
	case Code::ERROR_BAD_COMMAND:
		os << "ERROR_BAD_COMMAND";
		break;
	case Code::ERROR_UNSUPPORTED:
		os << "ERROR_UNSUPPORTED";
		break;
	case Code::ERROR_BAD_MESSAGE:
		os << "ERROR_BAD_MESSAGE";
		break;
	case Code::ERROR_PARAMETER:
		os << "ERROR_PARAMETER";
		break;
	case Code::ERROR_EXECUTE:
		os << "ERROR_EXECUTE";
		break;
	case Code::ERROR_MAIN_APP_CORRUPT:
		os << "ERROR_MAIN_APP_CORRUPT";
		break;
	case Code::ERROR_TIME_OUT:
		os << "ERROR_TIME_OUT";
		break;
	case Code::ERROR_UNKNOWN:
		os << "ERROR_UNKNOWN";
		break;
	case Code::UNKNOWN_RESPONSE:
		os << "UNKNOWN_RESPONSE";
		break;
	default:
		os << "INVALID";
		break;
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, InformationCode code) {
	switch (code) {
	case InformationCode::EMIT:
		os << "EMIT";
		break;
	case InformationCode::DATA:
		os << "DATA";
		break;
	default:
		os << "INVALID";
		break;
	}
	return os;
}

}

}
