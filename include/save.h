#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, char *fileName);

#endif
