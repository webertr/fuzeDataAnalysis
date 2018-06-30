#ifndef MDSPLUS_READ_H
#define MDSPLUS_READ_H

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
#include <mdsshr.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_sf_trig.h>
#include <sys/stat.h>

gsl_matrix *readDHIMDSplusImage(int shotNumber, char *nodeName);
gsl_vector *readDHIMDSplusDimension(int shotNumber, char *nodeName, int dimNum);
int mdsplusReadTest();

#endif
