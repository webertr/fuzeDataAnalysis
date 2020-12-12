#include "cpp/interferometry.h"

using namespace MDSplusAccess;

/******************************************************************************
 *
 * These are functions to break down inteferometry data from the FuZE experimemt
 *
 ******************************************************************************/

static double getCosOffset(long baseLineShotNumber, int chordNum);
static double getSinOffset(long baseLineShotNumber, int chordNum);
static double getInitialPhase(gsl_vector *cosVec, gsl_vector *sinVec);
static void rotate2DSignal(gsl_vector **xVec, gsl_vector **yVec, double phase);
static gsl_vector *getCosData(long shotNumber, int chordNum);
static gsl_vector *getSinData(long shotNumber, int chordNum);
static void boxCarSmooth(gsl_vector *xVec, int width);
static gsl_vector *getPhase(gsl_vector *cosVec, gsl_vector *sinVec);
static gsl_vector *convertPhase(gsl_vector *phaseVec);
static gsl_vector *getRadius(gsl_vector *cosVec, gsl_vector *sinVec);
static gsl_vector *unwrapPhase(gsl_vector *phase, gsl_vector *radius);
static gsl_vector *getTimeBase(long shotNumber, int chordNum);


/******************************************************************************
 * Function: getCosData
 * Inputs: 
 * Returns: boolean
 * Description: Gets the cos(phi(t)) data.
 ******************************************************************************/

static gsl_vector *getCosData(long shotNumber, int chordNum) {
  
  std::string cNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":COS" << chordNum;
  cNode = oss.str();
  oss.str("");

  gsl_vector *cosData = readMDSplusVector(shotNumber, cNode, "fuze");

  return cosData;
  
}


/******************************************************************************
 * Function: getSinData
 * Inputs: 
 * Returns: boolean
 * Description: Gets the sin(phi(t)) data.
 ******************************************************************************/

static gsl_vector *getSinData(long shotNumber, int chordNum) {
  
  std::string sNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":SIN" << chordNum;
  sNode = oss.str();
  oss.str("");

  gsl_vector *sinData = readMDSplusVector(shotNumber, sNode, "fuze");

  return sinData;
  
}

/******************************************************************************
 * Function: getInitialPhase
 * Inputs: 
 * Returns: boolean
 * Description: Calculates the initial phase of the interference data
 ******************************************************************************/

static double getInitialPhase(gsl_vector *cosVec, gsl_vector *sinVec) {
  
  double cos1, sin1;

  cos1 = gsl_vector_get(cosVec, 0);
  sin1 = gsl_vector_get(sinVec, 0);

  double initPhase = atan2(sin1, cos1);

  return initPhase;
  
}


/******************************************************************************
 * Function: getCosOffset
 * Inputs: 
 * Returns: boolean
 * Description: Calculates inteferometry data baseline offset for passed 
 * baseline shot number
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:COS# = The cos of the phase shift of the scene beam
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:SIN# = The sin of the phase shift of the scene beam
 ******************************************************************************/

static double getCosOffset(long baseLineShotNumber, int chordNum) {
  
  std::string baseLineCNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":COS" << chordNum;
  baseLineCNode = oss.str();
  oss.str("");

  gsl_vector *baseLineCos = readMDSplusVector(baseLineShotNumber, baseLineCNode, "fuze");

  double cosOffset = gsl_stats_mean(baseLineCos->data, baseLineCos->size, baseLineCos->stride);

  gsl_vector_free(baseLineCos);

  return cosOffset;
  
}

/******************************************************************************
 * Function: getSinOffset
 * Inputs: 
 * Returns: boolean
 * Description: Calculates inteferometry data baseline offset for passed 
 * baseline shot number
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:SIN# = The sin of the phase shift of the scene beam
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:SIN# = The sin of the phase shift of the scene beam
 ******************************************************************************/

static double getSinOffset(long baseLineShotNumber, int chordNum) {
  
  std::string baseLineCNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":SIN" << chordNum;
  baseLineCNode = oss.str();
  oss.str("");

  gsl_vector *baseLineSin = readMDSplusVector(baseLineShotNumber, baseLineCNode, "fuze");

  double sinOffset = gsl_stats_mean(baseLineSin->data, baseLineSin->size, baseLineSin->stride);

  gsl_vector_free(baseLineSin);

  return sinOffset;
  
}


/******************************************************************************
 * Function: rotate2DSignal
 * Inputs: 
 * Returns: void
 * Description: Rotates the 2D x, y data. Modifies the passed vectors.
 ******************************************************************************/

static void rotate2DSignal(gsl_vector **xVec, gsl_vector **yVec, double phase) {
  
  double tempX, tempY, tempXp, tempYp;
  int ii;


  for (ii = 0; ii < (int) (*xVec)->size; ii++) {

    tempX = gsl_vector_get(*xVec, ii);
    tempY = gsl_vector_get(*yVec, ii);
    tempXp = tempX * cos(phase) + tempY * sin(phase);
    tempYp = -tempX * sin(phase) + tempY * cos(phase);

    gsl_vector_set(*xVec, ii, tempXp);
    gsl_vector_set(*yVec, ii, tempYp);

  }



  return;

}


