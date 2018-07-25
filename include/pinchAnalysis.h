#ifndef PINCH_ANALYSIS_H
#define PINCH_ANALYSIS_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf_exp.h>
#include <sys/stat.h>

#include "plot.h"
#include "magnetic.h"
#include "plasmaParam.h"
#include "save.h"

int pinchAnalysis();


#endif
