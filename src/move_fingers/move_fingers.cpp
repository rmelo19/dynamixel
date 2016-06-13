/*
 * move_fingers.cpp
 *
 *  Created on: 2016. 6. 9. 
 *      Author: Rodrigo Marques
 */
 
#include <dynamixels.h>

int main(int argc, char* argv[])
{
  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

  // Initialize PacketHandler instance
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);


  dynamixels dxls(portHandler, packetHandler);
  dxls.enableTorqueALL();

  int index = 0;
  int dxl_goal_position[2] = {DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};  // Goal position
  int32_t dxl_present_position = 0;                                                     // Present position

  dxls.setPosition(index, dxl_goal_position[0]);
  dxls.readPosition(index);
  dxls.printInfo(POSITION_INFO);

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
