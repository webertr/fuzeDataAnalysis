#ifndef PLOT_H
#define PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

int plotImageData (gsl_matrix *imageIn);
int plotVectorData (gsl_vector *xVecIn, gsl_vector *yVecIn);
int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In);

#endif
