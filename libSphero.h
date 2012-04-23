
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

#ifndef LIBSPHERO_H_
#define LIBSPHERO_H_

#include <ostream>
#include <string>
#include <vector>
#include <inttypes.h>

namespace LibSphero {

typedef std::vector<uint8_t> ByteArrayBuffer;

namespace Command {

enum class MessageType {
	PING,
	VERSIONING,
	SET_BLUETOOTH_NAME,
	GET_BLUETOOTH_INFO,
	GO_TO_SLEEP,
	JUMP_TO_BOOTLOADER,
	LEVEL_1_DIAGNOSTICS,
	JUMP_TO_MAIN,
	CALIBRATE,
	STABILIZATION,
	ROTATION_RATE,
	RGB_LED_OUTPUT,
	FRONT_LED_OUTPUT,
	ROLL,
	BOOST,
	RAW_MOTOR,
	GET_CONFIGURATION_BLOCK,
	RUN_MACRO,
	MACRO,
	SAVE_MACRO,
	ABORT_MACRO,
	SET_DATA_STREAMING,
	SPIN_LEFT,
	SPIN_RIGHT,
	CUSTOM_PING,
	INVALID,

};

std::ostream &operator<<(std::ostream &os, MessageType type);

/** Command message class */
class Message {
private:
	MessageType command;
	ByteArrayBuffer payload;

public:
	/** Creates a message with the given command and no payload */
	Message(MessageType command);

	/** Creates a message with the given command and the given payload */
	Message(MessageType command, const ByteArrayBuffer &payload);

	~Message();

	/** Returns the command */
	MessageType getCommand() const {
		return command;
	}

	/** Returns the payload */
	ByteArrayBuffer &getPayload() {
		return payload;
	}

	/** Returns the payload */
	const ByteArrayBuffer &getPayload() const {
		return payload;
	}

	/** Returns a pointer to the payload */
	const uint8_t* getPayloadPointer() const {
		return &payload[0];
	}

	/** Converts the message to a packet */
	void packetize(ByteArrayBuffer &buffer, int seqNum) const;
};

}

namespace Response {

typedef std::vector<uint8_t> ByteArrayBuffer;

enum class Type {
	REGULAR, INFORMATION, UNKNOWN
};

enum class Code {
	OK,
	ERROR_GENERAL,
	ERROR_CHECKSUM,
	ERROR_FRAGMENT,
	ERROR_BAD_COMMAND,
	ERROR_UNSUPPORTED,
	ERROR_BAD_MESSAGE,
	ERROR_PARAMETER,
	ERROR_EXECUTE,
	ERROR_MAIN_APP_CORRUPT,
	ERROR_TIME_OUT,
	ERROR_UNKNOWN,
	UNKNOWN_RESPONSE,
	INVALID
};

enum class InformationCode {
	EMIT, DATA, INVALID
};

std::ostream &operator<<(std::ostream &os, Type type);
std::ostream &operator<<(std::ostream &os, Code code);
std::ostream &operator<<(std::ostream &os, InformationCode code);

/** Response message class */
struct Message {
private:
	ByteArrayBuffer packet;
	Code code;
	Type type;

public:
	/** Returns whether the data buffer contains at least one valid packet */
	static bool containsValidPacket(const ByteArrayBuffer &data);

	/** Creates a message */
	Message();

	/** Creates a message */
	Message(const ByteArrayBuffer &source);

	virtual ~Message();

	/** Returns the response type */
	Type getResponseType() const {
		return type;
	}

	/** Returns the response code */
	Code getResponseCode() const {
		return code;
	}

	/** Returns the message packet */
	const ByteArrayBuffer &getPacket() const {
		return packet;
	}

	/** Returns a pointer to the packet data */
	const uint8_t* getPacketPointer() const {
		return &packet[0];
	}

	/** For information responses, returns the information code */
	InformationCode getInformationCode() const;

	/** Returns whether the packet is corrupt */
	bool isCorrupt() const;

	/** Returns the size of the header */
	size_t getHeaderLength() const;

	/** Returns where, in the packet, the payload begins */
	size_t getPayloadStart() const;

	/** Returns the size of the payload */
	size_t getPayloadLength() const;

	/** Returns the total size of the packet */
	size_t getTotalLength() const;

	/** Returns the packet sequence number */
	int getSequenceNumber() const;

	/** Returns the checksum contained in the packet */
	uint8_t getClaimedChecksum() const;

	/** Returns the checksum calculated from the packet */
	uint8_t getActualChecksum() const;

};

}

struct Macro {
	enum BlockSpecifier {
		FACTORY = 0, USER = 1
	};

	enum MotorMode {
		FORWARD = 1, REVERSE = 2
	};

