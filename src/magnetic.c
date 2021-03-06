#include "magnetic.h"

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

static char *replaceWord(const char *s, const char *oldW, const char *newW) {

  char *result;
  int i, cnt = 0;
  int newWlen = strlen(newW);
  int oldWlen = strlen(oldW);
 
  // Counting the number of times old word
  // occur in the string
  for (i = 0; s[i] != '\0'; i++) {
    if (strstr(&s[i], oldW) == &s[i]) {
      cnt++;
 
      // Jumping to index after the old word.
      i += oldWlen - 1;
    }
  }
 
  // Making new string of enough length
  result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
 
  i = 0;
  while (*s) {
    
    // compare the substring with the result
    if (strstr(s, oldW) == s) {
      strcpy(&result[i], newW);
      i += newWlen;
      s += oldWlen;
    } else
      result[i++] = *s++;
  }
 
  result[i] = '\0';
  return result;
}

static int status_ok( int status ) {

  return ( (status & 1) == 1 );

}


/******************************************************************************
 * Function: get_signal_length
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 * if not successfull. Assumes that the user is already connected to the MDSplus
 * server, and that the tree is already open. Uses the tdi intrinsic function
 * SIZE() to get the signal length
******************************************************************************/

static int get_signal_length(const char *signal) {

  int dtype_long = DTYPE_LONG;
  char buf[1024];
  int size;
  int null = 0;
  int idesc = descr(&dtype_long, &size, &null);
  int status;

  /* init buffer */
  memset(buf,0,sizeof(buf));

  /* put SIZE() TDI function around signal name */
  snprintf(buf,sizeof(buf)-1,"SIZE(%s)",signal);

  /* use MdsValue to get the signal length */
  status = MdsValue(buf, &idesc, &null, 0);

  if ( !( (status & 1) == 1 ) ) {

    fprintf(stderr,"Unable to get length of %s.\n",signal);
    return -1;

  }

  /* return signal length */
  return size;

}


/******************************************************************************
 * Function: getMagneticDataAndTime
 * Inputs: dataVector*, char*
 * Returns: dataVector *
 * Description: Example usage:
 * dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");
 * printf("Data: %d, %f\n", data->length, data->deltaT);
 * printf("Node Name: %s\n", data->nodeName);
 * printf("20th Element: %f\n", data->getElement(data, 20));
 ******************************************************************************/

int initializeMagneticDataAndTime (int shotNumber, char *nodeName, gsl_vector *data,
				   gsl_vector *time) {

  /* local vars */
  int dtype_dbl = DTYPE_DOUBLE;
  int null = 0;
  int connectionStatus;
  char buf[1024];                 // Used to get DIM_OF of the array
  int signalDescriptor;           // signal descriptor
  int timeDescriptor;             // descriptor for timeBase
  int sizeArray = data->size;     // length of signal
  int len;
  int connectionID;               // Connection ID for mdsplus connection
  
  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    fprintf(stderr, "Connection Failed\n");
    return -1;

  }

  /* Opening a tree */
  connectionStatus = MdsOpen("fuze", &shotNumber);

  /* Checking to see if opened correctly */
  if ( !status_ok(connectionStatus) ) {

    fprintf(stderr,"Error opening tree for shot: %d.\n",shotNumber);
    MdsDisconnect();
    return -1;

  }


  /* create a descriptor for the timeBase */
  timeDescriptor = descr(&dtype_dbl, time->data, &sizeArray, &null);

  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName);

  /* retrieve timeBase of signal */
  connectionStatus = MdsValue(buf, &timeDescriptor, &null, 0);

  /* Verify that time base was read okay */
  if ( !status_ok(connectionStatus) ) {

    /* error */
    fprintf(stderr,"Error retrieving timeBase\n");
    MdsDisconnect();
    return -1;

  }

  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, data->data, &sizeArray, &null);
 
  /* retrieve signal */
  connectionStatus = MdsValue(nodeName, &signalDescriptor, &null, &len);

  if ( !status_ok(connectionStatus) ) {
    
    fprintf(stderr,"Error retrieving signal\n");
    fprintf(stderr,"Signal length: %d\n", get_signal_length(nodeName));
    MdsDisconnect();
    return -1;
    
  }

  /* Disconnecting from MDSplus */
  MdsDisconnect();

  return 0;

}


