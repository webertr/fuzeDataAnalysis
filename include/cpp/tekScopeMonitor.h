#ifndef CPP_TEKSCOPEMONITOR_H
#define CPP_TEKSCOPEMONITOR_H

#include <fstream>
#include <sstream>

#include "epicsCA.h"
#include "mdsplusAccess.h"
#include "serialTekScopeComm.h"

int runTekScopeMonitor();
bool testTekScopeMonitor();

#endif
