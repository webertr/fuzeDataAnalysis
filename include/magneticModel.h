#ifndef MAGNETIC_MODEL_H
#define MAGNETIC_MODEL_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf_exp.h>
#include <sys/stat.h>
#include <gsl/gsl_sf_trig.h>

#include "plot.h"
#include "save.h"

int magneticModel();
int testMagneticModel();

#endif
