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

/* 
 * A structure to pass around the necessary parameters
 * for hologram reconstruction and inversion
 */
struct datavector { 
  double deltaT;                              // Time step for each data point
  int length;                                 // Number of points
  gsl_vector *data;                           // Pointer to gsl_vector containg raw data
  int shotNumber;                             // The shot number
  int (*getData)(struct datavector*);         // Function pointer to getData function
  int (*getElement)(struct datavector*, int);  // Function to get element from vector
};

typedef struct datavector dataVector;

int getVectorData(int shotNumber);
dataVector* initializeMagneticData (int shotNumber, char *nodeName);

#endif