/******************************************************************************
 * Function: boxCarSmooth
 * Inputs: 
 * Returns: void
 * Description: Runs a box car smoothing procedure. Returns the same size
 * vector with the ends as duplicates of first and last smoothed point
 ******************************************************************************/

static void boxCarSmooth(gsl_vector *xVec, int width) {
  
  double sum;
  int ii, jj,
    sizeVec = xVec->size;

  /* Temp vector to hold box car */
  gsl_vector *xVecTemp = gsl_vector_alloc(sizeVec);

  /* Doing box car average. */
  for (ii = width; ii < (sizeVec - width); ii++) {
    
    sum = 0;
    for (jj = 0; jj < (2*width+1); jj++) {
      sum = sum + gsl_vector_get(xVec, ii - width + jj);
    }
    gsl_vector_set(xVecTemp, ii, sum/(2*width+1));

  }
	 
  /* Filling in begining and end */
  for (ii = 0; ii < width; ii++) {
    gsl_vector_set(xVecTemp, ii, gsl_vector_get(xVecTemp, width));
  } 

  for (ii = (sizeVec-width); ii < sizeVec; ii++) {
    gsl_vector_set(xVecTemp, ii, gsl_vector_get(xVecTemp, sizeVec-width-1));
  } 

  /* Copying vector over */
  gsl_vector_memcpy(xVec, xVecTemp);

  /* Removing temp vector */
  gsl_vector_free(xVecTemp);

  return;

}


/******************************************************************************
 * Function: getPhase
 * Inputs: 
 * Returns: void
 * Description: Simply gets the phase by calculating the atan(sin/cos).
 ******************************************************************************/

static gsl_vector *getPhase(gsl_vector *cosVec, gsl_vector *sinVec) {

  int ii,
    vecLen = (int) cosVec->size;

  double tempCos,
    tempSin,
    tempPhase;

  gsl_vector *retVec = gsl_vector_alloc(vecLen);

  for (ii = 0; ii < vecLen; ii++) {

    tempCos = gsl_vector_get(cosVec, ii);
    tempSin = gsl_vector_get(sinVec, ii);

    tempPhase = atan2(tempSin, tempCos);

    gsl_vector_set(retVec, ii, tempPhase);

  }

  return retVec;

}


/******************************************************************************
 * Function: unwrapPhase
 * Inputs: 
 * Returns: void
 * Description: These simply unwraps the phase from the phase vector 
 ******************************************************************************/

static gsl_vector *unwrapPhase(gsl_vector *phase, gsl_vector *radius) {

  double dp,
    sizeVec = phase->size,
    thresh = M_PI*1.0,
    p1, p2, rad;

  int ii, jj;

  gsl_vector *vecRet = gsl_vector_alloc(sizeVec);

  gsl_vector_memcpy(vecRet, phase);

  /* Unwrapping the rows, eliminate changes > pi */
  for (ii = 1; ii < sizeVec; ii++) {

    rad = gsl_vector_get(radius, ii);

    /* Skip if the radius is too small */
    if ( rad < 0.01) {
      continue;
    }

    p1 = gsl_vector_get(vecRet, ii-1);
    p2 = gsl_vector_get(vecRet, ii);

    dp = p2 - p1;

    /* Jump greater then pi means likely it rolled over from -pi */
    if ( dp > thresh) {
      for (jj = ii; jj < sizeVec; jj++) {
	gsl_vector_set(vecRet, jj, gsl_vector_get(vecRet, jj) - 2.0*M_PI);
      }
    }

    /* Jump less then -pi means likely it rolled over from +pi */
    if ( dp < -thresh) {
      for (jj = ii; jj < sizeVec; jj++) {
	gsl_vector_set(vecRet, jj, gsl_vector_get(vecRet, jj) + 2.0*M_PI);
      }
    }
     
  }

  return vecRet;

}


/******************************************************************************
 * Function: getRadius
 * Inputs: 
 * Returns: void
 * Description: Simplys calculates the radius by sqrt(cos^2+sin^2)
 ******************************************************************************/

static gsl_vector *getRadius(gsl_vector *cosVec, gsl_vector *sinVec) {

  int ii,
    vecLen = (int) cosVec->size;

  double tempCos,
    tempSin,
    tempRadius;

  gsl_vector *retVec = gsl_vector_alloc(vecLen);

  for (ii = 0; ii < vecLen; ii++) {

    tempCos = gsl_vector_get(cosVec, ii);
    tempSin = gsl_vector_get(sinVec, ii);

    tempRadius = sqrt(tempCos*tempCos+tempSin*tempSin);

    gsl_vector_set(retVec, ii, tempRadius);

  }

  return retVec;

}


/******************************************************************************
 * Function: convertPhase
 * Inputs: 
 * Returns: void
 * Description: Convert the phase to line integrated density
 ******************************************************************************/

