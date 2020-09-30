#ifndef ANALYSISCPP_H
#define ANALYSISCPP_H

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <gsl/gsl_vector.h>
#include <sstream>

#include <cpp/plot.h>
#include <cpp/mdsplusAccess.h>
#include <cpp/Kirana.h>
#include <cpp/magnetic.h>
#include <cpp/save.h>
#include <cpp/LightField.h>
#include <cpp/fit.h>
#include <cpp/plasmaParam.h>

int plotPostShotAnalysis();

#endif