	enum Flag {
		MacroFlagNone = 0,
		MacroFlagMotorControl = 1,
		MacroFlagExclusiveDrive = 2,
		MacroFlagUseVersion3 = 4,
		MacroFlagInhibitIfConnected = 8,
		MacroFlagEndMarker = 16,
		MacroFlagStealth = 32,
		MacroFlagUnkillable = 64,
		MacroFlagExtendedFlags = -128,
		MACRO_STREAMING_DESTINATION = -2,
	};

	/** Flags to determine sensor data */
	enum StreamingMasks {
		OFF = 0,

		MOTOR_BACK_EMF_LEFT_FILTERED = 32,
		MOTOR_BACK_EMF_LEFT_RAW = MOTOR_BACK_EMF_LEFT_FILTERED << 16,
		MOTOR_BACK_EMF_RIGHT_FILTERED = 64,
		MOTOR_BACK_EMF_RIGHT_RAW = MOTOR_BACK_EMF_RIGHT_FILTERED << 16,
		MOTOR_BACK_EMF_FILTERED = MOTOR_BACK_EMF_LEFT_FILTERED
				| MOTOR_BACK_EMF_RIGHT_FILTERED,
		MOTOR_BACK_EMF_RAW = MOTOR_BACK_EMF_LEFT_RAW
				| MOTOR_BACK_EMF_RIGHT_RAW,

		MAGNETOMETER_X_FILTERED = 512,
		MAGNETOMETER_X_RAW = MAGNETOMETER_X_FILTERED << 16,
		MAGNETOMETER_Y_FILTERED = 256,
		MAGNETOMETER_Y_RAW = MAGNETOMETER_Y_FILTERED << 16,
		MAGNETOMETER_Z_FILTERED = 128,
		MAGNETOMETER_Z_RAW = MAGNETOMETER_Z_FILTERED << 16,
		MAGNETOMETER_FILTERED = MAGNETOMETER_X_FILTERED
				| MAGNETOMETER_Y_FILTERED
				| MAGNETOMETER_Z_FILTERED,
		MAGNETOMETER_RAW = MAGNETOMETER_X_RAW
				| MAGNETOMETER_Y_RAW
				| MAGNETOMETER_Z_RAW,

		ACCELEROMETER_X_FILTERED = 32768,
		ACCELEROMETER_X_RAW = ACCELEROMETER_X_FILTERED << 16,
		ACCELEROMETER_Y_FILTERED = 16384,
		ACCELEROMETER_Y_RAW = ACCELEROMETER_Y_FILTERED << 16,
		ACCELEROMETER_Z_FILTERED = 8192,
		ACCELEROMETER_Z_RAW = ACCELEROMETER_Z_FILTERED << 16,
		ACCELEROMETER_FILTERED = ACCELEROMETER_X_FILTERED
				| ACCELEROMETER_Y_FILTERED
				| ACCELEROMETER_Z_FILTERED,
		ACCELEROMETER_RAW = ACCELEROMETER_X_RAW
				| ACCELEROMETER_Y_RAW
				| ACCELEROMETER_Z_RAW,

		GYRO_X_FILTERED = 4096,
		GYRO_X_RAW = GYRO_X_FILTERED << 16,
		GYRO_Y_FILTERED = 2048,
		GYRO_Y_RAW = GYRO_Y_FILTERED << 16,
		GYRO_Z_FILTERED = 1024,
		GYRO_Z_RAW = GYRO_Z_FILTERED << 16,
		GYRO_FILTERED = GYRO_X_FILTERED | GYRO_Y_FILTERED | GYRO_Z_FILTERED,
		GYRO_RAW = GYRO_X_RAW | GYRO_Y_RAW | GYRO_Z_RAW,

		IMU_YAW_FILTERED = 65536,
		IMU_ROLL_FILTERED = 131072,
		IMU_PITCH_FILTERED = 262144,
		IMU_FILTERED = IMU_YAW_FILTERED | IMU_ROLL_FILTERED | IMU_PITCH_FILTERED,
	};

	/** Aborts a running macro */
	static Command::Message abort();

	/** Calibrates using the given heading */
	static Command::Message calibrate(int heading);

	/** Sets the front LED brightness (0-255) */
	static Command::Message setFrontLED(uint8_t brightness);

	/** Gets the Bluetooth info  */
	static Command::Message getBluetoothInfo();

	static Command::Message getConfigurationBlock(BlockSpecifier block); //?

	/** Sets the robot in bootloader application. This closes the connection. */
	static Command::Message jumpToBootloader();

	/** Jumps to the application. This closes the connection. */
	static Command::Message jumpToMain();

	static Command::Message level1Diagnostics(); //?

	/** Sets the RGB LED color */
	static Command::Message RGBLED(uint8_t red,
			uint8_t green,
			uint8_t blue);