static gsl_vector *convertPhase(gsl_vector *phaseVec) {

  int ii,
    vecLen = (int) phaseVec->size;

  double tempPhase,
    conversion = 5.61 * 1E20; // Converstion using 632.8 nm for laser wavelength

  gsl_vector *retVec = gsl_vector_alloc(vecLen);

  for (ii = 0; ii < vecLen; ii++) {

    tempPhase = gsl_vector_get(phaseVec, ii);
    gsl_vector_set(retVec, ii, tempPhase*conversion);

  }

  return retVec;

}


/******************************************************************************
 * Function: getIFDensity 
 * Inputs: 
 * Returns: void
 * Description: This will return the line integrated density for the passed
 * chords with the given shot number and reference shot number
 ******************************************************************************/

gsl_vector *getIFDensity(int shotNumber, int shotNumberRef, int chordNum) {

  double cosOffset, sinOffset, initPhase;

  int boxCarSmoothParam = 10;

  /* Getting baseline offset */
  cosOffset = getCosOffset(shotNumberRef, chordNum);
  sinOffset = getSinOffset(shotNumberRef, chordNum);

  /* Getting cos and sin data */
  gsl_vector *cosVec = getCosData(shotNumber, chordNum);
  gsl_vector *sinVec = getSinData(shotNumber, chordNum);

  /* Subtracting offset */
  gsl_vector_add_constant(cosVec, -cosOffset);
  gsl_vector_add_constant(sinVec, -sinOffset);

  initPhase = getInitialPhase(cosVec, sinVec);

  rotate2DSignal(&cosVec, &sinVec, initPhase);

  gsl_vector *phase = getPhase(cosVec, sinVec);
  gsl_vector *radius = getRadius(cosVec, sinVec);
  gsl_vector *unwrapped = unwrapPhase(phase, radius);

  boxCarSmooth(unwrapped, boxCarSmoothParam);
  boxCarSmooth(phase, boxCarSmoothParam);
  
  gsl_vector *density = convertPhase(unwrapped);

  gsl_vector_free(cosVec);
  gsl_vector_free(sinVec);
  gsl_vector_free(phase);
  gsl_vector_free(radius);
  gsl_vector_free(unwrapped);

  return density;

}

/******************************************************************************
 * Function: getTimeBase
 * Inputs: 
 * Returns: boolean
 * Description: Gets the time value for the raw data for the give nchord
 ******************************************************************************/

static gsl_vector *getTimeBase(long shotNumber, int chordNum) {
  
  std::string cNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":COS" << chordNum;
  cNode = oss.str();
  oss.str("");

  gsl_vector *time = readMDSplusVectorDim(shotNumber, cNode, "fuze");

  return time;
  
}

/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testGetOffset();
static bool testGetInitialPhase();
static bool testRotate2DSignal();
static bool testBoxCarSmooth();
static bool testGetPhase();
static bool testConvertPhase();
static bool testGetRadius();
static bool testUnwrapPhase();
static bool testGetIFDensityForAPS();

bool testInterferometry() {

  if (0) {
    testGetIFDensityForAPS();
  }

  if (!testGetOffset()) {
    std::cout << "Test interferometry.cpp get offset FAILED\n";
    return false;
  }

  if (!testGetInitialPhase()) {
    std::cout << "Test interferometry.cpp get initial phase FAILED\n";
    return false;
  }

  if (!testRotate2DSignal()) {
    std::cout << "Test interferometry.cpp rotate 2D signal FAILED\n";
    return false;
  }

  if (!testBoxCarSmooth()) {
    std::cout << "Test interferometry.cpp box car smooth signal FAILED\n";
    return false;
  }

  if (!testGetPhase()) {
    std::cout << "Test interferometry.cpp getPhase FAILED\n";
    return false;
  }

  if (!testConvertPhase()) {
    std::cout << "Test interferometry.cpp getPhase FAILED\n";
    return false;
  }

  if (!testGetRadius()) {
    std::cout << "Test interferometry.cpp getRadius FAILED\n";
    return false;
  }

  if (!testUnwrapPhase()) {
    std::cout << "Test interferometry.cpp unwrapPhase FAILED\n";
    return false;
  }

  std::cout << "All interferometry.cpp tests passed\n";

  return true;

}


bool testGetOffset() { 

  double cosOffset, sinOffset;

  cosOffset = getCosOffset(190903004, 1);

  sinOffset = getSinOffset(190903004, 1);

  if ( (cosOffset > .014) && (cosOffset < .015) && (sinOffset > .02) && (sinOffset < .03)) {
    return true;
  } else {
    return false;
  }

}

bool testGetInitialPhase() { 

  double initPhase;
  double cosOffset, sinOffset;

  cosOffset = getCosOffset(190903004, 1);
  sinOffset = getSinOffset(190903004, 1);

  gsl_vector *cosVec = getCosData(190903007, 1);
  gsl_vector *sinVec = getSinData(190903007, 1);

  gsl_vector_add_constant(cosVec, -cosOffset);
  gsl_vector_add_constant(sinVec, -sinOffset);

  initPhase = getInitialPhase(cosVec, sinVec);

  gsl_vector_free(cosVec);
  gsl_vector_free(sinVec);

  if ( (initPhase > 1.4) && (initPhase < 1.5) ) {
    return true;
  } else {
    return false;
  }

}


