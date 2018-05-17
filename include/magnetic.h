#ifndef MAGNETIC_H
#define MAGNETIC_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <mdslib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <gsl/gsl_fft_real.h>

#include "plot.h"


int initializeMagneticDataAndTime(int shotNumber, char *nodeName, gsl_vector *data,
				  gsl_vector *time);
int initializeMagneticData(int shotNumber, char *nodeName, gsl_vector *data);
int getSignalLengthMDSplus(const char *signal, int shotNumber);
gsl_matrix *getAzimuthalArray(int shotNumber, char *nodeName);
int getAzimuthalArrayModes(gsl_matrix *mIn);
int getCurrentPulseNumber();
int plotPostShotModeData();
int testMagnetic();


#endif
