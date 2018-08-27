#ifndef BANK_CHECK_H
#define BANK_CHECK_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf_exp.h>
#include <sys/stat.h>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_fft_real.h>
#include <mdslib.h>
#include <mdsshr.h>


#include "plot.h"
#include "save.h"

int bankCheck();
int testBankCheck();

#endif
