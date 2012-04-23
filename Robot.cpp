
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

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "libSphero.h"

namespace LibSphero {

Robot::Robot() {
	socket = -1;
	seqNum = 0;
	state.heading = 0;
	state.velocity = 0;
	state.rotationRate = 0;
	state.red = 0;
	state.green = 0;
	state.blue = 0;
	state.brightness = 255;
	state.stop = true;
	debug = false;
}

Robot::~Robot() {
	// TODO Auto-generated destructor stub
}

bool Robot::connect(const std::string &_address) {
	socket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	address = _address;

	struct sockaddr_rc addr = { 0 };
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba(address.c_str(), &addr.rc_bdaddr);

	if (::connect(socket, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		if (debug) {
			std::cout << "Robot: Failed to connect to address '" << address << "'!"
					<< std::endl;
		}
		close(socket);
		socket = -1;
	} else {
		if (debug) {
			std::cout << "Robot: Connection to '" << address << "' succeeded!"
					<< std::endl;
		}

		send(Macro::abort());
		stop();
	}

	return isConnected();
}

void Robot::setDebug(bool b) {
	debug = b;
}

void Robot::disconnect() {
	if (isConnected()) {
		close(socket);
		socket = -1;
	}
}

void Robot::send(const Command::Message &message) {
	ByteArrayBuffer packet;
	message.packetize(packet, seqNum++);

	updateInternalValues(message);

	if (debug) {
		std::cout << ">> " << message.getCommand() << ": " << packet << std::endl;
	}

	size_t offset = 0;
	const uint8_t* bytes = &packet[0];

	while (offset != packet.size()) {
		int written = write(socket, bytes + offset, packet.size() - offset);
		if (written == -1) {
			std::cout << "Robot: Failed to write!" << std::endl;
			break;
		} else {
			offset += written;
		}
	}
}

void Robot::updateInternalValues(const Command::Message &message) {
	const ByteArrayBuffer &values = message.getPayload();

	switch (message.getCommand()) {
	case Command::MessageType::ROLL:
		state.velocity = values[0];
		state.heading = (values[1] << 8) + values[2];
		state.stop = values[3];
		break;
	case Command::MessageType::SPIN_LEFT:
		// TODO
		break;
	case Command::MessageType::SPIN_RIGHT:
		// TODO
		break;
	case Command::MessageType::RAW_MOTOR:
		// TODO
		break;
	case Command::MessageType::ROTATION_RATE:
		state.rotationRate = values[0];
		break;
	case Command::MessageType::RGB_LED_OUTPUT:
		state.red = values[0];
		state.green = values[1];
		state.blue = values[2];
		break;
	case Command::MessageType::FRONT_LED_OUTPUT:
		state.brightness = values[0];
		break;
	default:
		break;
	}
}

void Robot::listen(IListener &listener) {
	uint8_t data[100];

	while(true) {
		int read = ::read(socket, data, 100);
		if (read == -1) {
			if (isConnected()) {
				std::cout << "Robot: Failed to read!" << std::endl;
				disconnect();
			}
			return;
		}
		for (int k = 0; k < read; k++) {
			rxBuffer.push_back(data[k]);
		}

		while (Response::Message::containsValidPacket(rxBuffer)) {
			Response::Message message(rxBuffer);

			if (debug) {
				switch(message.getResponseType()) {
				case Response::Type::REGULAR:
					std::cout << "<< " << message.getResponseType() << "/"
							<< message.getResponseCode() << ": "
							<< message.getPacket() << std::endl;
					break;
				case Response::Type::INFORMATION:
					std::cout << "<< " << message.getResponseType() << "/"
							<< message.getInformationCode() << ": "
							<< message.getPacket() << std::endl;
					break;
				default:
					std::cout << "<< UNKNOWN: "
							<< message.getPacket() << std::endl;
					break;
				}
			}

			listener.onPacketReceived(message);

			size_t offset = message.getTotalLength();
			if (offset != rxBuffer.size()) {
				std::copy(rxBuffer.begin() + offset,
						rxBuffer.end(),
						rxBuffer.begin());
				rxBuffer.resize(rxBuffer.size() - offset);
			} else {
				rxBuffer.clear();
			}
		}
	}
}

void Robot::roll(int heading, uint8_t speed) {
	heading = ((heading % 360) + 360) % 360;
	send(Macro::roll(heading, speed, false));
}

void Robot::calibrateHeading(int heading) {
	roll(heading, 0);
	send(Macro::calibrate(heading));
}

void Robot::setHeading(int heading) {
	roll(heading, state.velocity);
}

void Robot::setSpeed(uint8_t speed) {
	roll(state.heading, speed);
}

void Robot::setLEDColor(uint8_t red, uint8_t green, uint8_t blue) {
	send(Macro::RGBLED(red, green, blue));
}

void Robot::setFrontLEDBrightness(uint8_t brightness) {
	send(Macro::setFrontLED(brightness));
}

void Robot::enableStabilizer(bool on) {
	send(Macro::enableStabilizer(on));
}

void Robot::stop() {
	send(Macro::roll(state.heading, 0, true));
}

void Robot::delay(unsigned int milliseconds) {
	if (milliseconds > 1000) {
		int seconds = milliseconds / 1000;
		sleep(seconds);
		milliseconds -= seconds * 1000;
	}
	usleep(milliseconds * 1000);
}


std::ostream &operator<<(std::ostream &os, const ByteArrayBuffer &packet) {
	const char* LETTERS = "0123456789ABCDEF";

	for (uint8_t c : packet) {
		os << LETTERS[c >> 4] << LETTERS[c & 0xf] << " ";
	}

	return os;
}

}
