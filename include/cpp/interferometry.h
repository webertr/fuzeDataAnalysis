#ifndef CPP_INTERFEROMETRY_H
#define CPP_INTERFEROMETRY_H

#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_statistics_double.h>

#include "mdsplusAccess.h"
#include "plot.h"

gsl_vector *getIFDensity(int shotNumber, int shotNumberRef, int chordNum);
bool testInterferometry();

#endif
