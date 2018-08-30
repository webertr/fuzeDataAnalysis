#ifndef NEUTRON_PRODUCTION_H
#define NEUTRON_PRODUCTION_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <math.h>
#include <mdslib.h>
#include <mdsshr.h>

#include "plot.h"
#include "DHI/mdsplusAccess.h"

double getNeutronProduction(gsl_vector *density, gsl_vector *temperature, gsl_vector *radius,
			    int maxIndex, double Lp, double tauPulse);
int testNeutronProduction();

#endif
