#include "cpp/magnetic.h"

/******************************************************************************
 *
 * This is the source file to analyze magnetic mode data
 *
 ******************************************************************************/

/******************************************************************************
 * Example Usage:
 *  gsl_vector *data = 0,
 *   *time = 0;
 *
 * initializeMagneticData(170817005, "\\b_n95_000_sm", &data, &time);
 *
 * printf("Data Length: %d", (int) data->size);
 * printf("20th Element: %f\n", gsl_vector_get(data, 20));
 * saveVectorData(time, data, "data/temp.dat");
 * plotVectorData(time, data);
 *
 * gsl_vector_free(data);
 * gsl_vector_free(time);
 ******************************************************************************/

#define RW 10.084

/******************************************************************************
 * Function: replaceWord
 * Inputs: int
 * Returns: int
 * Description: Returns 1 if OK, 0 otherwise. Status is OK if the LSB is set.
 ******************************************************************************/

static std::string replaceWord(std::string sen, std::string oldW, std::string newW) {

  std::string result(sen);
  size_t jj = 0;
  int cnt = 0;
  int newWLen = newW.length();
  int oldWLen = oldW.length();

  // Counting the number of times old word occur in the string jj = sen.find(oldW, 0);
  jj = sen.find(oldW, 0);
  while (jj != sen.npos) {
    cnt++;
    result.replace(jj, newWLen, newW);
    result.erase(jj+newWLen, oldWLen);
    jj = sen.find(oldW, jj + oldWLen - 1);
  }

  return result;

}


/******************************************************************************
 * Function: get8AzimuthalArray
 * Inputs: int
 * Returns: gsl_matrix
 * Description: Returns a matrix of the 8 azimuthal array of magnetic probes
 * at p15 in the fuze machine with time at the end. First column is the time
 * values, next 8 are the 8 azimuthal probes. Going down the rows represent
 * moving forward in time. Use "\\b_p15_000"
 ******************************************************************************/

gsl_matrix *get8AzimuthalArray(int shotNumber, std::string nodeName) {


  gsl_vector* time,
    *p_000,
    *p_045,
    *p_090,
    *p_135,
    *p_180,
    *p_225,
    *p_270,
    *p_315;
  
  int sigSize;
  
  time = readMDSplusVectorDim(shotNumber, nodeName, "fuze");
  sigSize = time->size;
  
  gsl_matrix *azimuthArray = gsl_matrix_alloc(sigSize, 9);
 

  std::string nodeName0 = nodeName,
    nodeName45 = replaceWord(nodeName, "000", "045"),
    nodeName90 = replaceWord(nodeName, "000", "090"),
    nodeName135 = replaceWord(nodeName, "000", "135"),
    nodeName180 = replaceWord(nodeName, "000", "180"),
    nodeName225 = replaceWord(nodeName, "000", "225"),
    nodeName270 = replaceWord(nodeName, "000", "270"),
    nodeName315 = replaceWord(nodeName, "000", "315");

  /* Geting Data */
  p_000 = readMDSplusVector(shotNumber, nodeName0, "fuze");
  p_045 = readMDSplusVector(shotNumber, nodeName45, "fuze");
  p_090 = readMDSplusVector(shotNumber, nodeName90, "fuze");
  p_135 = readMDSplusVector(shotNumber, nodeName135, "fuze");
  p_180 = readMDSplusVector(shotNumber, nodeName180, "fuze");
  p_225 = readMDSplusVector(shotNumber, nodeName225, "fuze");
  p_270 = readMDSplusVector(shotNumber, nodeName270, "fuze");
  p_315 = readMDSplusVector(shotNumber, nodeName315, "fuze");
    
  gsl_matrix_set_col(azimuthArray, 0, time);
  gsl_matrix_set_col(azimuthArray, 1, p_000);
  gsl_matrix_set_col(azimuthArray, 2, p_045);
  gsl_matrix_set_col(azimuthArray, 3, p_090);
  gsl_matrix_set_col(azimuthArray, 4, p_135);
  gsl_matrix_set_col(azimuthArray, 5, p_180);
  gsl_matrix_set_col(azimuthArray, 6, p_225);
  gsl_matrix_set_col(azimuthArray, 7, p_270);
  gsl_matrix_set_col(azimuthArray, 8, p_315);


  
  /* Freeing data */
  gsl_vector_free(time);
  gsl_vector_free(p_000);
  gsl_vector_free(p_045);
  gsl_vector_free(p_090);
  gsl_vector_free(p_135);
  gsl_vector_free(p_180);
  gsl_vector_free(p_225);
  gsl_vector_free(p_270);
  gsl_vector_free(p_315);

  
  return azimuthArray;

}


