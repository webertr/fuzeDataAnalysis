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
