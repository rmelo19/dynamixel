/*
 * dynamixel.h
 *
 *  Created on: 2016. 6. 9.
 *      Author: Rodrigo Marques
 */

#ifndef DYNAMIXELS_H
#define DYNAMIXELS_H

// switching between servo versions
// #define XM430
// #define MX28

// Protocol version
#define PROTOCOL_VERSION                1                 // See which protocol version is used in the Dynamixel

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


//#######################################################################################
// DYNAMIXEL CLASS
//#######################################################################################
class dynamixels
{
	public:
		// dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
        dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate = BAUDRATE);
		~dynamixels();

        dynamixel::PortHandler *dxl_portHandler;
        dynamixel::PacketHandler *dxl_packetHandler;



        uint8_t ID_array[MAXIMUM_NUMBER_DYNAMIXELS];
        uint8_t firmware[MAXIMUM_NUMBER_DYNAMIXELS];
        uint8_t operatingMode[MAXIMUM_NUMBER_DYNAMIXELS];
       
        #if PROTOCOL_VERSION == 2
            uint32_t presentPosition[MAXIMUM_NUMBER_DYNAMIXELS];
            uint8_t  protocol[MAXIMUM_NUMBER_DYNAMIXELS];
            uint32_t minPositionLimit[MAXIMUM_NUMBER_DYNAMIXELS];
            uint32_t maxPositionLimit[MAXIMUM_NUMBER_DYNAMIXELS];
            uint32_t readPosition(int index);

        #else
            uint16_t presentPosition[MAXIMUM_NUMBER_DYNAMIXELS];
            uint16_t readPosition(int index);
        #endif

        int qtdDyn; // dynamixels quantity

        void enableTorqueALL(void);
        void disableTorqueALL(void);
        void changeMode(uint8_t desired_mode);
        int searchDynamixels(int desired_baudrate = BAUDRATE);
        void printInfo(int info);
        void setPosition(int index, uint32_t desired_position);
        void setPositionALL(uint32_t desired_position);
        
    private:
        int baudrate;
	
};


//#######################################################################################
// General functions
//#######################################################################################
int getch();
int kbhit(void);