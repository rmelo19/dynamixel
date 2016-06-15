#include "dynamixels.h"

//#################################################################
//
// dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate = BAUDRATE)
//
//#################################################################
dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate)
{
	// Open port
	if (!portHandler->openPort())
	{
		printf("Failed to open the port!\n");
	}

	// Set port baudrate
	if (!portHandler->setBaudRate(desired_baudrate))
		printf("Failed to change the baudrate!\n");

	// initializing variables
	for (int i=0;i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	{
		ID_array[i] = 0;
		
		#if PROTOCOL_VERSION == 2
			operatingMode[i] = POSITION_CONTROL_MODE; // default mode
		#else
			operatingMode[i] = WHEEL_MODE; // default mode
		#endif
	}
	baudrate = desired_baudrate;
	qtdDyn = 0;

	dxl_packetHandler = packetHandler;
	dxl_portHandler = portHandler;

	// Searching for dynamixels
	searchDynamixels();


	#if PROTOCOL_VERSION == 2
	#else
		changeMode(WHEEL_MODE);
	#endif
}

dynamixels::~dynamixels()
{
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
	int dxl_comm_result;
	uint8_t dxl_error; 
	uint8_t dxl_id;

  	#if PROTOCOL_VERSION == 2
		uint8_t dxl_firmware;
		uint8_t dxl_protocol;	  
		uint32_t dxl_minPositionLimit;
		uint32_t dxl_maxPositionLimit;
	#endif


  // Set port baudrate
  if (!(dxl_portHandler->setBaudRate(desired_baudrate)))
  {
    printf("Failed to change the baudrate!\n");
  }

  for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  {
    dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, i, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
	if (dxl_comm_result == COMM_SUCCESS)
	{
		ID_array[count] = dxl_id;
		#if PROTOCOL_VERSION == 2
			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, i, ADDR_PROTOCOL_VERSION, (uint8_t*)&dxl_protocol, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				protocol[count] = dxl_protocol;
			}
			else
			{
				printf("Failed to read protocol.\n");
			}
			dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, i, ADDR_MIN_POSITION_LIMIT, (uint32_t*)&dxl_minPositionLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				minPositionLimit[count] = dxl_minPositionLimit;
			}
			else
			{
				printf("Failed to read minimum position limit.\n");
			}
			dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, i, ADDR_MAX_POSITION_LIMIT, (uint32_t*)&dxl_maxPositionLimit, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				maxPositionLimit[count] = dxl_maxPositionLimit;
			}
			else
			{
				printf("Failed to read maximum position limit.\n");
			}
			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, i, ADDR_VERSION_FIRMWARE, (uint8_t*)&dxl_firmware, &dxl_error);
			if (dxl_comm_result == COMM_SUCCESS)
			{
				firmware[count] = dxl_firmware;
			}
			else
			{
				printf("Failed to read firmware.\n");
			}
		#endif
		count++;
	}
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

  qtdDyn = count;
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
	{
		printf("Baudrate: %d\n", baudrate);
	}

	// print IDs
	if (info == ID_ARRAY_INFO)
	{
  		printf("IDs found: ");
		for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			printf(" %d", ID_array[i]);
  		}
	}

	if (info == POSITION_INFO)
	{
		printf("PRESENT POSITION:\n");
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			printf("    ID: %d, POSITION: %d\n", ID_array[i], presentPosition[i]);
  		}
  		printf("\n");
	}
	
	if (info == HEADER_INFO)
	{
		printf("DYNAMIXELS:\n");
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			printf("    ID: %d\n", ID_array[i]);		
  			#if PROTOCOL_VERSION == 2
	  			printf("        FIRMWARE: %d\n", firmware[i]);
	  			printf("        PROTOCOL: %d\n", protocol[i]);
	  			printf("        MIN_POSITION_LIMIT: %d\n", minPositionLimit[i]);
	  			printf("        MAX_POSITION_LIMIT: %d\n", maxPositionLimit[i]);
  			#endif
  		}
  		printf("\n");
	}
	if (info == MOVING_INFO)
	{
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error = 0;                          // Dynamixel error
		uint8_t isMoving = 0;
		

		printf("MOVING STATUS:\n");
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			dxl_comm_result = dxl_packetHandler->read1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_MOVING, (uint8_t*)&isMoving, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error detecting movement for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error detecting movement for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
  			if (ID_array[i] == 0)
  				break;
  			printf("    ID: %d, MOVING: %d\n", ID_array[i], isMoving);
  		}
  		printf("\n");
	}
}

