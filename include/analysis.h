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
#include "DHI/forceBalance.h"


int plotPostShotAccelData(int shotNumber, int tmin, int tmax);
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
int plotPostShotModeData(int shotNumber, int tmin, int tmax, char *nodeName, char *saveFile);
int plotOffAxisDisplacement(int shotNumber);
int invertFlatTopProfile();
int flatTopRadialForceBalance();
int plotPostShotNeutronData(int shotNumber, int tmin, int tmax, char *saveFile);
int plotPostShotSymmetryCheck(int shotNumber, int tmin, int tmax);
int plotPostShotIV(int shotNumber, int tmin, int tmax, char *saveFile);
int plotPostShotGVCurrent(int shotNumber, int tmin, int tmax);


/*
 * This is the struct that will hold the default values,
 * static means it should only be visible to the source file
 * that includes it, so the compiler won't complain when linking, I think?
 */

static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT = {
  .res = 3.85E-6, 
  .lambda = 532E-9,
  .d = 0.35,
  .zPosition = 0.140,
  .R_electrode = 0.100838,
  .deltaN = 1E21,
  .c = 2.998E8,
  .e0 = 8.854e-12,
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
  .fileRef = "/home/fuze/DHI_Images/180215/180215011.JPG",
  .filePlasma = "/home/fuze/DHI_Images/180215/180215012.JPG",
  .xLL = 2531,
  .yLL = 2437,
  .xUR = 3356,
  .yUR = 4349,
  //.fileRef = "/home/fuze/DHI_Images/180627/180627007_Baseline.JPG",
  //.filePlasma = "/home/fuze/DHI_Images/180627/180627007.JPG",
  //.xLL = 2750,
  //.yLL = 2424,
  //.xUR = 3804,
  //.yUR = 4867,
  .pinchCurrent = 95E3,
  .edgeFracFB = 0.1,
  .saveMDSplus = 1,
  .mdsPlusTree = "fuze",
  .mdsPlusHost = "10.10.10.240",
  .shotNumber = 12345,
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
  .fileCentroid = "data/centroid.txt",
  .fileDensity = "data/error.txt",
  .fileBTheta = "data/bTheta.txt",
  .fileTemperature = "data/temperature.txt"
};

#endif
