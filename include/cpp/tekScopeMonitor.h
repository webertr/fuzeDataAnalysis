#ifndef CPP_TEKSCOPEMONITOR_H
#define CPP_TEKSCOPEMONITOR_H

#include <fstream>
#include <sstream>
#include <unistd.h>


#include "epicsCA.h"
#include "mdsplusAccess.h"
#include "serialTekScopeComm.h"
#include "save.h"

int runTekScopeMonitor();
bool testTekScopeMonitor();

#endif
