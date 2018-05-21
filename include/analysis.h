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
#include "DHI/imageAnalysis.h"
#include "magnetic.h"
#include "plasmaParam.h"
#include "save.h"
#include "getLFData.h"
#include "fit.h"
#include "accelTrack.h"
#include "DHI/holoParam.h"

int plotPostShotAccelData(int shotNumber);
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
int plotPostAnalysis();
int plotPostShotModeData(int shotNumber);
int plotOffAxisDisplacement(int shotNumber);
int invertFlatTopProfile();


/*
 * This is the struct that will hold the default values,
 * static means it should only be visible to the source file
 * that includes it, so the compiler won't complain when linking, I think?
 */

/* 1 is using meters, 100 if using CM */
#define CM_ADJUST 100

static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT = {
  .res = 3.85E-6*CM_ADJUST, 
  .lambda = 532E-9*CM_ADJUST,
  .d = 0.35*CM_ADJUST,
  .deltaX = 0.000115*CM_ADJUST,
  .deltaY = 0.000115*CM_ADJUST,
  .zPosition = 0.140*CM_ADJUST,
  .R_electrode = 0.100838*CM_ADJUST,
  .deltaN = 1E21/(CM_ADJUST*CM_ADJUST*CM_ADJUST),
  .c = 2.998E8*CM_ADJUST,
  .e0 = 8.854e-12/(CM_ADJUST*CM_ADJUST*CM_ADJUST),
  .q = 1.602e-19,
  .me = 9.109E-31,
  .Z = 2,
  .numRows = 4000,
  .numCols = 6016,
  .hyperbolicWin = 8,
  .sampleInterval = 10,
  .centroidNum = 10,
  .offsetIter = 10,
  .boxCarSmoothWidth = 10,
  .unwrapThresh = 1.0*M_PI,
  .signTwin = 1,
  .rotateImage = 1,
  .flipImageRows = 0,
  .flipImageCols = 0,
  .refSubtract = 1,
  .axialCorrection = 0,
  .convertDensity = 1,
  //.fileRef = "/home/fuze/DHI_Images/Calibration/May_16_2018/DSC_0006.JPG",
  //.filePlasma = "/home/fuze/DHI_Images/Calibration/May_16_2018/DSC_0007.JPG",
  .fileRef = "/home/fuze/DHI_Images/180516/180516014_Baseline.JPG",
  .filePlasma = "/home/fuze/DHI_Images/180516/180516014.JPG",
  .xLL = 2764,
  .yLL = 2452,
  .xUR = 3845,
  .yUR = 4911,
  .saveHologram = 1,
  .fileHologram = "data/hologram.dat",
  .saveWrappedPhase = 1,
  .fileWrappedPhase = "data/wrappedPhase.dat",
  .saveLineInt = 1,
  .fileLineIntPos = "data/lineIntegratedPosition.dat",
  .fileLineInt = "data/lineIntegrated.dat",
  .fileLineIntText = "data/lineIntegrated.txt",
  .invertImage = 0,
  .fileLeftInvert = "data/leftAbelInvert.txt",
  .fileRightInvert = "data/rightAbelInvert.txt",
  .fileFullInvert = "data/fullAbelInvert.dat",
  .fileFullInvertPos = "data/fullAbelInvertPosition.dat",
  .fileFullInvertText = "data/fullAbelInvert.txt",
  .fileCentroid = "data/centroid.txt"
};

#endif
