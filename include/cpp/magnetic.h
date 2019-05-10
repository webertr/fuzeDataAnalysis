#ifndef CPP_MAGNETIC_H
#define CPP_MAGNETIC_H

#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_sf_trig.h>

#include "mdsplusAccess.h"
#include "plot.h"

int testMagnetic();
gsl_matrix *get8AzimuthalArray(int shotNumber, std::string nodeName);
int getAzimuthalArrayModes(gsl_matrix *mIn);
gsl_matrix *getOffAxisDisplacement(gsl_matrix *mIn);
gsl_vector *getM0Mode(int shotNumber, std::string nodeName);
gsl_vector *getM1Mode(int shotNumber, std::string nodeName);

#endif