/******************************************************************************
 * Function: getAzimuthalArrayModes
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: Receives a matrix that has 9 columns, the first being the time
 * value, and the next 8 being the 8 different azimuthal magnetic probe data
 * This function will calculate the normalized m = 0, 1, 2 and 3 modes, and 
 * overwrite them into the old matrix: the first being the time, and the next
 * 4 being the modes m = 0, 1, 2 and 3 all divided by the m = 0 mode
 * Power gets divided between the positive and negative frequcnes, so you
 * should be off by a factor of 2 for the amplitude
 ******************************************************************************/

int getAzimuthalArrayModes(gsl_matrix *mIn) {

  int ii,
    numRows = mIn->size1,
    numCols = mIn->size2;

  double *data,
    norm;
  
  gsl_fft_real_wavetable * wavetableCols;
  gsl_fft_real_workspace * workspaceCols;

  wavetableCols = gsl_fft_real_wavetable_alloc(numCols - 1);
  workspaceCols = gsl_fft_real_workspace_alloc(numCols - 1);

  /* 
   * 1D FFT of each row, stride = 1 
   * Dividide by n and multiply by two to get the amplitude
   */
  for (ii = 0; ii < numRows; ii++) {

    data = &(mIn->data[ii*numCols+1]);
    gsl_fft_real_transform(data, 1, (size_t) numCols - 1, wavetableCols, workspaceCols);
    data = &(mIn->data[ii*numCols+1]);
    data[0] = data[0]/8;
    norm = 2/8.0/data[0];
    data[1] = sqrt(gsl_pow_2(data[1]) + gsl_pow_2(data[2]))*norm;
    data[2] = sqrt(gsl_pow_2(data[3]) + gsl_pow_2(data[4]))*norm;
    data[3] = sqrt(gsl_pow_2(data[5]) + gsl_pow_2(data[6]))*norm;
    
  }


  gsl_fft_real_wavetable_free(wavetableCols);
  gsl_fft_real_workspace_free(workspaceCols);

  return 0;

}


/******************************************************************************
 * Function: getM0Mode
 * Inputs: int, std::string
 * Returns: gsl_vector *
 * Description: Will return the m=0 mode for the 0 degree azimuthal probe
 * passed to it.
 ******************************************************************************/

gsl_vector *getM0Mode(int shotNumber, std::string nodeName) {

  gsl_vector *m0Mode;
  int vecSize;
  
  gsl_matrix *azimuthalArray = get8AzimuthalArray(shotNumber, nodeName);
  getAzimuthalArrayModes(azimuthalArray);

  vecSize = azimuthalArray->size1;
  m0Mode = gsl_vector_alloc(vecSize);
  gsl_matrix_get_col(m0Mode, azimuthalArray, 1);

  return m0Mode;

}


/******************************************************************************
 * Function: getM1Mode
 * Inputs: int, std::string
 * Returns: gsl_vector *
 * Description: Will return the m=1 mode for the 0 degree azimuthal probe
 * passed to it.
 ******************************************************************************/