//#################################################################
//
// void dynamixels::setPosition(int index, int desired_position)
//
//#################################################################
void dynamixels::setPosition(int index, uint32_t desired_position)
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
	#if PROTOCOL_VERSION == 2
		dxl_comm_result = dxl_packetHandler->write4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_GOAL_POSITION, (uint32_t) desired_position, &dxl_error);
	#else
		// uint16_t desired_velocity = 1030; // slow CW speed // CORRECT
		uint16_t desired_velocity = 20; // slow CCW speed // CORRECT
		
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_GOAL_POSITION, (uint16_t) desired_position, &dxl_error);
		dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_MOVING_SPEED, (uint16_t) desired_velocity, &dxl_error);
	#endif

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

	#if PROTOCOL_VERSION == 2
		uint32_t position;
	#else
		uint16_t position;
	#endif
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
		printf("ID: %d, POSITION: %d %d %d\n", ID_array[i], position, upperLimit, lowerLimit);
	}
}

//#################################################################
//
// void dynamixels::enableTorqueALL(void)
//     reads position, stores in object attribute and returns it.
// returns: the present position
//#################################################################
#if PROTOCOL_VERSION == 2
	uint32_t dynamixels::readPosition(int index)
#else
	uint16_t dynamixels::readPosition(int index)
#endif
{
	int dxl_comm_result;
	uint8_t dxl_error; 
	// Read present position
	#if PROTOCOL_VERSION == 2
		uint32_t dxl_present_position;
		dxl_comm_result = dxl_packetHandler->read4ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
	#else
		uint16_t dxl_present_position;
		dxl_comm_result = dxl_packetHandler->read2ByteTxRx(dxl_portHandler, ID_array[index], ADDR_PRESENT_POSITION, (uint16_t*)&dxl_present_position, &dxl_error);
	#endif

	if (dxl_comm_result != COMM_SUCCESS)
	{
		dxl_packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
	    dxl_packetHandler->printRxPacketError(dxl_error);
	}
	presentPosition[index] = dxl_present_position;
	return dxl_present_position;
}
//#################################################################
//
// void dynamixels::changeMode(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t desired_mode)
//
//#################################################################
void dynamixels::changeMode(uint8_t desired_mode)
{
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	uint8_t dxl_error = 0;                          // Dynamixel error
	// Changing mode

	for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  	{
  		if (ID_array[i] == 0)
  			break;
  		#if PROTOCOL_VERSION == 2
			dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_OPERATING_MODE, desired_mode, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
			else
			{
				operatingMode[i] = desired_mode;
			}
		#else
			uint16_t CW;
			uint16_t CCW;
			if (desired_mode == WHEEL_MODE)
			{
				CW = 0;
				CCW = 0;
			}
			else if (desired_mode == JOINT_MODE)
			{
				CW = 1000;
				CCW = 1000;
			}
			else
			{
				CW = 4095;
				CCW = 4095;
			}
			dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[i], ADDR_CW_ANGLE_LIMIT, CW, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
			dxl_comm_result = dxl_packetHandler->write2ByteTxRx(dxl_portHandler, ID_array[i], ADDR_CCW_ANGLE_LIMIT, CCW, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error changing mode for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
			else
			{
				operatingMode[i] = desired_mode;
			}
		#endif
	}
}

//#################################################################
//
// PROTOCOL SPECIFIC FUNCTIONS
//
//#################################################################
#if PROTOCOL_VERSION == 2
	//#################################################################
	//
	// void dynamixels::enableTorqueALL(void)
	//
	//#################################################################
	void dynamixels::enableTorqueALL(void)
	{
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error = 0;                          // Dynamixel error
		// Enable Dynamixel Torque
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	  	{
	  		if (ID_array[i] == 0)
	  			break;
			dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error enabling torque for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error enabling torque for dynamixel: %d, ", ID_array[i]);
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
		int dxl_comm_result = COMM_TX_FAIL;             // Communication result
		uint8_t dxl_error = 0;                          // Dynamixel error
		// Enable Dynamixel Torque
		for (int i=0; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	  	{
	  		if (ID_array[i] == 0)
	  			break;
			dxl_comm_result = dxl_packetHandler->write1ByteTxRx(dxl_portHandler, ID_array[i], ADDR_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				printf("Error disabling torque for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{	
				printf("Error disabling torque for dynamixel: %d, ", ID_array[i]);
				dxl_packetHandler->printRxPacketError(dxl_error);
			}
		}
	}
#endif


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