#include "dynamixels.h"

//#################################################################
//
// dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
//
//#################################################################
dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
{
	// Open port
	if (!portHandler->openPort())
	{
		printf("Failed to open the port!\n");
	}

	// Set port baudrate
	if (!portHandler->setBaudRate(desired_baudrate))
		printf("Failed to change the baudrate!\n");

	// Searching for dynamixels
	searchDynamixels(desired_baudrate);

	// initializing variables
	for (int i=0;i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	{
		ID_array[i] = 0;
	}
}

//#################################################################
//
// dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate = BAUDRATE)
//
//#################################################################
dynamixels::dynamixels(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int desired_baudrate = BAUDRATE)
{
	// Open port
	if (!portHandler->openPort())
	{
		printf("Failed to open the port!\n");
	}

	// Set port baudrate
	if (!portHandler->setBaudRate(desired_baudrate))
		printf("Failed to change the baudrate!\n");

	// Searching for dynamixels
	searchDynamixels(desired_baudrate);

	// initializing variables
	for (int i=0;i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
	{
		ID_array[i] = 0;
	}
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
int dynamixels::searchDynamixels()
{
  // Search bus for Dynamixels
  int count = 0;
  for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  {
    dxl_comm_result = packetHandler->read1ByteTxRx(portHandler, i, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
    if (dxl_comm_result == COMM_SUCCESS)
    {
      ID_array[count] = dxl_id;
      count++;
    }
  }
  if (count == 0)
  {
  	printf("Failed to find dynamixels.\n");
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
  		printf("IDs found: ")
		for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			printf(" %d", ID_array[i])
  		}
	}

	if (info == POSITION_INFO)
	{
		printf("Present position: ")
		for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  		{
  			if (ID_array[i] == 0)
  				break;
  			printf(" %d", ID_array[i])
  		}
	}
	
	if (info == HEADER_INFO)
	{
		
	}
}

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
	for (int i=1; i<MAXIMUM_NUMBER_DYNAMIXELS;i++)
  	{
		dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, i, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{
			packetHandler->printRxPacketError(dxl_error);
		}
	}
}

//#################################################################
//
// void dynamixels::setPosition(int index, int desired_position)
//
//#################################################################
void dynamixels::setPosition(int index, int desired_position)
{
	// Write goal position
	dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, ID_array[index], ADDR_PRO_GOAL_POSITION, desired_position, &dxl_error);
	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
  	}
}

//#################################################################
//
// void dynamixels::enableTorqueALL(void)
//
// returns: the present position
//#################################################################
int32_t dynamixels::readPosition(int index)
{
	int32_t dxl_present_position = 0;
	// Read present position
	dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, ID_array[index], ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
	    packetHandler->printRxPacketError(dxl_error);
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