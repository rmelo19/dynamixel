#include "dynamixels.h"

//#################################################################
//
// dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate = BAUDRATE)
//
//#################################################################
dynamixels::dynamixels(char* deviceName, int protocolVersion, int desired_baudrate)
{
	// Initialize PortHandler instance
	// Set the port path
	// Get methods and members of PortHandlerLinux or PortHandlerWindows
	// char deviceName1[13] = "/dev/ttyUSB0"; //CORRECT
	// char deviceName2[13] = "/dev/ttyUSB1"; //CORRECT
	dxl_portHandler = dynamixel::PortHandler::getPortHandler(deviceName);

	// Initialize PacketHandler instance
	// Set the protocol version
	// Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
	dxl_packetHandler = dynamixel::PacketHandler::getPacketHandler(protocolVersion);

	dxl_deviceName = deviceName;
	dxl_protocolVersion = protocolVersion;

	// Open port
	if (!dxl_portHandler->openPort())
	{
		printf("Failed to open the port!\n");
	}

	// Set port baudrate
	if (!dxl_portHandler->setBaudRate(desired_baudrate))
		printf("Failed to change the baudrate!\n");

	// initializing variables
	for (int i=0;i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	{
		ID_array[i] = 0;
		positionOffset[i] = 0;
		if (dxl_protocolVersion == 2)
			operatingMode[i] = POSITION_CONTROL_MODE; // default mode
		else
		{
			operatingMode[i] = MULTI_TURN_MODE; // default mode
		}
	}
	baudrate = desired_baudrate;

	// Searching for dynamixels
	qtdDyn = searchDynamixels();

	if (dxl_protocolVersion == 2)
	{
		disableTorqueALL();
		for (int i = 0; i < MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; ++i)
		{
			readData(i, PROTOCOL_DATA);
			readData(i, FIRMWARE_DATA);
			readData(i, MAX_VOLTAGE_DATA);
			readData(i, MIN_VOLTAGE_DATA);
			readData(i, MAX_POSITION_LIMIT_DATA);
			readData(i, MIN_POSITION_LIMIT_DATA);
			readData(i, PRESENT_INPUT_VOLTAGE_DATA);
			readData(i, MOVING_DATA);
			readData(i, MOVING_STATUS_DATA);
			setVelocity(i, MAXIMUM_MOVING_SPEED);
			setCurrentLimit(i, MAXIMUM_CURRENT);
		}

		setOperatingModeALL(POSITION_CONTROL_MODE);
		setProfileVelocity(STEP_VELOCITY_PROFILE);
	}
	else if (dxl_protocolVersion == 1)
	{
		setOperatingModeALL(MULTI_TURN_MODE); // CORRECT, WHEEL_MODE?
		for (int i = 0; i < MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; ++i)
		{
			setVelocity(i, MAXIMUM_MOVING_SPEED);
		}
	}
	zeroPositionALL();
}

dynamixels::~dynamixels()
{
	if (dxl_protocolVersion == 2)
	{
		disableTorqueALL();
	}
	dxl_portHandler->closePort();
	return;
}

//#################################################################
//
// dynamixels::searchDynamixels(int info)
//     searches for dynamixels
// returns: number of dynamixels found
//#################################################################
int dynamixels::searchDynamixels(int desired_baudrate)
{
	// Search bus for Dynamixels
	int count = 0;
	bool success;

  // Set port baudrate
  if (!(dxl_portHandler->setBaudRate(desired_baudrate)))
  {
    printf("Failed to change the baudrate!\n");
  }

  for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  {
  	success = readData(i, ID_DATA);
	if (success)
		count++;
  }
  if (count == 0)
  {
  	printf("Failed to find dynamixels.\n");
  }

  // Set port baudrate back to original
  if (!dxl_portHandler->setBaudRate(baudrate))
  {
    printf("Failed to change the baudrate!\n");
  }

  return count;
}

//#################################################################
//
// dynamixels::printInfo(int info)
//
//#################################################################
void dynamixels::printInfo(int info)
{
	// print baudrate
	if (info == BAUDRATE_INFO)
		printf("Baudrate: %d\n", baudrate);

	// print IDs
	if (info == ID_ARRAY_INFO)
  		printf("IDs found: %d", qtdDyn);

	if (info == POSITION_INFO)
	{
		printf("PRESENT POSITION:\n");
		readPositionALL();
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
  			printf("    ID: %d, POSITION: %d\n", ID_array[i], presentPosition[i]);
	}
	
	if (info == HEADER_INFO)
	{
		printf("FOUND %d DYNAMIXELS:\n", qtdDyn);
		printf("    PROTOCOL VERSION: %d\n", dxl_protocolVersion);
		printf("    DEVICE PORT: %s\n", dxl_deviceName);
		printf("    BAUDRATE: %d\n", baudrate);
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
  		{
  			printf("    ID: %d\n", ID_array[i]);		
  			if (dxl_protocolVersion == 2)
	  		{
	  			printf("        FIRMWARE: %d\n", firmware[i]);
	  			printf("        PROTOCOL: %d\n", protocol[i]);
	  			printf("        MIN_POSITION_LIMIT: %d\n", minPositionLimit[i]);
	  			printf("        MAX_POSITION_LIMIT: %d\n", maxPositionLimit[i]);
	  			printf("        PRESENT_INPUT_VOLTAGE: %.1fV\n", inputVoltage[i]*0.1);
	  			printf("        MIN_VOLTAGE_LIMIT: %.1fV\n", minVoltageLimit[i]*0.1);
	  			printf("        MAX_VOLTAGE_LIMIT: %.1fV\n", maxVoltageLimit[i]*0.1);
	  			printf("        CURRENT_LIMIT: %d\n", currentLimit[i]);
  			}
  		}
  		printf("\n");
	}
	if (info == MOVING_INFO)
	{
		printf("MOVING STATUS:\n");
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			readData(i, MOVING_DATA);
  			printf("    ID: %d, MOVING: %d\n", ID_array[i], isMoving[i]);
  		}
	}

	if (info == DETAILS_INFO)
	{
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
  		{
  			printf("    ID: %d", ID_array[i]);
  			readData(i, POSITION_DATA);
  			readData(i, PRESENT_CURRENT_DATA);
  			if (dxl_protocolVersion == 2)
	  			printf(" POSITION %6.2f, CURRENT %6.2f", presentPosition[i]*0.088, presentCurrent[i]*2.69/1000);
	  		else
	  			printf(" POSITION %6.2f, CURRENT %6d", presentPosition[i]*0.088, presentCurrent[i]);
  		}
  		printf("\n");
	}

}


