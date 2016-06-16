# Dynamixel Library (under development)

Having to interface with the Dynamixels XM-430 and MX-28T, which uses different protocols (1.0 and 2.0 versions), I have designed a library to make the task of controlling the servomotors easier. It makes use of the SDK provided the Robotis:

[GitHub DynamixelSDK](https://github.com/ROBOTIS-GIT/DynamixelSDK)

### Setup to use the code:
I have developed this code to interface with servomoters connected to a computer running Ubuntu (14.04 LTS). They were connected using a USB-RS485 and a USB-TTL converter.

### Structure:

The structure is simple, in the include folder you will find a header file defining the class Dynamixels. One object of that class should account for all the dynamixels connected to the computer, even if they are connected to the computer using different serial ports.

To initialize a object you have to give the name of the Serial port and the protocol version you want to use. In my tests I had to connect to devices using both protocols using different ports, thats how the code looks like: 

```
  char deviceName1[13] = "/dev/ttyUSB0"; 
  dynamixels dxls1(deviceName1, 1);

  char deviceName2[13] = "/dev/ttyUSB1";
  dynamixels dxls2(deviceName2, 2);
```

#### Printing Information in the Terminal:

To make the debugging task easier there is a method *void dynamixels::printInfo(int info)*, which receives a integer defining the type of information you need. In *dynamixelsDefs.h* I have defined readable constants that can be used to have a better looking code:

```
// PRINTING INFO
#define BAUDRATE_INFO                   1
#define ID_ARRAY_INFO                   2
#define POSITION_INFO                   3
#define HEADER_INFO                     4
#define MOVING_INFO                     5
```

##### Example:
```
  dynamixels dxls("/dev/ttyUSB0", 2);
  dxls.printInfo(HEADER_INFO);
```

### Comments, critiques, advices...
Feel free to contact me: Rodrigo Marques, rmelo19.github@gmail.com