/******************************************************************************
 * Function: getMagneticDataAndTime
 * Inputs: dataVector*, char*
 * Returns: dataVector *
 * Description: Example usage:
 * dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");
 * printf("Data: %d, %f\n", data->length, data->deltaT);
 * printf("Node Name: %s\n", data->nodeName);
 * printf("20th Element: %f\n", data->getElement(data, 20));
 ******************************************************************************/

int initializeMagneticData(int shotNumber, char *nodeName, gsl_vector *data) {

  /* local vars */
  int dtype_dbl = DTYPE_DOUBLE;
  int null = 0;
  int connectionStatus;
  int signalDescriptor;              // signal descriptor
  int sizeArray = data->size;        // length of signal
  int len;
  int connectionID;                  // Connection ID for mdsplus connection

  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    fprintf(stderr, "Connection Failed\n");
    return -1;

  }

  /* Opening a tree */
  connectionStatus = MdsOpen("fuze", &shotNumber);

  /* Checking to see if opened correctly */
  if ( !status_ok(connectionStatus) ) {

    fprintf(stderr,"Error opening tree for shot: %d.\n",shotNumber);
    MdsDisconnect();
    return -1;

  }

  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, data->data, &sizeArray, &null);
 
  /* retrieve signal */
  connectionStatus = MdsValue(nodeName, &signalDescriptor, &null, &len);

  if ( !status_ok(connectionStatus) ) {
    
    fprintf(stderr,"Error retrieving signal\n");
    MdsDisconnect();
    return -1;
    
  }

  /* Disconnecting from MDSplus */
  MdsDisconnect();
  
  return 0;

}




/******************************************************************************
 * Function: getSignalLengthMDSplus
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 * if not successfull.
 ******************************************************************************/

int getSignalLengthMDSplus(const char *signal, int shotNumber) {

  char buf[1024];
  int size,
    null = 0,
    dtype_long = DTYPE_LONG,
    idesc = descr(&dtype_long, &size, &null),
    status,
    connectionStatus,
    connectionID;
  
  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    fprintf(stderr, "Connection Failed\n");
    return -1;

  }

  /* Opening a tree */
  connectionStatus = MdsOpen("fuze", &shotNumber);

  /* Checking to see if opened correctly */
  if ( !status_ok(connectionStatus) ) {

    fprintf(stderr,"Error opening tree for shot: %d.\n",shotNumber);
    MdsDisconnect();
    return -1;

  }

  /* init buffer */
  memset(buf,0,sizeof(buf));

  /* put SIZE() TDI function around signal name */
  snprintf(buf,sizeof(buf)-1,"SIZE(%s)",signal);

  /* use MdsValue to get the signal length */
  status = MdsValue(buf, &idesc, &null, 0);

  if ( !( (status & 1) == 1 ) ) {

    fprintf(stderr,"Unable to get length of %s.\n",signal);
    MdsDisconnect();
    return -1;

  }

  MdsDisconnect();
  
  /* return signal length */
  return size;

}



/******************************************************************************
 * Function: getAzimuthalArrayP15
 * Inputs: int
 * Returns: gsl_matrix
 * Description: Returns a matrix of the 8 azimuthal array of magnetic probes
 * at p15 in the fuze machine with time at the end. First column is the time
 * values, next 8 are the 8 azimuthal probes. Going down the rows represent
 * moving forward in time.
 ******************************************************************************/

