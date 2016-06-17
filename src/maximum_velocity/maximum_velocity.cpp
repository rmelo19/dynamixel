/*
 * maximum_velocity.cpp
 *
 *  Created on: 2016. 6. 9. 
 *      Author: Rodrigo Marques
 */

#include <dynamixels.h>

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

  int j = 0;
  while(j != 1)
  {
    dxls1.setPosition(0, 3000 );
    dxls2.setPosition(0, 4095);
    dxls2.setPosition(1, 4095);
    
    while(!dxls2.isIdle());
    
    dxls1.setPosition(0, 0);
    dxls2.setPosition(0, 0);
    dxls2.setPosition(1, 0);

    while(!dxls2.isIdle());

    j++;
  }

  return 0;
}