bool dynamixels::isIdle(void)
{
	bool moving = true;
    readDataALL(MOVING_STATUS_DATA);
    for (int i = 0; i < qtdDyn; i++)
    {
      moving = moving || !((bool) (movingStatus[i] & 0b00000001));
    }
    return !moving;
}

//#################################################################
//
// void dynamixels::setCurrent(int index, uint16_t desired_current)
//
//#################################################################
void dynamixels::setCurrent(int index, uint16_t desired_current)
{
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error;
	if (dxl_protocolVersion == 2)
	{
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_GOAL_CURRENT, (uint16_t) desired_current, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Failed to change current limit for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Failed to change current limit for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
	}
	else
	{
		printf("Feature not available for protocol %d.\n", dxl_protocolVersion);
		return;
	}
}

//#################################################################
//
// void dynamixels::setCurrentALL(uint16_t desired_current)
//
//#################################################################
void dynamixels::setCurrentALL(uint16_t desired_current)
{
	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i]!= 0; i++)
	{
		setCurrent(i, desired_current);
	}
}


//#################################################################
//
// void dynamixels::setCurrentLimit(int index, uint16_t desired_current_limit);
//
//#################################################################
void dynamixels::setCurrentLimit(int index, uint16_t desired_current_limit)
{
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error;
	if (dxl_protocolVersion == 1)
	{
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_TORQUE_LIMIT, (uint16_t) desired_current_limit, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
		else
		{
			currentLimit[index] = desired_current_limit; // CORRECT
		}
	}
	if (dxl_protocolVersion == 2)
	{
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_CURRENT_LIMIT, (uint16_t) desired_current_limit, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Failed to change current limit for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Failed to change current limit for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
		else
		{
			currentLimit[index] = desired_current_limit;
		}
	}
}

//#################################################################
//
// void dynamixels::setCurrentLimitALL(uint16_t desired_current_limit)
//
//#################################################################
void dynamixels::setCurrentLimitALL(uint16_t desired_current_limit)
{
	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i]!= 0; i++)
	{
		setCurrentLimit(i, desired_current_limit);
	}
}


