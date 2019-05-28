#ifndef CPP_EPICSCA_H
#define CPP_EPICSCA_H

#include <iostream>
#include <iomanip>

#include "cadef.h"
#include "dbDefs.h"
#include "epicsString.h"
#include "cantProceed.h"


/* Declaring funcction pointer to be used for callbacks */
typedef void(*epicsCBFuncPointer)(struct event_handler_args eha);

int testEpicsCA();
int monitorLongPVsWithCallback(std::string pvNames[], epicsCBFuncPointer cbFunc[], 
			       const int length);

#endif
