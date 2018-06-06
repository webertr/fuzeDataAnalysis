#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <string.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


int saveVectorData(gsl_vector *vecIn, char *fileName);
int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, char *fileName);
int save5VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, char *fileName);
int saveMatrixData(gsl_matrix *mIn, char *fileName);
int saveImageData(gsl_matrix *mInput, char *fileName);
int readImageData(gsl_matrix *mInput, char *fileName);
int saveImageDataWithPosition(gsl_matrix *mInput, gsl_vector* xVec, gsl_vector* yVec,
			      char *fileName);
int saveVectorBinary(gsl_vector *vecIn, char *fileName);
int save6VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, gsl_vector *vec6In, 
		    char *fileName);
gsl_matrix *readMatrixTextFile(char *fileName);

#endif
