#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <string.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf_exp.h>

int testBoxCarSmooth();
int unwrapTest();
gsl_matrix *testReadRawFileHol (char * filename, int numRows, int numCols);
void testSmoothData();
gsl_vector *testMatrixMult(gsl_matrix *mInput, gsl_vector *vInput);
gsl_matrix *getTestData();
void saveVectorTest(gsl_vector *vecIn, char *fileName);
int saveRadialProfileTest(char *fileLeftMatrix, char *fileRightMatrix,
			  int numRows, int numCols,
			  int colNum, char *fileLeftProfile,
			  char *fileRightProfile);
int testWaveletFit();
int testPutVerticalLine(gsl_matrix* mInput, int colNumber, int width);
int testPutHorizontalLine(gsl_matrix* mInput, int rowNumber, int width);
int testWaveletFitRow(gsl_vector *vecIn, int nc);
gsl_matrix *testWaveletGetFit(char *fileName, int nc);
int testAsymetricProjectionGenerate();
int testGetDensitySlice(gsl_vector *forwardProjectData, int numRows, int numCols);
gsl_matrix *getRadialProfileImageTest(char *fileLeftMatrix, char *fileRightMatrix,
				      char *fileNameCentroid,
				      int numRows, int numCols);
int testModifyJPEG();
int testGetMaxIndexComplex(gsl_vector_complex *vIn);
int testMagneticData();

#endif
