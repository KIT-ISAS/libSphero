#include "libSphero.h"
#include "Thread.h"
#include "ResponseMessage.h"
#include <iostream>

using namespace LibSphero;


/*
 * Main.cpp
 *
 *  Created on: 19.04.2012
 *      Author: telepresence
 */

const char* SPHERO_ID = "00:06:66:44:29:56";
struct Listener : public IListener {
	void onPacketReceived(const Response::Message &message) {
		const uint8_t *payload = message.getPacketPointer() + message.getPayloadStart();
		const int16_t *values = (const int16_t*)payload;
		std::cout << values[0] << " " << values[1] << " " << values[2] << std::endl;
	}
};

int main(int argc, char **argv) {
	Robot robot;
	robot.setDebug(true);
	robot.connect(SPHERO_ID);
	robot.stop();

	robot.enableStabilizer(false);

	robot.setColor(0, 0, 0);

	while(true) {
		robot.setFrontLEDBrightness(255);
		robot.delay(200);
		robot.setFrontLEDBrightness(0);
		robot.delay(200);
	}

	robot.send(Macro::setDataStreaming(100, 1, Macro::MAGNETOMETER_RAW, 1000));

	//Listener listener;
	//robot.listen(listener);
}




