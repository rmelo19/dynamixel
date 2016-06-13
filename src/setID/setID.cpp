/*
 * read_write.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Read and Write Example      *********
//
//
// Available Dynamixel model on this example : All models using Protocol 2.0
// This example is tested with a Dynamixel PRO 54-200, and an USB2DYNAMIXEL
// Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000)
//

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>

// #include <cstdlib.h>

#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library

// Control table address
// #define ADDR_PRO_TORQUE_ENABLE          562                 // Control table address is different in Dynamixel model
// #define ADDR_PRO_GOAL_POSITION          596
// #define ADDR_PRO_PRESENT_POSITION       611
#define ADDR_PRO_TORQUE_ENABLE          64                 // Control table address is different in Dynamixel model
#define ADDR_PRO_GOAL_POSITION          116
#define ADDR_PRO_PRESENT_POSITION       132


// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID                          2                   // Dynamixel ID: 1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      0            // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4095              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b


// new defines
#define ADDR_ID                         7
#define DESIRED_ID                      1 // default desired ID
#define TARGET_ID                       1 // default target ID
#define ADDR_BAUD_RATE                  8
#define DESIRED_BAUD_RATE               3 // this translates to 1000000 baud rate


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

int main(int argc, char* argv[])
{
  // check if received desired ID as argument
  uint8_t desired_id;
  uint8_t target_id;

  if (argc > 2)
  {
    desired_id = atoi(argv[1]);
    target_id = atoi(argv[2]);
  }
  else
  {
    desired_id = DESIRED_ID;
    target_id =  TARGET_ID;
  }
  printf("################################################\n");
  printf("Target Dynamixel ID: %d\n", target_id);
  printf("Desired ID: %d\n", desired_id);
  // if the target dynamixel id already have the desired_id, don't do anything
  if (target_id == desired_id)
  {
    printf("Target dynamixel ID is equal to the desired ID.\n");
    return 0; 
  }

  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

  // Initialize PacketHandler instance
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

  // int index = 0;
  int dxl_comm_result = COMM_TX_FAIL;             // Communication result
  // int dxl_goal_position[2] = {DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};         // Goal position

  uint8_t dxl_error = 0;                          // Dynamixel error
  // int32_t dxl_present_position = 0;               // Present position

  uint8_t dxl_id = 1;

  printf("################################################\n");
  // Open port
  if (portHandler->openPort())
  {
    printf("Succeeded to open the port!\n");
  }
  else
  {
    printf("Failed to open the port!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Set port baudrate
  printf("################################################\n");
  if (portHandler->setBaudRate(57600))
  {
    printf("Succeeded to change the baudrate!\n");
  }
  else
  {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Search bus for Dynamixels
  printf("################################################\n");
  printf("Searching for Dynamixels with baud rate: %d\n", 57600);
  bool idConflict = false;
  bool foundTarget = false;
  for (int i=1; i<11;i++)
  {
    dxl_comm_result = packetHandler->read1ByteTxRx(portHandler, i, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
      printf("    No dynamixel found with ID %d\n", i);
      foundTarget = foundTarget || dxl_id == target_id;
    }
    else
    {
      printf("    Found dynamixel with ID %d.\n", dxl_id);
      idConflict = idConflict || dxl_id == desired_id;
      dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, dxl_id, ADDR_BAUD_RATE, DESIRED_BAUD_RATE, &dxl_error);
        if (dxl_comm_result == COMM_SUCCESS)
        {
          printf("        Succeeded to change baud rate to %d\n", DESIRED_BAUD_RATE);
        }
    }
  }

  // Set port baudrate
  printf("################################################\n");
  if (portHandler->setBaudRate(BAUDRATE))
  {
    printf("Succeeded to change the baudrate!\n");
  }
  else
  {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Search bus for Dynamixels
  printf("################################################\n");
  printf("Searching for Dynamixels with baud rate: %d\n", BAUDRATE);
  for (int i=1; i<11;i++)
  {
    dxl_comm_result = packetHandler->read1ByteTxRx(portHandler, i, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
      printf("    No dynamixel found with ID %d\n", i);
      foundTarget = foundTarget || dxl_id == target_id;
    }
    else
    {
      printf("    Found dynamixel with ID %d.\n", dxl_id);
      idConflict = idConflict || dxl_id == desired_id;
    }
  }
  
  printf("################################################\n");
  if (!idConflict && foundTarget)
  {
    // set Dynamixel ID
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, target_id, ADDR_ID, desired_id, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
      printf("Failed to change dynamixel ID!\n");
    }
    else
    {
      printf("Succeeded to change dynamixel ID!\n");
    }
    sleep(1); // wait for data to be written
    // read Dynamixel ID to check if it is OK
    dxl_comm_result = packetHandler->read1ByteTxRx(portHandler, desired_id, ADDR_ID, (uint8_t*)&dxl_id, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
      printf("Failed to read new ID!\n");
    }
    else
    {
      printf("New ID is %d.\n", dxl_id);
    }   
  }
  else if (!foundTarget)
  {
    printf("Couldn't find target dynamixel.\n");
  }
  else if (idConflict)
  {
    printf("There is another Dynamixel in the bus with the desired ID.\n");
  }

 

  // // Enable Dynamixel Torque
  // dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  // if (dxl_comm_result != COMM_SUCCESS)
  // {
  //   packetHandler->printTxRxResult(dxl_comm_result);
  // }
  // else if (dxl_error != 0)
  // {
  //   packetHandler->printRxPacketError(dxl_error);
  // }
  // else
  // {
  //   printf("Dynamixel has been successfully connected \n");
  // }


  // while(1)
  // {
  //   printf("Press any key to continue! (or press ESC to quit!)\n");
  //   if (getch() == ESC_ASCII_VALUE)
  //     break;

  //   // Write goal position
  //   dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
  //   if (dxl_comm_result != COMM_SUCCESS)
  //   {
  //     packetHandler->printTxRxResult(dxl_comm_result);
  //   }
  //   else if (dxl_error != 0)
  //   {
  //     packetHandler->printRxPacketError(dxl_error);
  //   }

  //   do
  //   {
  //     // Read present position
  //     dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
  //     if (dxl_comm_result != COMM_SUCCESS)
  //     {
  //       packetHandler->printTxRxResult(dxl_comm_result);
  //     }
  //     else if (dxl_error != 0)
  //     {
  //       packetHandler->printRxPacketError(dxl_error);
  //     }

  //     printf("[ID:%03d] GoalPos:%03d  PresPos:%03d\n", DXL_ID, dxl_goal_position[index], dxl_present_position);

  //   }while((abs(dxl_goal_position[index] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));

  //   // Change goal position
  //   if (index == 0)
  //   {
  //     index = 1;
  //   }
  //   else
  //   {
  //     index = 0;
  //   }
  // }

  // // Disable Dynamixel Torque
  // dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
  // if (dxl_comm_result != COMM_SUCCESS)
  // {
  //   packetHandler->printTxRxResult(dxl_comm_result);
  // }
  // else if (dxl_error != 0)
  // {
  //   packetHandler->printRxPacketError(dxl_error);
  // }

  // Close port
  portHandler->closePort();

  return 0;
}