gsl_matrix *getAzimuthalArray(int shotNumber, char *nodeName) {
  
  int sigSize = getSignalLengthMDSplus(nodeName, shotNumber);
  
  gsl_matrix *azimuthArray = gsl_matrix_alloc(sigSize, 9);
 
  /* Declaring variables */
  gsl_vector  *time = gsl_vector_alloc(sigSize),
    *p_000 = gsl_vector_alloc(sigSize),
    *p_045 = gsl_vector_alloc(sigSize),
    *p_090 = gsl_vector_alloc(sigSize),
    *p_135 = gsl_vector_alloc(sigSize),
    *p_180 = gsl_vector_alloc(sigSize),
    *p_225 = gsl_vector_alloc(sigSize),
    *p_270 = gsl_vector_alloc(sigSize),
    *p_315 = gsl_vector_alloc(sigSize);

  char *nodeName0 = nodeName,
    *nodeName45 = replaceWord(nodeName, "000", "045"),
    *nodeName90 = replaceWord(nodeName, "000", "090"),
    *nodeName135 = replaceWord(nodeName, "000", "135"),
    *nodeName180 = replaceWord(nodeName, "000", "180"),
    *nodeName225 = replaceWord(nodeName, "000", "225"),
    *nodeName270 = replaceWord(nodeName, "000", "270"),
    *nodeName315 = replaceWord(nodeName, "000", "315");

  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, nodeName0, p_000, time);
  initializeMagneticData(shotNumber, nodeName45, p_045);
  initializeMagneticData(shotNumber, nodeName90, p_090);
  initializeMagneticData(shotNumber, nodeName135, p_135);
  initializeMagneticData(shotNumber, nodeName180, p_180);
  initializeMagneticData(shotNumber, nodeName225, p_225);
  initializeMagneticData(shotNumber, nodeName270, p_270);
  initializeMagneticData(shotNumber, nodeName315, p_315);
  
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
 * Function: getCurrentPulseNumber
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
******************************************************************************/

int getCurrentPulseNumber() {

  int connectionID;
  int dtype_long = DTYPE_LONG;
  char *buf = "current_shot('fuze')";
  int shotNumber;
  int null = 0;
  int idesc = descr(&dtype_long, &shotNumber, &null);
  int status;

  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    fprintf(stderr, "Connection Failed\n");
    return -1;

  }


  /* use MdsValue to get the signal length */
  status = MdsValue(buf, &idesc, &null, 0);

  if ( !( (status & 1) == 1 ) ) {

    fprintf(stderr,"Unable to get pulse number.\n");
    return -1;

  }

  /* return signal length */
  return shotNumber;

}


/******************************************************************************
 * Function: getDHITime
 * Inputs: int
 * Returns: double
 * Description: Returns the dhi timing for the shot number passed
******************************************************************************/

double getDHITime(int shotNumber) {

  double dhiTime = -1;

  char *dhiTimeExp = "\\T_DHI_GATE",
    *treeName = "fuze";

  int connectionID,
    dtype_dbl = DTYPE_DOUBLE,
    null = 0,
    status,
    len,
    size, 
    sigdescDbl;

  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    MdsDisconnect();
    fprintf(stderr, "MDSplus connection Failed\n");
    return dhiTime;

  }

  /* Opening tree */
  status = MdsOpen(treeName, &shotNumber);

  if ( !status_ok(status) ) {

    MdsClose(treeName, &shotNumber);
    fprintf(stderr,"\nError opening tree in getDHITime\n");
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(status));
    return dhiTime;

  }


  size = get_signal_length(dhiTimeExp);
  sigdescDbl = descr(&dtype_dbl, &dhiTime, &size, &null);


  /* use MdsValue to get DHI Time */
  status = MdsValue(dhiTimeExp, &sigdescDbl, &null, &len);

  /* Checking status from the read */
  if ( !( (status & 1) == 1 ) ) {

    MdsClose(treeName, &shotNumber);
    fprintf(stderr,"\nMDSplus double read failed\n");
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(status));
    return dhiTime;

  }

  return dhiTime;

}



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testAziMode();
static int testDHIRead();
static int testOffAxis();

int testMagnetic() {

  if (0) {
    testAziMode();
    testDHIRead();
  }

  testOffAxis();

  return 0;

}

static int testDHIRead() {

  printf("DHI Time (should be 59E-5): %g\n", getDHITime(180517018));

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
  char *dataNodeX = "\\X_P15";
  char *dataNodeY = "\\Y_P15";
  int sizeVec = getSignalLengthMDSplus(dataNodeX, shotNumber);
  gsl_vector *xDataVec1 = gsl_vector_alloc(sizeVec),
    *xDataVec2 = gsl_vector_alloc(sizeVec),
    *yDataVec1 = gsl_vector_alloc(sizeVec),
    *yDataVec2 = gsl_vector_alloc(sizeVec),
    *dataVec = gsl_vector_alloc(sizeVec);
  initializeMagneticData(shotNumber, dataNodeX, xDataVec1);
  initializeMagneticData(shotNumber, dataNodeY, yDataVec1);

  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000");
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
