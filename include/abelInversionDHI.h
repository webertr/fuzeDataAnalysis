#ifndef ABELINVERSIONDHI_H
#define ABELINVERSIONDHI_H

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
#include <gsl/gsl_sort.h>
#include <gsl/gsl_wavelet.h>

#include "holoParam.h"


gsl_matrix* getInvertedMatrixLeft(int sizeM, double res);
gsl_matrix* getInvertedMatrixRight(int sizeM, double res);
int findDensityOffset(gsl_vector* smallCrossSection, gsl_vector* largeCrossSection,
		      holographyParameters* param);
int solveRightSystemLinearEq(gsl_matrix* mInput, gsl_vector* vInput,
			     gsl_vector* vOutput, int rightSize);
int solveLeftSystemLinearEq(gsl_matrix* mInput, gsl_vector* vInput,
			    gsl_vector* vOutput, int leftSize);
gsl_matrix* getProjectMatrix(int sizeM, double res);
int axialVariationCorrection(gsl_matrix *leftDensityProfile, gsl_matrix *rightDensityProfile,
			     gsl_matrix *imageM, gsl_vector *centroidLocation,
			     holographyParameters* param);
int getRadialDensityProfile(gsl_vector* leftCrossSection, gsl_vector* rightCrossSection, 
			    gsl_vector* crossSection, gsl_vector* centroidLocation,
			    gsl_matrix* projectMatrix,
			    int centroidIterations,  int centroidIndexTest,
			    int colNumber, holographyParameters* param);

#endif
