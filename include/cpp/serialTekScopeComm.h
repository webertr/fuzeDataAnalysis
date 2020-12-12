#ifndef CPP_SERIALTEKSCOPECOMM_H
#define CPP_SERIALTEKSCOPECOMM_H

#include <SerialPort.h>
#include <SerialStream.h>
#include <iostream>
#include <sstream>
#include <gsl/gsl_vector.h>

namespace SerialTekScopeComm {
  
  gsl_vector *getScopeChData(int chNum);
  bool testSerialTekScopeComm();
  gsl_vector *getScopeTimeBase(int chNum);
  
}

#endif