gsl_vector *getM1Mode(int shotNumber, std::string nodeName) {

  gsl_vector *m1Mode;
  int vecSize;
  
  gsl_matrix *azimuthalArray = get8AzimuthalArray(shotNumber, nodeName);
  getAzimuthalArrayModes(azimuthalArray);

  vecSize = azimuthalArray->size1;
  m1Mode = gsl_vector_alloc(vecSize);
  gsl_matrix_get_col(m1Mode, azimuthalArray, 2);

  return m1Mode;

}


/******************************************************************************
 * Function: getOffAxisDisplacement
 * Inputs: gsl_matrix *
 * Returns: gsl_matrix *
 * Description: Receives a matrix that has 9 columns, the first being the time
 * value, and the next 8 being the 8 different azimuthal magnetic probe data.
 * This is similiar to getAzimuthalArrayModes, but it will return the y
 * position of the current centroid. The return value will be a 3 column
 * matrix. The first column being the time value, the next being the x value
 * and the 3rd being the x value. m1/m0 = 2 * dr / r
 ******************************************************************************/

gsl_matrix *getOffAxisDisplacement(gsl_matrix *mIn) {

  int ii,
    numRows = mIn->size1,
    numCols = mIn->size2;

  double *data,
    m0,
    m1,
    angle,
    dr,
    xValue,
    yValue;
  
  gsl_matrix *mRet = gsl_matrix_alloc(numRows, 3);

  gsl_fft_real_wavetable * wavetableCols;
  gsl_fft_real_workspace * workspaceCols;

  wavetableCols = gsl_fft_real_wavetable_alloc(numCols - 1);
  workspaceCols = gsl_fft_real_workspace_alloc(numCols - 1);

  /* 
   * 1D FFT of each row, stride = 1 
   * Dividide by n and multiply by two to get the amplitude
   */
  for (ii = 0; ii < numRows; ii++) {

    data = &(mIn->data[ii*numCols+1]);
    gsl_fft_real_transform(data, 1, (size_t) numCols - 1, wavetableCols, workspaceCols);
    data = &(mIn->data[ii*numCols+1]);
    m0 = data[0]/8.0;
    m1 = sqrt(gsl_pow_2(data[1]) + gsl_pow_2(data[2]))*2/8.0;
    dr = m1/m0*RW*0.5;
    angle = atan2(-data[2], data[1])+0.3927;
    xValue =dr*cos(angle);
    yValue =dr*sin(angle);
    gsl_matrix_set(mRet, ii, 0, gsl_matrix_get(mIn, ii, 0));
    gsl_matrix_set(mRet, ii, 1, xValue);
    gsl_matrix_set(mRet, ii, 2, yValue);
    
  }


  gsl_fft_real_wavetable_free(wavetableCols);
  gsl_fft_real_workspace_free(workspaceCols);

  return mRet;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testAziMode();
static int testOffAxis();

int testMagnetic() {

  testAziMode();

  testOffAxis();

  return 0;

}


static int testOffAxis() {

  int ii, n = 8;
  double data[n];
  double val;

  gsl_matrix *testData = gsl_matrix_alloc(1, n+1);

  double m0 = 3.4,
    ms1 = 1.4,
    mc1 = 0.8,
    ms2 = 0.65,
    mc2 = 0.4,
    ms3 = 1.8,
    mc3 = 0.6,
    m1 = sqrt(gsl_pow_2(ms1) + gsl_pow_2(mc1));
  
  double angleOffset = 0.3927;

  for (ii = 0; ii < n; ii++) {

    val = m0 +ms1*gsl_sf_sin(1*ii*2*M_PI/8+angleOffset)+ms2*gsl_sf_sin(2*ii*2*M_PI/8+angleOffset)\
      +ms3*gsl_sf_sin(3*ii*2*M_PI/8+angleOffset)\
      +mc1*gsl_sf_cos(1*ii*2*M_PI/8+angleOffset)+mc2*gsl_sf_cos(2*ii*2*M_PI/8+angleOffset)\
      +mc3*gsl_sf_cos(3*ii*2*M_PI/8+angleOffset);
    data[ii] = val;
    gsl_matrix_set(testData, 0, ii+1, val);

  }

  gsl_matrix *result = getOffAxisDisplacement(testData);

  printf("Y/X Value: %g (Should be %g)\n", 
	 gsl_matrix_get(result, 0, 2)/gsl_matrix_get(result, 0, 1), ms1/mc1);

  (void) data[0];
  (void) m1;

  gsl_matrix_free(testData);
  gsl_matrix_free(result);


  /* Pulling data from tree for 180517006 */

  int shotNumber = 180517006;
  std::string dataNodeX = "\\X_P15";
  std::string dataNodeY = "\\Y_P15";

  gsl_vector *xDataVec1,
    *yDataVec1,
    *dataVec;
  
  xDataVec1 = readMDSplusVector(shotNumber, dataNodeX, "fuze");
  yDataVec1 = readMDSplusVector(shotNumber, dataNodeY, "fuze");
  dataVec = readMDSplusVectorDim(shotNumber, dataNodeX, "fuze");

  int sizeVec = xDataVec1->size;

  gsl_vector *xDataVec2 = gsl_vector_alloc(sizeVec),
    *yDataVec2 = gsl_vector_alloc(sizeVec);

  /* Getting data */
  gsl_matrix *azimuthArray = get8AzimuthalArray(shotNumber, "\\b_p15_000");
  getAzimuthalArrayModes(azimuthArray);
  gsl_matrix *disp = getOffAxisDisplacement(azimuthArray);

  for (ii = 0; ii < sizeVec; ii++) {
    gsl_vector_set(dataVec, ii, gsl_matrix_get(disp, ii, 0));
    gsl_vector_set(xDataVec2, ii, 
		   gsl_matrix_get(disp, ii, 1)*1E-2);
    gsl_vector_set(yDataVec2, ii, 
		   gsl_matrix_get(disp, ii, 2)*1E-2);
  }

  plot2VectorData(dataVec, xDataVec1, "1", xDataVec2, "2", "set xrange [50E-6:60E-6]\n",
		  "data/offAxisTestData1.dat", "data/offAxisTestScript1.sh");
  plot2VectorData(dataVec, yDataVec1, "1", yDataVec2, "2", "set xrange [50E-6:60E-6]\n",
		  "data/offAxisTestData2.dat", "data/offAxisTestScript2.sh");

  return 0;

}

static int testAziMode() {
  
  int ii, n = 8;
  double data[n];
  double val;

  gsl_matrix *testData = gsl_matrix_alloc(1, n+1);

  double m0 = 3.4,
    ms1 = 1.4,
    mc1 = 0.8,
    ms2 = 0.65,
    mc2 = 0.4,
    ms3 = 1.8,
    mc3 = 0.6,
    m1 = sqrt(gsl_pow_2(ms1) + gsl_pow_2(mc1)),
    m2 = sqrt(gsl_pow_2(ms2) + gsl_pow_2(mc2)),
    m3 = sqrt(gsl_pow_2(ms3) + gsl_pow_2(mc3));
  
  for (ii = 0; ii < n; ii++) {

    val = m0 +ms1*gsl_sf_sin(1*ii*2*M_PI/8)+ms2*gsl_sf_sin(2*ii*2*M_PI/8)\
      +ms3*gsl_sf_sin(3*ii*2*M_PI/8)\
      +mc1*gsl_sf_cos(1*ii*2*M_PI/8)+mc2*gsl_sf_cos(2*ii*2*M_PI/8)\
      +mc3*gsl_sf_cos(3*ii*2*M_PI/8);
    data[ii] = val;
    gsl_matrix_set(testData, 0, ii+1, val);

  }

  getAzimuthalArrayModes(testData);

  printf("m = 0: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 1), m0);
  printf("m = 1: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 2), m1/m0);
  printf("m = 2: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 3), m2/m0);
  printf("m = 3: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 4), m3/m0);

  (void) data[0];

  gsl_matrix_free(testData);
  
  return 0;

}
