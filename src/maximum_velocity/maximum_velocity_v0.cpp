/*
 * move_fingers.cpp
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

  printf("Press any key to close! (or press ESC to quit!)\n");
  // if (getch() == ESC_ASCII_VALUE)
  //   return 0;
  dxls2.printInfo(POSITION_INFO); 

  int j = 0;
  // sleep(5);
  while(j != 1)
  {
    dxls1.setPosition(0, 3000 );
    dxls2.setPosition(0, 4095);
    dxls2.setPosition(1, 4095);

    // printf("Press any key to go back to zero position! (or press ESC to quit!)\n");
    // if (getch() == ESC_ASCII_VALUE)
    //   break;
    
    bool moving = true;
    dxls2.readDataALL(MOVING_STATUS_DATA);
    for (int i = 0; i < dxls2.qtdDyn; i++)
    {
      moving = moving || !((bool) (dxls2.movingStatus[i] & 0b00000001));
    }
    while (moving)
    {
      dxls2.printInfo(DETAILS_INFO);  
      dxls2.readDataALL(MOVING_STATUS_DATA);
      moving = !((bool) (dxls2.movingStatus[0] & (uint8_t) 1));
      for (int i = 1; i < dxls2.qtdDyn; i++)
      {
        moving = moving || !((bool) (dxls2.movingStatus[i] & 0b00000001));;
      }
    }

    // printf("1 some not moving\n");
    // bool notmoving = false;
    // dxls2.readDataALL(MOVING_DATA);
    // for (int i = 0; i < dxls2.qtdDyn; i++)
    // {
    //   notmoving = notmoving || !dxls2.isMoving[i];
    // }
    // while (notmoving)
    // {
    //   dxls2.printInfo(DETAILS_INFO);  
    //   dxls2.readDataALL(MOVING_DATA);
    //   for (int i = 0; i < dxls2.qtdDyn; i++)
    //   {
    //     notmoving = notmoving || !dxls2.isMoving[i];
    //   }
    //   printf("1 some not moving\n");
    // }

    // printf("1 all moving\n");
    // bool moving = true;
    // dxls2.readDataALL(MOVING_DATA);
    // for (int i = 0; i < dxls2.qtdDyn; i++)
    // {
    //   moving = moving || dxls2.isMoving[i];
    // }
    // while (moving)
    // {
    //   dxls2.printInfo(DETAILS_INFO);  
    //   dxls2.readDataALL(MOVING_DATA);
    //   for (int i = 0; i < dxls2.qtdDyn; i++)
    //   {
    //     moving = moving || dxls2.isMoving[i];
    //   }
    //   printf("1 all moving\n");
    // }
    
    dxls1.setPosition(0, 0);
    dxls2.setPosition(0, 0);
    dxls2.setPosition(1, 0);
    

    // printf("Press any key to close! (or press ESC to quit!)\n");
    // if (getch() == ESC_ASCII_VALUE)
    //   break;

    moving = true;
    dxls2.readDataALL(MOVING_STATUS_DATA);
    for (int i = 0; i < dxls2.qtdDyn; i++)
    {
      moving = moving || !((bool) (dxls2.movingStatus[i] & (uint8_t) 1));
    }
    while (moving)
    {
      dxls2.printInfo(DETAILS_INFO);  
      dxls2.readDataALL(MOVING_STATUS_DATA);
      moving = !((bool) (dxls2.movingStatus[0] & (uint8_t) 1));
      for (int i = 1; i < dxls2.qtdDyn; i++)
      {
        moving = moving || !((bool) (dxls2.movingStatus[i] & (uint8_t) 1));
      }
      // printf("2 all moving\n");
    }




    // printf("2 some not moving\n");
    // notmoving = false;
    // dxls2.readDataALL(MOVING_DATA);
    // for (int i = 0; i < dxls2.qtdDyn; i++)
    // {
    //   notmoving = notmoving || !dxls2.isMoving[i];
    // }
    // while (notmoving)
    // {
    //   dxls2.printInfo(DETAILS_INFO);  
    //   dxls2.readDataALL(MOVING_DATA);
    //   for (int i = 0; i < dxls2.qtdDyn; i++)
    //   {
    //     notmoving = notmoving || !dxls2.isMoving[i];
    //   }
    //   printf("some not moving\n");
    // }

    // printf("2 all moving\n");
    // moving = true;
    // dxls2.readDataALL(MOVING_DATA);
    // for (int i = 0; i < dxls2.qtdDyn; i++)
    // {
    //   moving = moving && dxls2.isMoving[i];
    // }
    // while (moving)
    // {
    //   dxls2.printInfo(DETAILS_INFO);  
    //   dxls2.readDataALL(MOVING_DATA);
    //   for (int i = 0; i < dxls2.qtdDyn; i++)
    //   {
    //     moving = moving && dxls2.isMoving[i];
    //   }
    //   printf("2 all moving\n");
    // }

    j++;
  }

  dxls2.disableTorqueALL();
  // Close port
  dxls1.dxl_portHandler->closePort();

  return 0;
}
