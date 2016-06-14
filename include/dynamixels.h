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
		dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
        dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate);
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
            uint32_t readPosition(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int index);

        #else
            uint16_t presentPosition[MAXIMUM_NUMBER_DYNAMIXELS];
            uint16_t readPosition(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int index);
        #endif

        int qtdDyn; // dynamixels quantity

        void enableTorqueALL(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
        void disableTorqueALL(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
        void changeMode(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t desired_mode);
        int searchDynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
        int searchDynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int baudrate);
        void printInfo(int info);
        void setPosition(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int index, uint32_t desired_position);
        void setPositionALL(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint32_t desired_position);
        
    private:
        int baudrate;
	
};


//#######################################################################################
// General functions
//#######################################################################################
int getch();
int kbhit(void);