#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


int saveVectorData(gsl_vector *vecIn, char *fileName);
int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, char *fileName);
int save5VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, char *fileName);
int saveMatrixData(gsl_matrix *mIn, char *fileName);
int saveImageData(gsl_matrix *mInput, char *fileName);
int readImageData(gsl_matrix *mInput, char *fileName);

#endif
