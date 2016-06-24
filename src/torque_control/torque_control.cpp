/*
 * torque_control.cpp
 *
 *  Created on: 2016. 6. 9. 
 *      Author: Rodrigo Marques
 */

#include <dynamixels.h>

int main(int argc, char* argv[])
{
  // Creating dynamixels objects
  // char deviceName1[13] = "/dev/ttyUSB0"; 
  // dynamixels dxls1(deviceName1, 1);
  // dxls1.printInfo(HEADER_INFO);

  char deviceName2[13] = "/dev/ttyUSB1";
  dynamixels dxls2(deviceName2, 2);
  dxls2.printInfo(HEADER_INFO);
  // setCurrentLimit(i, MAXIMUM_CURRENT);
  sleep(1); 
  dxls2.setOperatingModeALL(TORQUE_CONTROL_MODE);
  dxls2.enableTorqueALL();

  sleep(1);
  int j = 0;
  while(j != 1)
  {
    dxls2.setPosition(0, 200);
    dxls2.setPosition(1, 200);

    dxls2.setCurrent(0, 20);
    dxls2.setCurrent(0, 20);
    

    while(1)
    {
      dxls2.printInfo(DETAILS_INFO);
    }

    j++;
  }

  return 0;
}
