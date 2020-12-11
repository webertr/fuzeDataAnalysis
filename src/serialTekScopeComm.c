// C library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


static int getID(int serialPort);
static int initSerialPort(int *serialPortRet, struct termios *ttyRet);
static int *getChData(int serialPort, int channelNumber, int *numPointsRet);


/******************************************************************************
 * Function: initSerialPort
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, initialize the serial
 * port to the required specifications for our application with the TPS 2024B
 ******************************************************************************/

static int initSerialPort(int *serialPortRet, struct termios *ttyRet) {

  /* 
   * Open the serial port. Change device path as needed 
   * (currently set to an standard FTDI USB-UART cable type device)
   */
  int serialPort = open("/dev/ttyUSB0", O_RDWR);

  // Create new termios struc, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serialPort, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~PARENB;

  // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSTOPB;

  // Clear all bits that set the data size 
  tty.c_cflag &= ~CSIZE;

  // 8 bits per byte (most common)
  tty.c_cflag |= CS8;

  // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag &= ~CRTSCTS;

  // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  tty.c_cflag |= CREAD | CLOCAL; 

  tty.c_lflag &= ~ICANON;

  // Disable echo
  tty.c_lflag &= ~ECHO;

  // Disable erasure
  tty.c_lflag &= ~ECHOE;

  // Disable new-line echo
  tty.c_lflag &= ~ECHONL;
  
  // Disable interpretation of INTR, QUIT and SUSP
  tty.c_lflag &= ~ISIG;

  // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  // Disable any special handling of received bytes
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~OPOST; 

  // Prevent conversion of newline to carriage return/line feed
  tty.c_oflag &= ~ONLCR;

  // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~OXTABS;

  // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT;

  // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VTIME] = 10;
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Set values of pointers equal to variables in this scope.
  // Variables will not be destroyed this way
  *serialPortRet = serialPort;
  *ttyRet = tty;

  return 0;

}


/******************************************************************************
 * Function: getID
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 for it's ID
 ******************************************************************************/

static int getID(int serialPort) {

  // Clearing the device
  unsigned char msgClr[] = { '*', 'C', 'L', 'S', '\n' };
  write(serialPort, msgClr, sizeof(msgClr));
  
  // Sending the ID query
  unsigned char msgID[] = { 'I', 'D', '?', '\n' };
  write(serialPort, msgID, sizeof(msgID));

  char readChar;
  char readCharArray[256];
  
  // Reading from the scope one byte at a time. Should stop with a '\n'
  read(serialPort, &readChar, 1);
  int ii = 0;
  while (readChar != '\n') {
    readCharArray[ii] = readChar;    
    read(serialPort, &readChar, 1);
    ii = ii + 1;
  }
  readCharArray[ii] = '\n';
  readCharArray[ii+1] = '\0';
  
  printf("Received ID response: %s", readCharArray);

  return 0;
  
}

/******************************************************************************
 * Function: getChData
 * Inputs: int
 * Returns: int
 * Description: Assuming a serial port is opened, this will poll the
 * Tektronix TPS 2024B scope via RS232 for it's channel data
 ******************************************************************************/

static int *getChData(int serialPort, int channelNumber, int *numPointsRet) {

  char readChar;
  char readCharArray[1024];
  char writeCharArray[1024];


  // Setting the chanel number
  snprintf(writeCharArray, sizeof(writeCharArray), "DATA:SOURCE CH%d", channelNumber);

  unsigned char setChNum[16];
  int ii;
  for (ii = 0; ii < 15; ii++) {
    setChNum[ii] = writeCharArray[ii];
  }
  setChNum[ii] = '\n';
  
  write(serialPort, setChNum, sizeof(setChNum));

  // Setting the data width. Signed range should be 0 to 256. Only 1 byte of data
  unsigned char dataWidth[] = { 'D', 'A', 'T', 'A', ':', 'W', 'I', 'D',
				'T', 'H', ' ', '1', '\n' };
  write(serialPort, dataWidth, sizeof(dataWidth));

  // Setting data encode DATA:ENCDG RIB\n
  unsigned char dataEncode[] = { 'D', 'A', 'T', 'A', ':', 'E', 'N', 'C',
				 'D', 'G', ' ', 'R', 'I', 'B', '\n' };
  write(serialPort, dataEncode, sizeof(dataEncode));

  // Checking dataEncode "DATA:ENCDG?\n"
  unsigned char dataEncodeCheck[] = { 'D', 'A', 'T', 'A', ':', 'E', 'N', 'C',
				      'D', 'G', '?', '\n' };
  write(serialPort, dataEncodeCheck, sizeof(dataEncodeCheck));
  read(serialPort, &readChar, 1);
  ii = 0;
  while (readChar != '\n') {
    readCharArray[ii] = readChar;    
    read(serialPort, &readChar, 1);
    ii = ii + 1;
  }
  readCharArray[ii] = '\n';
  readCharArray[ii+1] = '\0';
  unsigned char dataEncodeTest[] = { 'R', 'I', 'B', 'I', 'N', 'A', 'R', 'Y', '\n' };  
  for (ii = 0; ii < 9; ii++) {
    if (dataEncodeTest[ii] != readCharArray[ii]) {
      printf("Data encode error. Expected RIBINARY but got %s\n", readCharArray);
      break;
    }
  }

  // WFMPre:YMUlt?\n
  
  // Getting the number of points to read from the scope
  unsigned char wfNumPoints[] = { 'W', 'F', 'M', 'P', 'r', 'e', ':', 'N',
				  'R', '_', 'P', 't', '?', '\n' };
  write(serialPort, wfNumPoints, sizeof(wfNumPoints));
  
  read(serialPort, &readChar, 1);
  ii = 0;
  while (readChar != '\n') {
    readCharArray[ii] = readChar;    
    read(serialPort, &readChar, 1);
    ii = ii + 1;
  }
  readCharArray[ii] = '\n';
  readCharArray[ii+1] = '\0';
  
  // Converting string to integer
  int numPoints;
  sscanf(readCharArray, "%d", &numPoints);

  // Allocating array to hold int data
  int *data = (int *) malloc(numPoints * sizeof(int));

  // Allocating array to hold char data
  char *readDataArray = (char *) malloc(numPoints * sizeof(char));
    
  // Selecting channel and data type

  

  
  // Getting data from scope
  unsigned char getCurve[] = { 'C', 'U', 'R', 'V', 'E', '?', '\n' };
  write(serialPort, getCurve, sizeof(wfNumPoints));

  // Reading some preample that is not data
  for (ii = 0; ii < 6; ii++){
    read(serialPort, &readChar, 1);
  }

  // Starting read of data
  for (ii = 0; ii < numPoints; ii++) {
    read(serialPort, &readChar, 1);
    readDataArray[ii] = readChar;
  }

  // Reading '\n' character
  read(serialPort, &readChar, 1);

  // Writing data to the data array created earlier
  for (ii = 0; ii < numPoints; ii++) {
    data[ii] = readDataArray[ii];
  }

  // Freeing array created earlier
  free(readDataArray);

  // Setting number of points read
  *numPointsRet = numPoints;

  return data;
  
}
  
int main() {

  int serialPort;
  struct termios tty;
  
  initSerialPort(&serialPort, &tty);
  
  getID(serialPort);

  int *data;
  int numPoints;
  data = getChData(serialPort, 1, &numPoints);

  free(data);
  
  close(serialPort);
  
  return 0;
	 
}
