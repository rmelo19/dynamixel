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
  dxls.printInfo(HEADER_INFO);
  // dxls.enableTorqueALL(portHandler, packetHandler);

  // int index = 0;
  // uint32_t dxl_goal_position[2] = {DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};  // Goal position


  // zeroing
  printf("Zeroing position\n");
  dxls.setPositionALL(DXL_MINIMUM_POSITION_VALUE);

  // // dxls.setPosition(portHandler, packetHandler, index, dxl_goal_position[1]);
  // // dxls.printInfo(MOVING_INFO);

  // // dxls.setPosition(portHandler, packetHandler, index, dxl_goal_position[1]);
  // // dxls.printInfo(MOVING_INFO);

  // // sleep(2);
  // dxls.printInfo(POSITION_INFO);



  // printf("Maximizing position\n");
  // dxls.setPositionALL(DXL_MAXIMUM_POSITION_VALUE);

  // dxls.setPosition(portHandler, packetHandler, index, dxl_goal_position[1]);
  // dxls.printInfo(MOVING_INFO);
  // dxls.setPosition(portHandler, packetHandler, index + 1, dxl_goal_position[1]);
  // dxls.printInfo(MOVING_INFO);

  // dxls.printInfo(POSITION_INFO);

  // dxls.disableTorqueALL();
  // Close port
  portHandler->closePort();

  return 0;
}
