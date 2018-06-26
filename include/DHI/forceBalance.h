#ifndef FORCEBALANCE_H
#define FORCEBALANCE_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>

#include "DHI/holoParam.h"
#include "save.h"
#include "plot.h"
#include "DHI/imageAnalysis.h"


int testForceBalance();
int azimuthBFieldForceBalance(gsl_vector *densityProfile, gsl_vector *azimuthalBField, 
			      double Ip, double dr, int edge);
int temperatureForceBalance(gsl_vector *densityProfile, gsl_vector *azimuthalBField, 
			    gsl_vector *temperature, double Ip, double dr, int edge);



/* 1 is using meters, 100 if using CM */
#define CM_ADJUST 100

static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT_FORCE = {
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
  .fileRef = "/home/fuze/DHI_Images/180215/180215011.JPG",
  .filePlasma = "/home/fuze/DHI_Images/180215/180215012.JPG",
  .xLL = 2531,
  .yLL = 2437,
  .xUR = 3356,
  .yUR = 4349,
  .saveHologram = 1,
  .fileHologram = "data/hologram.dat",
  .saveWrappedPhase = 1,
  .fileWrappedPhase = "data/wrappedPhase.dat",
  .saveLineInt = 1,
  .fileLineIntPos = "data/lineIntegratedPosition.dat",
  .fileLineInt = "data/lineIntegrated.dat",
  .fileLineIntText = "data/lineIntegrated.txt",
  .invertImage = 1,
  .fileLeftInvert = "data/leftAbelInvert.txt",
  .fileRightInvert = "data/rightAbelInvert.txt",
  .fileFullInvert = "data/fullAbelInvert.dat",
  .fileFullInvertPos = "data/fullAbelInvertPosition.dat",
  .fileFullInvertText = "data/fullAbelInvert.txt",
  .fileCentroid = "data/centroid.txt"
};


#endif
