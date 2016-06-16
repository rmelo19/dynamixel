/*
 * move_fingers.cpp
 *
 *  Created on: 2016. 6. 9. 
 *      Author: Rodrigo Marques
 */

#include <dynamixels.h>
#define PROTOCOL_VERSION  1

int main(int argc, char* argv[])
{
  // Creating dynamixels objects
  char deviceName1[13] = "/dev/ttyUSB0"; 
  dynamixels dxls1(deviceName1, 1);
  dxls1.printInfo(HEADER_INFO);

  char deviceName2[13] = "/dev/ttyUSB1";
  dynamixels dxls2(deviceName2, 2);
  dxls2.printInfo(HEADER_INFO);
  sleep(1); 
  dxls2.enableTorqueALL();

  printf("Press any key to close! (or press ESC to quit!)\n");
  // if (getch() == ESC_ASCII_VALUE)
  //   return 0;
  dxls2.printInfo(POSITION_INFO); 

  int i = 0;
  // sleep(5);
  while(i != 1)
  {
    dxls1.setPosition(0, 3000 );
    dxls2.setPosition(0, 4095);
    dxls2.setPosition(1, 4095);

    printf("Press any key to go back to zero position! (or press ESC to quit!)\n");
    // if (getch() == ESC_ASCII_VALUE)
    //   break;
    sleep(2);
    dxls2.printInfo(POSITION_INFO);
    
    dxls1.setPosition(0, 0);
    dxls2.setPosition(0, 0);
    dxls2.setPosition(1, 0);
    

    printf("Press any key to close! (or press ESC to quit!)\n");
    // if (getch() == ESC_ASCII_VALUE)
    //   break;
    sleep(2);
    dxls2.printInfo(POSITION_INFO);
    i++;
  }

  dxls2.disableTorqueALL();
  // Close port
  dxls1.dxl_portHandler->closePort();

  return 0;
}
