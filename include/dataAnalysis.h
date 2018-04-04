#ifndef GETSVFFILE_H
#define GETSVFFILE_H

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

int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn);
int initializeMagneticData(int shotNumber, char *nodeName, gsl_vector **data,
			    gsl_vector **time);
int saveVectorData(gsl_vector *xVec, gsl_vector *yVec, char *fileName);

#endif