//#################################################################
//
// void dynamixels::setPosition(int index, int desired_position)
//
//#################################################################
void dynamixels::setPosition(int index, uint32_t desired_position, uint16_t desired_velocity)
{
	if (desired_position < DXL_MINIMUM_POSITION_VALUE)
	{
		printf("Desired position is too low.\n");
	}
	if (desired_position > DXL_MAXIMUM_POSITION_VALUE)
	{
		printf("Desired position is too high.\n");
	}

	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error; 
	// Write goal position
	if (dxl_protocolVersion == 2)
		dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_GOAL_POSITION, (uint32_t) desired_position, &dxl_error);
	else
	{
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_GOAL_POSITION, (uint16_t) desired_position, &dxl_error);
		setVelocity(index, desired_velocity);
	}

	if (dxl_comm_result != COMM_SUCCESS)
	{
		dxl_packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		dxl_packetHandler->printRxPacketError(dxl_error);
  	}

}

//#################################################################
//
// void dynamixels::setPositionALL(int desired_position)
//
//#################################################################
void dynamixels::setPositionALL(uint32_t desired_position)
{
	for(int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++) // CORRECT
  	{
  		if (ID_array[i] == 0)
  			break;
		// Write goal position
		setPosition(i, desired_position);
	}

	uint32_t lowerLimit = desired_position - POSITION_THRESHOLD;
	uint32_t upperLimit = desired_position + POSITION_THRESHOLD;
	if ((int) desired_position - POSITION_THRESHOLD < DXL_MINIMUM_POSITION_VALUE)
		lowerLimit = DXL_MINIMUM_POSITION_VALUE;
	if ((int) desired_position + POSITION_THRESHOLD > DXL_MAXIMUM_POSITION_VALUE)
		upperLimit = DXL_MAXIMUM_POSITION_VALUE;

	uint32_t position;
	bool inPlace = true;
  	for(int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	{
		if (ID_array[i] == 0)
		{
			if (!inPlace)
			{
				i = 0;
				inPlace = true;
			}
			else 
				break;
		}
		
		position = 0;
		position = readPosition(i);
		inPlace = inPlace && (position < upperLimit && position > lowerLimit);
		// printf("ID: %d, POSITION: %d %d %d\n", ID_array[i], position, upperLimit, lowerLimit); // CORRECT
	}
}

//#################################################################
//
// uint32_t dynamixels::readPosition(int index)
//
//#################################################################
uint32_t dynamixels::readPosition(int index)
{
	readData(index, POSITION_DATA);
	return presentPosition[index];
} 

//#################################################################
//
// void dynamixels::readPositionALL(void)
//     
//
//#################################################################
void dynamixels::readPositionALL(void)
{
	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
	{
		readPosition(i);
	}
}


bool dynamixels::readData(int index, int info)
{
	int dxl_comm_result;
	uint8_t dxl_error;
	bool success;
	// static int count = 0; // CORRECT
	int count = 0;
	switch(info)
	{
		case ID_DATA:
			uint8_t dxl_id;
			if (dxl_protocolVersion == 2)
		    	dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, index, ADDR_PRO_ID, (uint8_t*)&dxl_id, &dxl_error);
		    else
		    	dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, index, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
		    if (dxl_comm_result == COMM_SUCCESS)
			{
				// ID_array[count] = dxl_id;
				while (ID_array[count] != 0) count++;
				ID_array[count] = dxl_id;

				success = true;
				// count++; // CORRECT
				count = 0;
			}
			else
			{
				success = false;
			}
		    break;
		case PROTOCOL_DATA:
			uint8_t dxl_protocol;
			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_PROTOCOL_VERSION, (uint8_t*)&dxl_protocol, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				protocol[index] = dxl_protocol;
				success = true;
			}
			else
			{
				printf("Failed to read protocol.\n");
				success = false;
			}
			break;
		case MIN_POSITION_LIMIT_DATA:
			uint32_t dxl_minPositionLimit;
			dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_MIN_POSITION_LIMIT, (uint32_t*)&dxl_minPositionLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				minPositionLimit[index] = dxl_minPositionLimit;
				success = true;
			}
			else
			{
				printf("Failed to read minimum position limit.\n");
				success = false;
			}
			break;
		case MAX_POSITION_LIMIT_DATA:
			uint32_t dxl_maxPositionLimit;
			dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_MAX_POSITION_LIMIT, (uint32_t*)&dxl_maxPositionLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				maxPositionLimit[index] = dxl_maxPositionLimit;
				success = true;
			}
			else
			{
				printf("Failed to read maximum position limit.\n");
				success = false;
			}
			break;
		case CURRENT_LIMIT_DATA:
			uint16_t dxl_currentLimit;
			dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_CURRENT_LIMIT, (uint16_t*)&dxl_currentLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				currentLimit[index] = dxl_currentLimit;
				success = true;
			}
			else
			{
				printf("Failed to read current limit.\n");
				success = false;
			}
			break;
		case PRESENT_CURRENT_DATA:
			if (dxl_protocolVersion == 2)
			{
				uint16_t dxl_presentCurrent;
				dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_PRESENT_CURRENT, (uint16_t*)&dxl_presentCurrent, &dxl_error);
				int16_t dxl_presentCurrentINT;
				dxl_presentCurrentINT = (int16_t) dxl_presentCurrent;
				if (dxl_comm_result == COMM_SUCCESS)
				{
					presentCurrent[index] = dxl_presentCurrentINT;
					success = true;
				}
				else
				{
					printf("Failed to read current limit.\n");
					success = false;
				}
			}
			else
			{
				uint16_t dxl_presentLoad;
				dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRESENT_LOAD, (uint16_t*)&dxl_presentLoad, &dxl_error);
				if (dxl_comm_result == COMM_SUCCESS)
				{
					presentCurrent[index] = dxl_presentLoad; // CORRECT
					success = true;
				}
				else
				{
					printf("Failed to read current limit.\n");
					success = false;
				}
			}
			break;


		case FIRMWARE_DATA:
			uint8_t dxl_firmware;
			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_VERSION_FIRMWARE, (uint8_t*)&dxl_firmware, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				firmware[index] = dxl_firmware;
				success = true;
			}
			else
			{
				printf("Failed to read firmware.\n");
				success = false;
			}
			break;
		case POSITION_DATA:
			// Read present position
			uint32_t dxl_present_position;
			if (dxl_protocolVersion == 2)	
				dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
			else
			{
				uint16_t aux;
				dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRESENT_POSITION, (uint16_t*)&aux, &dxl_error);
				dxl_present_position = (uint32_t) aux;
			}
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to read position.");
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
				success = false;
			}
			else if (dxl_error != 0)
			{
				printf("Failed to read position.");
			    dxl_packetHandler->printRxPacketError(dxl_error);
			    success = false;
			}
			else
			{
				presentPosition[index] = dxl_present_position;
				success = true;
				// presentPosition[index] = (uint32_t) ( (int) dxl_present_position + positionOffset[index]);		
			}
			// printf("%d %d %d\n", (int) dxl_present_position, positionOffset[index], presentPosition[index] + positionOffset[index]); // CORRECT
			break;
		case PRESENT_INPUT_VOLTAGE_DATA:
			uint16_t dxl_inputVoltage;
			dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_PRESENT_INPUT_VOLTAGE, (uint16_t*)&dxl_inputVoltage, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				inputVoltage[index] = dxl_inputVoltage;
				success = true;
			}
			else
			{
				printf("Failed to read input voltage.\n");
				success = false;
			}
			break;
		case MAX_VOLTAGE_DATA:
			uint16_t dxl_maxVoltageLimit;
			dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_MAX_VOLTAGE, (uint16_t*)&dxl_maxVoltageLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				maxVoltageLimit[index] = dxl_maxVoltageLimit;
				success = true;
			}
			else
			{
				printf("Failed to read maximum voltage limit.\n");
				success = false;
			}		
			break;
		case MIN_VOLTAGE_DATA:
			uint16_t dxl_minVoltageLimit;
			dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_MIN_VOLTAGE, (uint16_t*)&dxl_minVoltageLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				minVoltageLimit[index] = dxl_minVoltageLimit;
				success = true;
			}
			else
			{
				printf("Failed to read minimum voltage limit.\n");
				success = false;
			}
			break;
		case MOVING_DATA:
			uint8_t dxl_isMoving;
  			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, ID_array[index], ADDR_MOVING, (uint8_t*)&dxl_isMoving, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error detecting movement for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
				success = false;
			}
			else if (dxl_error != 0)
			{	
				printf("Error detecting movement for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printRxPacketError(dxl_error);
				success = false;
			}
			else
			{
				isMoving[index] = (dxl_isMoving == 1);
				printf("%d\n", dxl_isMoving);
				success = true;
			}
			break;
		case MOVING_STATUS_DATA:
			uint8_t dxl_moving_status_data;
  			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_MOVING_STATUS, (uint8_t*)&dxl_moving_status_data, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to read moving status for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
				success = false;
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to read moving status for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printRxPacketError(dxl_error);
				success = false;
			}
			else
			{
				movingStatus[index] = dxl_moving_status_data;
				success = true;
			}
			break;
		default:
			success = false;
			break;

	}
	return success;
}