bool testRotate2DSignal() { 

  double initPhase;
  double cosOffset, sinOffset;

  cosOffset = getCosOffset(190903004, 1);
  sinOffset = getSinOffset(190903004, 1);

  gsl_vector *cosVec = getCosData(190903007, 1);
  gsl_vector *sinVec = getSinData(190903007, 1);

  gsl_vector_add_constant(cosVec, -cosOffset);
  gsl_vector_add_constant(sinVec, -sinOffset);

  initPhase = getInitialPhase(cosVec, sinVec);

  rotate2DSignal(&cosVec, &sinVec, initPhase);

  initPhase = getInitialPhase(cosVec, sinVec);

  gsl_vector_free(cosVec);
  gsl_vector_free(sinVec);
  
  if ( (initPhase > -0.01) && (initPhase < 0.01) ) {
    return true;
  } else {
    return false;
  }

}


bool testBoxCarSmooth() { 

  int ii;

  gsl_vector *xVecTest = gsl_vector_alloc(100);
  gsl_vector *yVecTest1 = gsl_vector_alloc(100);
  gsl_vector *yVecTest2 = gsl_vector_alloc(100);

  for (ii = 0; ii < 100; ii++) {
    gsl_vector_set(xVecTest, ii, ii);
    
    if (ii % 2 == 0) {
      gsl_vector_set(yVecTest1, ii, ii+10);
      gsl_vector_set(yVecTest2, ii, ii+10);
    } else {
      gsl_vector_set(yVecTest1, ii, ii-10);
      gsl_vector_set(yVecTest2, ii, ii-10);
    }

  }

  boxCarSmooth(yVecTest2, 10);

  double testVal = gsl_vector_get(yVecTest2, 50);

  gsl_vector_free(xVecTest);
  gsl_vector_free(yVecTest1);
  gsl_vector_free(yVecTest2);

  if ( (testVal < 55) && (testVal > 45) ) {
    return true;
  }

  plot2VectorData(xVecTest, yVecTest1, "title 'Non-smooth'", yVecTest2, "title 'Smooth'", "",
		  "data/temp.txt", "data/temp.sh");

  return false;

}


bool testGetPhase() { 

  gsl_vector *cosVec = getCosData(190903007, 1);
  gsl_vector *sinVec = getSinData(190903007, 1);

  gsl_vector *phase = getPhase(cosVec, sinVec);

  double testVal = gsl_vector_get(phase, 50);

  gsl_vector_free(cosVec);
  gsl_vector_free(sinVec);
  gsl_vector_free(phase);

  if ( (testVal < 1.13) && (testVal > 1.1) ) {
    return true;
  }

  return false;

}


bool testConvertPhase() { 

  gsl_vector *cosVec = getCosData(190903007, 1);
  gsl_vector *sinVec = getSinData(190903007, 1);

  gsl_vector *phase = getPhase(cosVec, sinVec);
  gsl_vector *density = convertPhase(phase);

  double testVal = gsl_vector_get(density, 50);

  gsl_vector_free(sinVec);
  gsl_vector_free(cosVec);
  gsl_vector_free(phase);
  gsl_vector_free(density);

  if ( (testVal < 6.4E20) && (testVal > 6.3E20) ) {
    return true;
  }

  return false;

}


bool testGetRadius() { 

  gsl_vector *cosVec = getCosData(190903007, 1);
  gsl_vector *sinVec = getSinData(190903007, 1);

  gsl_vector *radius = getRadius(cosVec, sinVec);

  double testVal = gsl_vector_get(radius, 50);

  gsl_vector_free(sinVec);
  gsl_vector_free(cosVec);
  gsl_vector_free(radius);

  if ( (testVal < 0.04) && (testVal > 0.03) ) {
    return true;
  }

  return false;

}


bool testUnwrapPhase() { 

  int ii, jj;
  double phase;
  
  gsl_vector *testTime = gsl_vector_alloc(100);
  gsl_vector *testPhase = gsl_vector_alloc(100);
  gsl_vector *unwrapped = gsl_vector_alloc(100);
  gsl_vector *radius = gsl_vector_alloc(100);

  for (ii = 0; ii < 100; ii++) {
    gsl_vector_set(testTime, ii, ii);
    gsl_vector_set(radius, ii, 0.1);
    gsl_vector_set(testPhase, ii, ii/10.0);
  }

  for (ii = 0; ii < 100; ii++) {
    phase = gsl_vector_get(testPhase, ii); 
    if ( phase > (2*M_PI) ) {
      for (jj = ii; jj < 100; jj++) {
	gsl_vector_set(testPhase, jj, gsl_vector_get(testPhase, jj) - 2*M_PI);
      }
    }
    
  }

  unwrapped = unwrapPhase(testPhase, radius);

  double testVal = gsl_vector_get(unwrapped, 75);

  gsl_vector_free(testTime);
  gsl_vector_free(testPhase);
  gsl_vector_free(unwrapped);
  gsl_vector_free(radius);

  if ( (testVal < 7.6) && (testVal > 7.4) ) {
    return true;
  }

  plot2VectorData(testTime, testPhase, "title 'wrapped'", unwrapped, "title 'unwrapped'", "",
		 "data/temp.txt", "data/temp.sh");

  return false;

}


