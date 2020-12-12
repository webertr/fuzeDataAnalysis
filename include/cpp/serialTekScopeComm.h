#ifndef CPP_SERIALTEKSCOPECOMM_H
#define CPP_SERIALTEKSCOPECOMM_H

#include <SerialPort.h>
#include <SerialStream.h>
#include <iostream>
#include <sstream>
#include <gsl/gsl_vector.h>

namespace SerialTekScopeComm {
  
  gsl_vector *getScopeChData(LibSerial::SerialPort *serialPort, int chNum);
  bool testSerialTekScopeComm();
  gsl_vector *getScopeTimeBase(LibSerial::SerialPort *serialPort, int chNum);
  
}

#endif
