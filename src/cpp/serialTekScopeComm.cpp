#include <SerialPort.h>
#include <SerialStream.h>
#include <iostream>
#include <sstream>


/******************************************************************************
 *
 * This file includes function to communicate via a serial port and
 * RS285 protocol. It's using
 * https://libserial.readthedocs.io/en/latest/index.html
 * https://github.com/crayzeewulf/libserial
 * sudo apt-get install libserial-dev
 * We want to communicate with a Tektronix TPS 2024B scope
 *
 ******************************************************************************/

static int initSerialPort(LibSerial::SerialPort *serialPort);
static std::string getID(LibSerial::SerialPort *serialPort);
static int setString(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		     std::string setStringVal);
static int setInt(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		  int intVal);
static float getFloat(LibSerial::SerialPort *serialPort, std::string setStringCmd);


/******************************************************************************
 * Function: getID
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 for it's ID
 ******************************************************************************/

static std::string getID(LibSerial::SerialPort *serialPort) {

  std::string idString = "ID?\n";
  serialPort->Write(idString);

  char nextChar;
  std::string idRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(idRet, '\n', timeoutMS);

  return idRet;

}

/******************************************************************************
 * Function: initSerialPort
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, initialize the serial
 * port to the required specifications for our application with the TPS 2024B
 ******************************************************************************/

static int initSerialPort(LibSerial::SerialPort *serialPort) {

  serialPort->SetBaudRate(LibSerial::BaudRate::BAUD_9600);

  serialPort->SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  
  //serialPort.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_HARDWARE);
  serialPort->SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);

  serialPort->SetParity(LibSerial::Parity::PARITY_NONE);

  serialPort->SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  return 0;
   
}

/******************************************************************************
 * Function: setDataEncode
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and set a string
 ******************************************************************************/

static int setString(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		     std::string setStringVal) {

  
  serialPort->Write(setStringCmd + " " + setStringVal + "\n");

  serialPort->Write(setStringCmd + "?\n");

  char nextChar;
  std::string setStringRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(setStringRet, '\n', timeoutMS);
  setStringVal.append("\n");
  
  if (setStringRet.compare(setStringVal) != 0) {
      std::cout << "Error in setting string\n"
		<< "Expected " << setStringVal
		<< " but got " << setStringRet;
      return -1;
  }
    
  return 0;

}


/******************************************************************************
 * Function: setInt
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and set an int value
 ******************************************************************************/

static int setInt(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		  int intVal) {
  
  // Setting value
  serialPort->Write(setStringCmd + " " + std::to_string(intVal) + "\n");

  // Commanding readback
  serialPort->Write(setStringCmd + "?\n");

  char nextChar;
  std::string setStringRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(setStringRet, '\n', timeoutMS);
  int intValRet = std::stoi(setStringRet);
  
  if (intValRet != intVal) {
      std::cout << "Error in setting int\n"
		<< "Expected " << intVal
		<< " but got " << intValRet;
      return -1;
  }
    
  return 0;

}


/******************************************************************************
 * Function: getFloat
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and get a float value
 ******************************************************************************/

static float getFloat(LibSerial::SerialPort *serialPort, std::string setStringCmd) {
  
  // Commanding readback
  serialPort->Write(setStringCmd + "?\n");

  char nextChar;
  std::string setStringRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(setStringRet, '\n', timeoutMS);

  std::istringstream os(setStringRet);
  
  float floatValRet = std::stoi(setStringRet);

  os >> floatValRet;
  
  return floatValRet;

}


/******************************************************************************
 * Function: main
 * Inputs: 
 * Returns: int
 * Description: This executes the main function in this file.
 ******************************************************************************/

int main() {

  
  LibSerial::SerialPort serialPort("/dev/ttyUSB0");

  // Initializing the port
  initSerialPort(&serialPort);

  // Printing the ID
  std::cout << getID(&serialPort);

  setString(&serialPort, "DATA:ENCDG", "RIBINARY");

  setInt(&serialPort, "DATA:WIDTH", 2);
  setInt(&serialPort, "DATA:WIDTH", 2);

  float yMult = getFloat(&serialPort, "WFMPre:YMUlt");
  float yOffset = getFloat(&serialPort, "WFMPre:YOFf");
  float yZero = getFloat(&serialPort, "WFMPre:YZEro");

  

  return 0;
  
  // serialPort.Read(nextChar);
  // while (nextChar != '\n') {
  //   serialPort.Read(nextChar);
  
  // // Read one character from the serial port.
  // //serialPort >> nextChar;

  // // You can also read other types of values from the serial port in a similar fashion.
  // //int dataSize;
  // //serialPort >> dataSize;


  
  // // Write a single character to the serial port.
  // my_serial_port.WriteByte( 'U' );
  // serialPort << 'U' ;

  // // You can easily write strings.
  // std::string my_string = "Hello, Serial Port.";

  // serialPort.Write( my_string );
  // serialPort << my_string << std::endl ;

  // // And, with serial stream objects, you can easily write any type
  // // of object that is supported by a "<<" operator.
  // double radius = 2.0 ;
  // double area = M_PI * 2.0 * 2.0 ;
  
  // serialPort << area << std::endl ;


  
  // // Read a whole array of data from the serial port.
  // const int BUFFER_SIZE = 256;
  // char input_buffer[BUFFER_SIZE];

  // my_serial_port.Read( input_buffer, BUFFER_SIZE );
  // serialPort.read( input_buffer, BUFFER_SIZE );


  serialPort.Close();
  
  return 0;
	 
}
