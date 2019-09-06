#include "cpp/interferometry.h"

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
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testGetOffset();
static bool testGetInitialPhase();
static bool testRotate2DSignal();

bool testInterferometry() {

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
  
  if ( (initPhase > -0.01) && (initPhase < 0.01) ) {
    return true;
  } else {
    return false;
  }

}
