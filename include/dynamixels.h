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
        dynamixels(char* deviceName, int protocolVersion=2, int desired_baudrate = BAUDRATE);
		~dynamixels();

        dynamixel::PortHandler *dxl_portHandler;
        dynamixel::PacketHandler *dxl_packetHandler;

        uint8_t ID_array[MAXIMUM_NUMBER_DYNAMIXELS];
        uint8_t firmware[MAXIMUM_NUMBER_DYNAMIXELS];
        uint8_t operatingMode[MAXIMUM_NUMBER_DYNAMIXELS];
        uint8_t movingStatus[MAXIMUM_NUMBER_DYNAMIXELS];

        char *dxl_deviceName;
        int dxl_protocolVersion;
        int qtdDyn; // dynamixels quantity
        
        uint8_t  protocol[MAXIMUM_NUMBER_DYNAMIXELS];
        uint32_t minPositionLimit[MAXIMUM_NUMBER_DYNAMIXELS];
        uint32_t maxPositionLimit[MAXIMUM_NUMBER_DYNAMIXELS];
        uint16_t inputVoltage[MAXIMUM_NUMBER_DYNAMIXELS];
        uint16_t maxVoltageLimit[MAXIMUM_NUMBER_DYNAMIXELS];
        uint16_t minVoltageLimit[MAXIMUM_NUMBER_DYNAMIXELS];
        uint32_t presentPosition[MAXIMUM_NUMBER_DYNAMIXELS];
        int16_t presentCurrent[MAXIMUM_NUMBER_DYNAMIXELS];
        uint16_t currentLimit[MAXIMUM_NUMBER_DYNAMIXELS];
        int positionOffset[MAXIMUM_NUMBER_DYNAMIXELS];
        bool isMoving[MAXIMUM_NUMBER_DYNAMIXELS];

        uint8_t goalCurrent[MAXIMUM_NUMBER_DYNAMIXELS];

        void setProfileVelocity(uint8_t desired_profile_velocity = STEP_VELOCITY_PROFILE);
        void enableTorqueALL(void);
        void disableTorqueALL(void);
        uint32_t readPosition(int index);
        void readPositionALL(void);

        bool readData(int index, int info);
        bool readDataALL(int info);
        bool isIdle(void);

        void zeroPositionALL(void);
        void setOperatingMode(int index, uint8_t desired_mode);
        void setOperatingModeALL(uint8_t desired_mode);
        void printInfo(int info);
        void setCurrent(int index, uint16_t desired_current);
        void setCurrentALL(uint16_t desired_current);
        void setPosition(int index, uint32_t desired_position, uint16_t desired_velocity = MAXIMUM_MOVING_SPEED);
        void setPositionALL(uint32_t desired_position);
        void setMaxInputVoltage(int desired_max_input_voltage);
        void setVelocity(int index, uint16_t desired_velocity = MAXIMUM_MOVING_SPEED);
        void setCurrentLimit(int index, uint16_t desired_current_limit);
        void setCurrentLimitALL(uint16_t desired_current_limit);
        int searchDynamixels(int desired_baudrate = BAUDRATE);
        
        
    private:
        int baudrate;
	
};


//#######################################################################################
// General functions
//#######################################################################################
int getch();
int kbhit(void);