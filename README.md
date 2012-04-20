# libSphero

This is a small library for C++ to control Orbotix Sphero devices from the desktop. It's basically a port of the code by 
Nicklas Gavelin (nicklas.gavelin@gmail.com) at https://github.com/nicklasgav/Sphero-Desktop-API.

## Example code

This code snippet connects to the robot and sets the LED color.

	#include <libSphero.h>
	#include <iostream>

	using namespace LibSphero;

	int main(int argc, char **argv) {
	    Robot robot;

	    if (!robot.connect("00:06:66:44:29:56")) { // replace with your address!
	        std::cout << "Failed to connect!" << std::endl;
	        return 0;
	    }	

	    while(true) {
	        robot.setLEDColor(255, 0, 0);
	        robot.delay(200);
	        robot.setLEDColor(0, 255, 0);
	        robot.delay(200);
	        robot.setLEDColor(0, 0, 255);
	        robot.delay(200);
	    }
	}

