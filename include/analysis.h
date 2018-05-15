#ifndef ANALYSIS_H
#define ANALYSIS_H

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
#include "imageAnalysisDHI.h"
#include "magnetic.h"
#include "plasmaParam.h"
#include "save.h"
#include "getLFData.h"
#include "fit.h"
#include "accelTrack.h"

int plotPostShotData();
int plotIPApril2018Talk(int shotNumber);
int plotAccelApril2018Talk();
int plotModeApril2018Talk();
int plotDHIApril2018Talk();
int plotSpecCIIIImageApril2018Talk();
int hologramAnalysis();
int plotCIIILineApril2018Talk();
int plotModeCompApril2018Talk();
int plasmaParameters();
int simluateAccel();

#endif
