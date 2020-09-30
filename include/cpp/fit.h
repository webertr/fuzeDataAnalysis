#ifndef FITCPP_H
#define FITCPP_H

#include <iostream>
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
#include <gsl/gsl_multifit_nlinear.h>

int fitGaussian(gsl_vector *xVec, gsl_vector *yVec, gsl_vector *gaussFit,
		double *amplitude, double *center, double *width,
		double *offset, double *widthError, int printOption);

#endif
