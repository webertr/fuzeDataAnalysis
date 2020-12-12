#include "cpp/serialTekScopeComm.h"


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
static std::string writeReadBack(LibSerial::SerialPort *serialPort, std::string cmd);
static int setString(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		     std::string setStringVal);
static int setInt(LibSerial::SerialPort *serialPort, std::string setStringCmd,
		  int intVal);
static float getFloat(LibSerial::SerialPort *serialPort, std::string setStringCmd);
static int getInt(LibSerial::SerialPort *serialPort, std::string setStringCmd);


/******************************************************************************
 * Function: getID
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 for it's ID
 ******************************************************************************/

static std::string writeReadBack(LibSerial::SerialPort *serialPort, std::string cmd) {

  serialPort->Write(cmd + "\n");

  std::string cmdRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(cmdRet, '\n', timeoutMS);

  return cmdRet;

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
  
  serialPort->SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);

  serialPort->SetParity(LibSerial::Parity::PARITY_NONE);

  serialPort->SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  // Clearing
  serialPort->Write("*CLS\n");

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

  std::string setStringRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(setStringRet, '\n', timeoutMS);

  std::istringstream os(setStringRet);
  
  float floatValRet;

  os >> floatValRet;
  
  return floatValRet;

}


/******************************************************************************
 * Function: getInt
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and get a int val
 ******************************************************************************/

static int getInt(LibSerial::SerialPort *serialPort, std::string setStringCmd) {
  
  // Commanding readback
  serialPort->Write(setStringCmd + "?\n");

  std::string setStringRet;
  int timeoutMS = 5000;
  serialPort->ReadLine(setStringRet, '\n', timeoutMS);

  std::istringstream os(setStringRet);
  
  int intValRet;

  os >> intValRet;
  
  return intValRet;

}


/******************************************************************************
 * Function: getChData
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and get a channel data
 ******************************************************************************/

gsl_vector *SerialTekScopeComm::getScopeChData(LibSerial::SerialPort *serialPort,
					       int chNum) {
  
  // Setting channel
  setString(serialPort, "DATA:SOURCE", "CH" + std::to_string(chNum));

  // Setting data format
  setString(serialPort, "DATA:ENCDG", "RIBINARY");
  setInt(serialPort, "DATA:WIDTH", 1);
  
  // Getting information
  float yMult = getFloat(serialPort, "WFMPre:YMUlt");
  float yOffset = getFloat(serialPort, "WFMPre:YOFf");
  float yZero = getFloat(serialPort, "WFMPre:YZEro");
  int numPoints = getInt(serialPort, "WFMPre:NR_Pt");

  gsl_vector *dataVector = gsl_vector_alloc(numPoints);
  
  // Commanding readback
  serialPort->Write("CURVE?\n");

  int timeOutMS = 5000;  
  char nextChar;
  int ii;
  // Reading preample
  for (ii = 0; ii < 6; ii++) {
    serialPort->ReadByte(nextChar, timeOutMS);
  }

  for (ii = 0; ii < numPoints; ii++) {
    serialPort->ReadByte(nextChar, timeOutMS);
    gsl_vector_set(dataVector, ii, (((int) nextChar)*yMult - yOffset) + yZero);
  }

  // Reading "\n"
  serialPort->ReadByte(nextChar, timeOutMS);
  
  return dataVector;

}


/******************************************************************************
 * Function: getTimeData
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 and get a time base
 ******************************************************************************/

gsl_vector *SerialTekScopeComm::getScopeTimeBase(LibSerial::SerialPort *serialPort,
						 int chNum) {
  
  // Setting channel
  setString(serialPort, "DATA:SOURCE", "CH" + std::to_string(chNum));
  
  // Getting information
  float xDelta = getFloat(serialPort, "WFMPre:XINcr");
  int numPoints = getInt(serialPort, "WFMPre:NR_Pt");

  gsl_vector *dataVector = gsl_vector_alloc(numPoints);
  
  int ii;
  for (ii = 0; ii < numPoints; ii++) {
    gsl_vector_set(dataVector, ii, xDelta * ii);
  }

  return dataVector;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

bool SerialTekScopeComm::testSerialTekScopeComm() {
  
  LibSerial::SerialPort serialPort("/dev/ttyUSB0");
  
  // Initializing the port
  initSerialPort(&serialPort);

  // Printing the information
  std::cout << writeReadBack(&serialPort, "WFMpre?");

  // Getting data
  gsl_vector *data = getScopeChData(&serialPort, 4);

  // Getting time base
  gsl_vector *time = getScopeTimeBase(&serialPort, 1);

  std::cout << gsl_vector_get(data, data->size-1) << "\n";
  std::cout << gsl_vector_get(time, time->size-1) << "\n";
  
  // Closing serial port
  serialPort.Close();

  free(data);

  free(time);
  
  return 0;
	 
}
