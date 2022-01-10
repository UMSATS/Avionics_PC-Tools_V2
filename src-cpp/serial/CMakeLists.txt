#
#   Copyright 2016 rm5248
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and

PROJECT(serial)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


INCLUDE(CheckIncludeFiles)
INCLUDE(GNUInstallDirs)
#
# CMake variables
#

#see if we are on Linux and have RS485
CHECK_INCLUDE_FILES(linux/serial.h HAVE_LINUX_SERIAL)

# Platform Defines, append to this to set various defines for the code
SET( PLATFORM_DEFINES )
#SET( install_targets )

#
# library version
#
#SET(CSERIAL_VERSION_MAJOR 0)
#SET(CSERIAL_VERSION_MINOR 2)
#SET(CSERIAL_VERSION_PATCH 0)
#SET(CSERIAL_VERSION_STRING ${CSERIAL_VERSION_MAJOR}.${CSERIAL_VERSION_MINOR}.${CSERIAL_VERSION_PATCH})

#ADD_DEFINITIONS( "-DCSERIAL_VERSION=${CSERIAL_VERSION_STRING}" )


#
# Sources for the library
#
SET( C_SERIAL_SOURCES c_serial.c)
SET( C_SERIAL_HEADERS c_serial.h simplelogger_defs.h)
SET( C_SERIAL_INCLUDE_DIRECTORIES serial)
#INCLUDE_DIRECTORIES( ${C_SERIAL_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_BINARY_DIR} )

CONFIGURE_FILE(config.h.in config.h)

#
# Library information
#
#ADD_LIBRARY( cserial STATIC ${C_SERIAL_SOURCES} )
#set_target_properties(cserial PROPERTIES VERSION ${CSERIAL_VERSION_STRING} SOVERSION ${CSERIAL_VERSION_MAJOR} )

#
# Library-wrapper information
#
ADD_LIBRARY ( serial STATIC c_serial.c serial.cpp )
#target_include_directories(serial PUBLIC serial)
#target_link_libraries(serial PUBLIC cserial)



