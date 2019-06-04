#ifndef CPP_SPECTROSCOPYMONITOR_H
#define CPP_SPECTROSCOPYMONITOR_H

#include <fstream>
#include <sstream>

#include "epicsCA.h"
#include "LightField.h"
#include "mdsplusAccess.h"
#include "psqlAccess.h"

int runSpectroscopyMonitor();
bool testSpectroscopyMonitor();

#endif
