
#######################################
# LibSphero
#######################################

#######################################
# Project
#######################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibSphero)

SET(CMAKE_BUILD_TYPE Release)

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x")

ADD_LIBRARY(
	Sphero
	SHARED
	CommandMessage.cpp
	ResponseMessage.cpp
	Robot.cpp
	Macro.cpp
)

TARGET_LINK_LIBRARIES(
	Sphero
	bluetooth
)

INSTALL_TARGETS(/lib Sphero)
INSTALL_FILES(/include libSphero.h)

