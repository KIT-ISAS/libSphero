
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
#include <string.h>

namespace LibSphero {

Command::Message Macro::abort() {
	return {Command::MessageType::ABORT_MACRO};
}

Command::Message Macro::calibrate(int heading) {
	int corrHeading = ((unsigned int) heading) % 360;
	return {
		Command::MessageType::CALIBRATE,
		{
			(uint8_t)((int)corrHeading >> 8),
			(uint8_t)(int)corrHeading
		}
	};
}

Command::Message Macro::setFrontLED(uint8_t brightness) {
	return {
		Command::MessageType::FRONT_LED_OUTPUT,
		{
			(uint8_t)(brightness),
		}
	};
}

Command::Message Macro::getBluetoothInfo() {
	return {Command::MessageType::GET_BLUETOOTH_INFO};
}

Command::Message Macro::getConfigurationBlock(BlockSpecifier block) {
	return {
		Command::MessageType::GET_CONFIGURATION_BLOCK,
		{
			(uint8_t)block,
		}
	};
}

Command::Message Macro::jumpToBootloader() {
	return {Command::MessageType::JUMP_TO_BOOTLOADER};
}

Command::Message Macro::jumpToMain() {
	return {Command::MessageType::JUMP_TO_MAIN};
}

Command::Message Macro::level1Diagnostics() {
	return {Command::MessageType::LEVEL_1_DIAGNOSTICS};
}

Command::Message Macro::RGBLED(uint8_t red, uint8_t green,
		uint8_t blue) {
	return {
		Command::MessageType::RGB_LED_OUTPUT,
		{
			(uint8_t)red,
			(uint8_t)green,
			(uint8_t)blue
		}
	};
}

Command::Message Macro::rawMotor(MotorMode leftMode, uint8_t leftSpeed,
		MotorMode rightMode, uint8_t rightSpeed) {
	return {
		Command::MessageType::RAW_MOTOR,
		{
			(uint8_t)leftMode,
			(uint8_t)leftSpeed,
			(uint8_t)rightMode,
			(uint8_t)rightSpeed,
		}
	};
}

Command::Message Macro::roll(int heading, uint8_t velocity, bool stop) {
	return {
		Command::MessageType::ROLL,
		{
			(uint8_t)velocity,
			(uint8_t)(heading >> 8),
			(uint8_t)heading,
			(uint8_t)(stop ? 0 : 1)
		}
	};
}

Command::Message Macro::rotationRate(uint8_t rate) {
	return {
		Command::MessageType::ROTATION_RATE,
		{
			(uint8_t)rate
		}
	};
}

Command::Message Macro::runMacro(uint8_t macroId) {
	return {
		Command::MessageType::RUN_MACRO,
		{
			(uint8_t)macroId
		}
	};
}

Command::Message Macro::setDataStreaming(uint16_t mDivisor, uint16_t mPacketFrames,
		int mSensorMask, uint8_t mPacketCount) {
	return {
		Command::MessageType::SET_DATA_STREAMING,
		{
			(uint8_t)(mDivisor >> 8),
			(uint8_t)(mDivisor >> 0),
			(uint8_t)(mPacketFrames >> 8),
			(uint8_t)(mPacketFrames >> 0),
			(uint8_t)(mSensorMask >> 24),
			(uint8_t)(mSensorMask >> 16),
			(uint8_t)(mSensorMask >> 8),
			(uint8_t)(mSensorMask >> 0),
			(uint8_t)mPacketCount
		}
	};
}

Command::Message Macro::setRobotName(const std::string &name) {
	const size_t BUFFER_SIZE = 48;

	Command::Message message(Command::MessageType::SET_BLUETOOTH_NAME);
	ByteArrayBuffer &buffer = message.getPayload();

	buffer.resize(BUFFER_SIZE);
	memcpy(&buffer[0], name.c_str(), std::min(name.size(), BUFFER_SIZE));

	for (size_t i = name.size(); i < BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}

	return message;
}

Command::Message Macro::sleep(int time, int macroId) {
	return {
		Command::MessageType::GO_TO_SLEEP,
		{
			(uint8_t)(time >> 8),
			(uint8_t)time,
			(uint8_t)macroId
		}
	};
}

Command::Message Macro::spinLeft(uint8_t speed) {
	return {
		Command::MessageType::SPIN_LEFT,
		{
			(uint8_t)FORWARD,
			(uint8_t)speed,
			(uint8_t)REVERSE,
			(uint8_t)speed
		}
	};
}

Command::Message Macro::spinRight(uint8_t speed) {
	return {
		Command::MessageType::SPIN_RIGHT,
		{
			(uint8_t)FORWARD,
			(uint8_t)speed,
			(uint8_t)REVERSE,
			(uint8_t)speed
		}
	};
}

Command::Message Macro::enableStabilizer(bool on) {
	return {
		Command::MessageType::STABILIZATION,
		{
			(uint8_t)(on ? 1 : 0),
		}
	};
}

Command::Message Macro::version() {
	return {Command::MessageType::VERSIONING};
}

}
