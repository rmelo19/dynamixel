/*
 * dynamixel.h
 *
 *  Created on: 2016. 6. 9.
 *      Author: Rodrigo Marques
 */

#ifndef DYNAMIXELS_H
#define DYNAMIXELS_H

// switching between servo versions
#define XM430

#include <dynamixelsDefs.h>
#include <dynamixel_sdk.h>                                  // Uses Dynamixel SDK library
#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#endif

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"
#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque


#define DXL_MINIMUM_POSITION_VALUE      0                   // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4095                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

// GENERAL VARIABLES
#define ESC_ASCII_VALUE                 0x1b
#define MAXIMUM_NUMBER_DYNAMIXELS       10

// PRINTING INFO
#define BAUDRATE_INFO                   1
#define ID_ARRAY_INFO                   2
#define POSITION_INFO                   3
#define HEADER_INFO                     4



//#######################################################################################
// DYNAMIXEL CLASS
//#######################################################################################
class dynamixels
{
	public:
		dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
		~dynamixels();

    // dynamixel::PortHandler *portHandler;
    // dynamixel::PacketHandler *packetHandler;
    int ID_array[MAXIMUM_NUMBER_DYNAMIXELS];
    int32_t presentPosition[MAXIMUM_NUMBER_DYNAMIXELS];
    int baudrate = BAUDRATE;
    int qtdDyn = 0; // dynamixels quantity

    void enableTorqueALL(void);
    int searchDynamixels(int baudrate);
    void printInfo(int info);
    void setPosition(int index);
    int32_t dynamixels::readPosition(int index)
	
};


//#######################################################################################
// General functions
//#######################################################################################
int getch();
int kbhit(void);