	/** Sets the direction and speed for each individual internal motor */
	static Command::Message rawMotor(MotorMode leftMode,
			uint8_t leftSpeed,
			MotorMode rightMode,
			uint8_t rightSpeed);

	/** Moves the robot in a given heading angle (in degress) and a given speed (0-255) */
	static Command::Message roll(int heading,
			uint8_t velocity,
			bool stop);

	static Command::Message rotationRate(uint8_t rate); //?

	/** Runs the given macro (untested) */
	static Command::Message runMacro(uint8_t macroId);

	/** Tells the robot to stream back sensor data
	 * @param mDivisor Divisor to divide the default sampling rate of 400 Hz
	 * @param mPacketFrames Number of frames per packet
	 * @param mSensorMask Which sensor values to send (bit mask from StreamingMasks)
	 * @param mPacketCount Number of packets to receive
	 **/
	static Command::Message setDataStreaming(uint16_t mDivisor,
			uint16_t mPacketFrames,
			int mSensorMask,
			uint8_t mPacketCount);

	/** Sets the robot name */
	static Command::Message setRobotName(const std::string &name);

	/** Sleeps for the given number of seconds. This closes the connection */
	static Command::Message sleep(int time, int macroId);

	/** Tells the robot to spin left */
	static Command::Message spinLeft(uint8_t speed);

	/** Tells the robot to spin right */
	static Command::Message spinRight(uint8_t speed);

	/** Enables or disables the stabilizer */
	static Command::Message enableStabilizer(bool on);

	/** Requests version information from the robot */
	static Command::Message version();

};

struct RobotState {
	int heading;
	uint8_t velocity;
	uint8_t rotationRate;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t brightness;
	bool stop;
};

struct IListener {
	virtual ~IListener() {}
	virtual void onPacketReceived(const Response::Message &message) = 0;
};

class Robot {
private:
	ByteArrayBuffer rxBuffer;
	std::string address;
	RobotState state;
	int socket;
	unsigned int seqNum;
	bool debug;

	void updateInternalValues(const Command::Message &command);

public:
	Robot();
	virtual ~Robot();

	/** Connects to the given Bluetooth address. The address should be
	 * in the form '00:06:66:XX:XX:XX' */
	bool connect(const std::string &address);

	/** Closes the connection */
	void disconnect();

	/** Sends a direct command to the robot */
	void send(const Command::Message &message);

	/** Listens for data coming from the robot, sending the received data to the listener.
	 * This function will block indefinitely. */
	void listen(IListener &listener);

	/** Sets whether incoming and outgoing data should be printed */
	void setDebug(bool b);

	/** Sends a roll command to the given heading (in degrees) and speed (0-255) */
	void roll(int heading, uint8_t speed);

	/** Sets the speed (0-255) while maintaining the heading */
	void setSpeed(uint8_t speed);

	/** Sets the heading (in degrees) while maintaining the speed */
	void setHeading(int heading);

	/** Stops the motors */
	void stop();

	/** Calibrates the heading */
	void calibrateHeading(int heading);

	/** Sets the LED RGB color */
	void setLEDColor(uint8_t red, uint8_t green, uint8_t blue);

	/** Sets the brightness of the front LED (0-255) */
	void setFrontLEDBrightness(uint8_t brightness);

	/** Enables or disables the stabilizer */
	void enableStabilizer(bool on);

	/** Convenience function, waits the given amount of milliseconds.
	 * This function is not related to the SLEEP macro. */
	void delay(unsigned int milliseconds);

	/** Returns whether the socket is connected */
	bool isConnected() const {
		return socket != -1;
	}

	/** Returns information about the state stored from sent commands */
	const RobotState &getState() const {
		return state;
	}

	/** Returns the last heading sent */
	int getHeading() const {
		return state.heading;
	}

	/** Returns the last velocity sent */
	uint8_t getVelocity() const {
		return state.velocity;
	}

	/** Returns the last red color sent */
	uint8_t getLEDRed() const {
		return state.red;
	}

	/** Returns the last green color sent */
	uint8_t getLEDGreen() const {
		return state.green;
	}

	/** Returns the last blue color sent */
	uint8_t getLEDBlue() const {
		return state.blue;
	}

	/** Returns the last front LED brightness sent */
	uint8_t getFrontLEDBrightness() const {
		return state.brightness;
	}

	/** Returns whether a stop command has been sent */
	bool isStopped() const {
		return state.stop;
	}

	/** Returns the address of the Bluetooth device */
	const std::string &getAddress() const {
		return address;
	}

};

std::ostream &operator<<(std::ostream &os, const ByteArrayBuffer &packet);

}

#endif /* LIBSPHERO_H_ */