bool testGetIFDensityForAPS() { 

  int shotNumber = 190416004;
  int shotNumberRef = 190416001;

  gsl_vector *density5 = getIFDensity(shotNumber, shotNumberRef, 5);
  gsl_vector *density8 = getIFDensity(shotNumber, shotNumberRef, 8);
  gsl_vector_scale(density8, -1.0);
  //gsl_vector_scale(density5, -1.0);

  gsl_vector *time = getTimeBase(shotNumber, 5);
  gsl_vector_scale(time, 1E6);

  std::string plotOptions = "set xrange[20:35]\n"
    "set yrange[-1E21:]\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/if_density_190416004.png'\n"
    "set grid\n"
    "set key left top\n"
    "set ylabel 'n_{e} (cm^{-3})' font ',16' offset 0,0\n"
    "set xlabel 'Time ({/Symbol m}sec)' font ',16' offset 0,0\n"
    "set title 'Line-integrated density in acceleration region' "
    "font 'Times Bold,16'\n"
    "set label '#190416004' at graph 0.02,0.83 font 'Times Bold, 14'\n";


  plot2VectorData(time, density5, "title 'z=-40 cm'", density8, "title 'z=-10 cm'", 
  		  plotOptions, "data/temp.txt", "data/temp.sh");

  gsl_vector *timeB = readMDSplusVectorDim(shotNumber, "\\b_n40_0", "fuze");
  gsl_vector *b_n40 = readMDSplusVector(shotNumber, "\\b_n40_0", "fuze");
  gsl_vector *b_n10 = readMDSplusVector(shotNumber, "\\b_n10_0", "fuze");

  gsl_vector_scale(timeB, 1E6);

  plotOptions = "set xrange[20:35]\n"
    //"set yrange[-1E21:]\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/bfield_190416004.png'\n"
    "set grid\n"
    "set key left top\n"
    "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n"
    "set xlabel 'Time ({/Symbol m}sec)' font ',16' offset 0,0\n"
    "set title 'B_{/Symbol q} in the acceleration region' "
    "font 'Times Bold,16'\n"
    "set label '#190416004' at graph 0.02,0.83 font 'Times Bold, 14'\n";

  plot2VectorData(timeB, b_n40, "title 'z=-40 cm'", b_n10, "title 'z=-10 cm'", 
		  plotOptions, "data/temp.txt", "data/temp.sh");


  double testVal = 0.035;

  if ( (testVal < 0.04) && (testVal > 0.03) ) {
    return true;
  }

  return false;

}