bool dynamixels::readDataALL(int info)
{
	bool success = true;
	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
	{
		success = success && readData(i, info);
	}
	return success;
}

//#################################################################
//
// void dynamixels::zeroPositionALL(void)
//     
//
//#################################################################
void dynamixels::zeroPositionALL(void)
{
	int dxl_comm_result;
	uint8_t dxl_error;

	if (dxl_protocolVersion == 2)
	{
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_HOMING_OFFSET, 0, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to zero position.");
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{
				printf("Failed to zero position.");
			    dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
		readPositionALL();
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			positionOffset[i] = -(int) presentPosition[i];
		}
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_HOMING_OFFSET, positionOffset[i], &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to zero position.");
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{
				printf("Failed to zero position.");
			    dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}
	else if (dxl_protocolVersion == 1)
	{
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[i], ADDR_MULTI_TURN_OFFSET, 0, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to zero position.\n");
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{
				printf("Failed to zero position.\n");
			    dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
		readPositionALL();
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			positionOffset[i] = -(int) presentPosition[i];
		}
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
		{
			dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[i], ADDR_MULTI_TURN_OFFSET, positionOffset[i], &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to zero position.\n");
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{
				printf("Failed to zero position.\n");
			    dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}
}


void dynamixels::setOperatingMode(int index, uint8_t desired_mode)
{
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error = 0;                          // Dynamixel error
	// Changing mode
	if (dxl_protocolVersion == 2)
	{
		dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_OPERATING_MODE, desired_mode, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Error changing mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Error changing mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
		else
		{
			operatingMode[index] = desired_mode;
		}
	}
	else
	{
		uint16_t CW;
		uint16_t CCW;
		switch(desired_mode)
		{
			case WHEEL_MODE:
				CW = 0;
				CCW = 0;
				break;
			case JOINT_MODE:
				CW = 1000;
				CCW = 1000;
				break;
			case MULTI_TURN_MODE:
				CW = 4095;
				CCW = 4095;
				break;
			default:
				printf("Couldn't find desired mode.\n");
				return;
		}
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_CW_ANGLE_LIMIT, CW, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Failed to change mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Failed change mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_CCW_ANGLE_LIMIT, CCW, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			printf("Failed to change mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{	
			printf("Failed to change mode for dynamixel %d, ", ID_array[index]);
			dxl_packetHandler->printRxPacketError(dxl_error);
		}
		else
		{
			operatingMode[index] = desired_mode;
		}
	}
}


	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error = 0;                          // Dynamixel error
	// Changing modey
//#################################################################
//
// void dynamixels::setOperatingModeALL(uint8_t desired_mode)
//
//#################################################################
void dynamixels::setOperatingModeALL(uint8_t desired_mode)
{
	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
  	{
  		setOperatingMode(i, desired_mode);
	}
}

//#################################################################
//
// PROTOCOL SPECIFIC FUNCTIONS
//
//#################################################################
	//#################################################################
	//
	// void dynamixels::setPositionALL(int desired_position)
	//
	//#################################################################
	void dynamixels::setMaxInputVoltage(int desired_max_input_voltage)
	{
		uint16_t desired_voltage = desired_max_input_voltage*10;
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error;
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
		{
			if (dxl_protocolVersion == 2)
			{
				dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_MAX_VOLTAGE, (uint16_t) desired_voltage, &dxl_error);
				if (dxl_comm_result != COMM_SUCCESS)
				{
					printf("Failed to set max input voltage, ID: %d, ", ID_array[i]);
					dxl_packetHandler->printTxRxResult(dxl_comm_result);
				}
				else if (dxl_error != 0)
				{	
					printf("Failed to set max input voltage, ID: %d, ", ID_array[i]);
					dxl_packetHandler->printRxPacketError(dxl_error);
				}
				else
				{
					maxVoltageLimit[i] = desired_voltage;
				}
			}
		}
	}

	//#################################################################
	//
	// void dynamixels::setVelocity(int index, uint32_t desired_position)
	//
	//#################################################################
	void dynamixels::setVelocity(int index, uint16_t desired_velocity)
	{
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error;
		if (dxl_protocolVersion == 1)
		{
			dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_MOVING_SPEED, (uint16_t) desired_velocity, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
		if (dxl_protocolVersion == 2)
		{
			dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRO_VELOCITY_LIMIT, (uint32_t) desired_velocity, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to change velocity for dynamixel %d, ", ID_array[index]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}

	//#################################################################
	//
	// void dynamixels::setProfileVelocity(uint8_t desired_profile_velocity)
	//
	//#################################################################
	void dynamixels::setProfileVelocity(uint8_t desired_profile_velocity)
	{
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error;

		if (dxl_protocolVersion != 2)
		{
			printf("Feature not available for protocol %d.\n", dxl_protocolVersion);
			return;
		}

		uint32_t velocityProfile;
		uint32_t accelerationProfile;

		switch(desired_profile_velocity)
		{
			case STEP_VELOCITY_PROFILE:
				velocityProfile = 0;
				accelerationProfile = MAXIMUM_MOVING_SPEED;
				break;

			case RECTANGLE_VELOCITY_PROFILE:
				velocityProfile = MAXIMUM_MOVING_SPEED;
				accelerationProfile = 0;
				break;

			case TRIANGLE_VELOCITY_PROFILE:
				velocityProfile = MAXIMUM_MOVING_SPEED;
				accelerationProfile = MAXIMUM_MOVING_SPEED;
				break;

			case TRAPEZOIDAL_VELOCITY_PROFILE:
				velocityProfile = MAXIMUM_MOVING_SPEED;
				accelerationProfile = MAXIMUM_MOVING_SPEED;
				break;
			default:
				velocityProfile = 0;
				accelerationProfile = MAXIMUM_MOVING_SPEED;
				break;
		}
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0;i++)
		{
			// Writing Velocity Profile
			dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_PROFILE_VELOCITY, velocityProfile, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to change velocity profile, ID: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to change velocity profile, ID: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
			// Writing Acceleration Profile
			dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_PROFILE_ACCELERATION, accelerationProfile, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to change acceleration profile, ID: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to change acceleration profile,  ID: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}

	//#################################################################
	//
	// void dynamixels::enableTorqueALL(void)
	//
	//#################################################################
	void dynamixels::enableTorqueALL(void)
	{
		if (dxl_protocolVersion != 2)
		{
			printf("Protocol not compatible for this function.\n");
			return;
		}
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error = 0;                          // Dynamixel error
		// Enable Dynamixel Torque
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	  	{
	  		if (ID_array[i] == 0)
	  			break;
			dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to enable torque for dynamixel %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to enable torque for dynamixel %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}

	//#################################################################
	//
	// void dynamixels::disableTorqueALL(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
	//
	//#################################################################
	void dynamixels::disableTorqueALL(void)
	{
		if (dxl_protocolVersion != 2)
		{
			printf("Protocol not compatible for this function.\n");
			return;
		}
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error = 0;                          // Dynamixel error
		// Enable Dynamixel Torque
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS && ID_array[i] != 0; i++)
	  	{
			dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Failed to disable torque for dynamixel %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Failed to disable torque for dynamixel %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}


//#######################################################################################
// General functions
//#######################################################################################

//#################################################################
//
// int getch()
//
//#################################################################
int getch()
{
#ifdef __linux__
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#elif defined(_WIN32) || defined(_WIN64)
  return _getch();
#endif
}

//#################################################################
//
// int kbhit(void)
//
//#################################################################
int kbhit(void)
{
#ifdef __linux__
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined(_WIN32) || defined(_WIN64)
  return _kbhit();
#endif
}