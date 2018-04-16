#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, char *fileName);
int save5VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, char *fileName);

#endif