/*
 * This is the python code to process the IF data for the FuZE experiment. From Han.
 * For reference

#!/usr/bin/env python
"""
Created on Oct 2017
Last Updated on Dec 13 2017
author: Bonghan Kim
FuZE HeNe interferometry code - Location TODO NOTINPLACE

Version history

V3: Added Brian's optimization and style corrections.
    NOTE: Don't for-loop np.array for simple operations. They can handle
    element wise operations efficiently.

V4: loadSmoothingInfo implemented
  : loadTrendlineInfo implemented
  : Write to tree functions tested

V5: Calculate offset if shotNum == refShotNum
  : Read offset if shotNum != refShotNum
  : Write to offset

V5 TODO: Write detect IP functionality for detecting the beginning and the end
         of the relevent sig for trendline subtraction.

"""

from scipy.constants import pi
import MDSplus as MDS
import numpy as np
import matplotlib.pyplot as plt
# Possible packages that I could try and use later
# import pylab as P
# import scipy.signal as signal


def main(shotNum=1233,
         save2tree=True, ignoreZeroR=False, invert=False, debug=False):

    # Get Tree
    T = MDS.Tree('fuze', shotNum)

    for chord in range(1, 9):
        try:

            # load reference shot number
            refShotNum = T.getNode(r'\NE_1:BASELINE').getData().data()

            if shotNum == refShotNum:
                setRawOffset(T, chord, save2tree, debug)
            else:
                processDensity(T, chord, save2tree, ignoreZeroR, invert, debug)

            if debug:
                print('chord %d processed' % chord)

        except:
            if debug:
                print('chord %d skipped due to lack of data' % chord)
            continue


def setRawOffset(MDSTree, chord, save2tree, debug):
    (cos_Raw, sin_Raw, time_Raw) = loadIFdata(chord, MDSTree)
    cosOffset = np.mean(cos_Raw)
    sinOffset = np.mean(sin_Raw)

    if save2tree:
        saveDouble(cosOffset, r'\COS_%d:COS_OFF' % chord, MDSTree)
        saveDouble(sinOffset, r'\SIN_%d:SIN_OFF' % chord, MDSTree)

    if debug:
        plt.figure('offset diagnostic plots')
        plt.subplot(2, 2, 1)
        plt.plot(sin_Raw)
        plt.subplot(2, 2, 2)
        plt.plot(cos_Raw)


def processDensity(MDStree, chord, save2tree, ignoreZeroR, invert, debug):

    # -------------------------------------------------------------------------
    # ---------------------------- LOAD SETTINGS ------------------------------
    # -------------------------------------------------------------------------

    convert = 5.61*1E20  # Conversion using 632.8nm for laser wavelength
    T = MDStree

    # load reference shot numbers
    refShotNum = T.getNode(r'\NE_1:BASELINE').getData().data()
    Tref = MDS.Tree('fuze', refShotNum)

    # load smoothing window
    (sm_win1, sm_win2, sm_win3, sm_win4) = loadSmoothingInfo(chord, T)

    # Threshhold used for detecting fringe jump.
    jumpDet = pi

    # Import IF data from MDS tree with default name T
    if invert:
        (sin_Raw, cos_Raw, time_Raw) = loadIFdata(chord, T)
    else:
        (cos_Raw, sin_Raw, time_Raw) = loadIFdata(chord, T)

    # Import IF baseline data from MDS tree with default name T.
    if invert:
        (sinOffset, cosOffset) = loadIFbaseline(chord, Tref)
    else:
        (cosOffset, sinOffset) = loadIFbaseline(chord, Tref)

    # reads IP data to be used in determining trendline fit domain
    (Ip, Tip) = loadIpdata(chord, T)

    (sig_i_t, plas_i_t, plas_f_t, sig_f_t) = loadTrendlineInfo(chord, T)

    # TODO: make auto detection more reliable
    (plas_i_t_auto, plas_f_t_auto) = find_trendline_domain(Ip, Tip)

    if plas_i_t == -99.:
        plas_i_t = plas_i_t_auto
    if plas_f_t == -99.:
        plas_f_t = plas_f_t_auto

    signal_i = timeToIndex(sig_i_t, time_Raw)
    plasma_i = timeToIndex(plas_i_t, time_Raw)
    plasma_f = timeToIndex(plas_f_t, time_Raw)
    signal_f = timeToIndex(sig_f_t, time_Raw)

    # if auto detection fails,
    if plasma_i - 5000 < signal_i:
        plasma_i = signal_i + 5001  # must be greater than
    if plasma_f + 5000 > signal_f:
        plasma_f = signal_f - 5001

    # -------------------------------------------------------------------------
    # -------------------------- SIGNAL PROCESSING ----------------------------
    # -------------------------------------------------------------------------

    # Obtain cosine and sine signal with baseline subtraction.
    # This centeres the lisajou.
    cos_SubBase = cos_Raw - cosOffset
    sin_SubBase = sin_Raw - sinOffset

    # Find the initial phase of the cos and sin signal.
    phaseInitial = calc_initial_phase(cos_SubBase, sin_SubBase)

    # Rotate the cos and sin signal so the phase starts at 0.
    # the give pi+1 above and pi-1 below of phase before phase jump.
    (cos_SubBase, sin_SubBase) = rotate2d(cos_SubBase, sin_SubBase,
                                          phaseInitial)

    # Smooths cos and sin signal using two windows.
    cos_SubBase = smooth(smooth(cos_SubBase, window_len=sm_win1),
                         window_len=sm_win2)
    sin_SubBase = smooth(smooth(sin_SubBase, window_len=sm_win1),
                         window_len=sm_win2)

    # Convert cos_SubBase and sin_SubBase signals into phase signal with
    #  fringe jumps.x
    phase = get_phase(cos_SubBase, sin_SubBase)
    densityRaw = phase * convert

    # Calculates radius of lisajou.
    radius = get_radius(cos_SubBase, sin_SubBase)

    # When change in value between data+ is greater than threashhold.
    # it increments the jump_correction list.
    jump_correction = get_jump_correction(phase, radius, jumpDet, ignoreZeroR)

    # Subtracting jump_correction*2*pi gives the jump corrected phase.
    phase_jump_corrected = phase - jump_correction * 2 * pi
    density = phase_jump_corrected * convert

    # Trendline Subtraction.
    # NOTE: This trucates the signal from index signal_i ot index signal_f.
    (baseline, fitline, fitline_time) = get_trendline(phase_jump_corrected,
                                                      time_Raw, signal_i,
                                                      plasma_i, plasma_f,
                                                      signal_f)
    phase_sub_TrendLine = phase_jump_corrected[signal_i:signal_f] - baseline
    timeForTrendline = time_Raw[signal_i:signal_f]
    densitySubTrend = phase_sub_TrendLine*convert

    # Saving smoothed signals
    density_sm = smooth(smooth(density, sm_win3), sm_win4)
    densitySubTrend_sm = smooth(smooth(densitySubTrend, sm_win3), sm_win4)

    # -------------------------------------------------------------------------
    # ----------------------------- SAVE TO TREE ------------------------------
    # -------------------------------------------------------------------------

    # TODO Test this
    if save2tree:
        saveData(density, time_Raw, r'\NE_%d' % chord, T)  # NE#
        saveData(density_sm, time_Raw, r'\NE_%d_sm' % chord, T)
        saveData(densityRaw, time_Raw, r'\NE_%d_Raw' % chord, T)  # NE#_Raw
        saveData(densitySubTrend, timeForTrendline,
                 r'\NE_%d_SubTrend' % chord, T)  # NE_subTrend
        saveData(densitySubTrend_sm, timeForTrendline,
                 r'\NE_%d_SubTrend_sm' % chord, T)  # TODO Too long
        saveData(cos_SubBase, time_Raw,
                 r'\COS%d_rotated' % chord, T)  # processed cos signal
        saveData(sin_SubBase, time_Raw,
                 r'\SIN%d_rotated' % chord, T)  # processed sin signal
        saveData(radius, time_Raw,
                 r'\NE_%d:radius' % chord, T)  # radius of processed cos,sin
        saveDouble(cosOffset, r'\COS_%d:COS_OFF' % chord, T)  # cos offset used
        saveDouble(sinOffset, r'\SIN_%d:SIN_OFF' % chord, T)  # sin offset used
        trendlineDomain = np.array([sig_i_t, plas_i_t, plas_f_t, sig_f_t])
        saveArray(trendlineDomain, r'\ne_%d:TREND_WINDOW' % chord, T)

    # -------------------------------------------------------------------------
    # ----------------------------- DEBUG PLOTS -------------------------------
    # -------------------------------------------------------------------------

    # Plots for debugging
    if debug:
        plt.close()
        plt.figure("IF Diagnostic Signals shot %d chord %d" % (T.shot, chord),
                   figsize=(18, 8))

        # Phase and jump corrected phase
        ax = plt.subplot(2, 3, 1)
        plt.plot(time_Raw, densityRaw, label='Original')
        plt.plot(time_Raw, density, 'r-', label='Jump Corrected')
        plt.legend(loc='lower right')
        plt.grid(True)

        # Radius
        plt.subplot(2, 3, 2, sharex=ax)
        plt.plot(time_Raw, radius, 'g-', label="Radius")

        plt.title('Shot: %d Chord: %d' % (T.shot, chord))
        plt.legend()
        plt.grid(True)
        plt.ylim(ymin=0)
        # ca = plt.gca()

        # Details of trendline subtraction (NOTE: Truncated)
        plt.subplot(2, 3, 4, sharex=ax)
        plt.plot(time_Raw[signal_i:signal_f],
                 phase_jump_corrected[signal_i:signal_f], 'r-',
                 label='Phase w/o trendline')
        plt.plot(time_Raw[signal_i:signal_f], baseline, 'g-',
                 label='Trendline')
        plt.plot(fitline_time, fitline, 'b.', label='Fitline Domain')
        plt.legend(loc='upper right')
        plt.grid(True)

        # Trendline subtracted phase signal (NOTE: Truncated)
        plt.subplot(2, 3, 5, sharex=ax)
        plt.plot(timeForTrendline, densitySubTrend, 'b-',
                 label='Trendline Corrected')
        plt.plot(timeForTrendline, densitySubTrend_sm, 'r-', lw=2,
                 label='Trendline Corrected SM')
        plt.legend(loc='upper right')
        plt.grid()

        # Plasma current trace
        plt.subplot(2, 3, 6, sharex=ax)
        plt.plot(Tip, Ip)
        plt.plot(Tip, np.ones(Tip.size)*max(Ip)*0.05, 'r-')


# Imports smoothing windows used in the analysis
# TODO: test
def loadSmoothingInfo(chord, MDStree):
    sm_win = MDStree.getNode(r'\ne_%d:SM_WINDOW' % chord).getData().data()
    return(sm_win[0], sm_win[1], sm_win[2], sm_win[3])


# Imports trendline subtraction domains
# TODO: test
def loadTrendlineInfo(chord, MDStree):
    trnd_win = MDStree.getNode(r'\ne_%d:TREND_WINDOW' % chord).getData().data()
    return(trnd_win[0], trnd_win[1], trnd_win[2], trnd_win[3])


# Imports cosRaw, sinRaw, and time_Raw signal given
#  (chord, refShotnum, MDStree = T)
def loadIFdata(chord, MDStree):
    cosRaw = MDStree.getNode(r'\cos%d' % chord).getData().data()
    sinRaw = MDStree.getNode(r'\sin%d' % chord).getData().data()
    time_Raw = MDStree.getNode(r'\cos%d' % chord).dim_of().data()
    return (cosRaw, sinRaw, time_Raw)


# Imports IP signal given (chord, MDStree =T)
def loadIpdata(chord, MDStree):
    I_P = MDStree.getNode(r'\I_P').getData().data()
    Tip = MDStree.getNode(r'\I_P').dim_of().data()
    return (I_P, Tip)


# Imports baseline value for cos and sin
def loadIFbaseline(chord, MDStree):
    cosOffset = MDStree.getNode(r'\COS_%d:COS_OFF' % chord).getData().data()
    sinOffset = MDStree.getNode(r'\SIN_%d:SIN_OFF' % chord).getData().data()
    return(cosOffset, sinOffset)


# Calculates initial phase
def calc_initial_phase(cos_SubBase, sin_SubBase):
    # Finding initial phase
    phaseInitial = np.arctan2(sin_SubBase[0], cos_SubBase[0])
    # Rotate the sin and cos signal by - initial phase
    return phaseInitial


# Rotate (xArray,yArray) CW by (angleRad)
def rotate2d(xArray, yArray, angleRad):
    xArray_new = xArray * np.cos(angleRad) + yArray * np.sin(angleRad)
    yArray_new = -xArray * np.sin(angleRad) + yArray * np.cos(angleRad)
    return (xArray_new, yArray_new)


# Calculates phase signal using (cos_SubBase) and (sin_SubBase)
def get_phase(cos_SubBase, sin_SubBase):
    phase = np.arctan2(cos_SubBase, sin_SubBase)
    return phase


# Calculates radius of lisajou using (cos_SubBase,sin_SubBase)
def get_radius(cos_SubBase, sin_SubBase):
    radius = np.sqrt(sin_SubBase**2 + cos_SubBase**2)
    return radius


# When change in value between data is greater than threashhold
# it increments the jump_correction list
def get_jump_correction(phase, radius, jump=pi, ignoreZeroR=False):
    length = len(phase)
    total_correction = 0
    jump_correction = np.zeros(length)
    for i in range(1, length):

        if ignoreZeroR and radius[i] < 0.01:
            pass
        elif (phase[i] - phase[i-1]) > jump:
            total_correction += 1
        elif (phase[i] - phase[i-1]) < -jump:
            total_correction -= 1

        jump_correction[i] = total_correction

    return jump_correction


# smooths the signal x and returns as y.
def smooth(x, window_len=11, window='flat'):
        # Possible Windows
        # ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
        s = np.r_[2 * x[0] - x[window_len-1::-1], x, 2 * x[-1] -
                  x[-1:-window_len:-1]]
        if window == 'flat':   # moving average
            w = np.ones(window_len, 'd')
        else:
            w = eval('np.'+window+'(window_len)')
        y = np.convolve(w/w.sum(), s, mode='same')
        return y[window_len:-window_len+1]


# Calculates trendline
def get_trendline(phase, time_Raw, signalStart, plasmaStart, plasmaEnd,
                  signalEnd):

    phase = phase[signalStart:signalEnd]
    time_Raw = time_Raw[signalStart:signalEnd]
    plasmaStart -= signalStart
    plasmaEnd -= signalEnd

    section1 = smooth(phase[:plasmaStart], window_len=5000)
    section1_time = time_Raw[:plasmaStart]
    section2 = smooth(phase[plasmaEnd:], window_len=5000)
    section2_time = time_Raw[plasmaEnd:]

    fitline = np.concatenate([section1, section2])
    fitline_time = np.concatenate([section1_time, section2_time])

    z = np.polyfit(fitline_time, fitline, 7)
    p = np.poly1d(z)
    trendline = [p(x) for x in time_Raw]

    return (trendline, fitline, fitline_time)


# Finds trendline domain from IP data
def find_trendline_domain(Ip, Tip):
    IpThreash = max(Ip) * 0.1
    plasmaIndex = np.where(Ip > IpThreash)
    plasmaBeginIndex = plasmaIndex[0][0]
    plasmaEndIndex = plasmaIndex[0][-1]
    plasmaBeginTime = Tip[plasmaBeginIndex]
    plasmaEndTime = Tip[plasmaEndIndex]
    return (plasmaBeginTime, plasmaEndTime)


# Saves Data to MDS Tree
def saveData(sig, time, signame, MDSTree):
    node = MDSTree.getNode(signame)
    ySig = MDS.Float32Array(sig)
    node.putData(MDS.Signal(ySig, None, time))


# for saving single number to Tree
def saveDouble(data, name, MDSTree):
    node = MDSTree.getNode(name)
    node.putData(MDS.Float64(data))


# for saving numpy array to Tree
def saveArray(data, name, MDSTree):
    node = MDSTree.getNode(name)
    node.putData(MDS.Float32Array(data))


def timeToIndex(time, time_Raw):
    indexs = np.where(time < time_Raw)
    return indexs[0][0]


# Runs main if you run this program (not importing to other modules)
if __name__ == "__main__":
    from sys import argv

    nargs = len(argv)
    if nargs == 1:
        print("Usage:")
        print("HeNe_IF_processDensity shotNum chord")
    elif nargs == 2:
        shotNum = int(argv[1])
        main(shotNum=shotNum)

*/

