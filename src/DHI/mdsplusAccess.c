#include <DHI/mdsplusAccess.h>

/******************************************************************************
 * Example Usage:
 ******************************************************************************/


/******************************************************************************
 * Function: statusOk
 * Inputs: int
 * Returns: int
 * Description: Returns 1 if OK, 0 otherwise. Status is OK if the LSB is set.
 ******************************************************************************/

static int statusOk( int status ) {

  return ( (status & 1) == 1 );

}


/******************************************************************************
 * Function: getSignalLength
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 * if not successfull. Assumes that the user is already connected to the MDSplus
 * server, and that the tree is already open. Uses the tdi intrinsic function
 * SIZE() to get the signal length
******************************************************************************/

static int getSignalLength(const char *signal) {

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
 * Function: writeDHIMDSplusMatrix
 * Inputs: gsl_matrix *, int, char *
 * Returns: int
 * Description: This will write an image to the DHI tree in mdsplus.
 * For this raw data, expression can just be:
 * expression = "$1"
 ******************************************************************************/

int writeDHIMDSplusMatrix(gsl_matrix* image, char *nodeName, char *expression, int shotNumber,
			  char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    dTypeDouble = DTYPE_DOUBLE,
    null = 0,
    sigDescrImage,
    ii, jj,
    numRows = image->size1,
    numCols = image->size2;

  double *data = (double *)malloc(numRows*numCols * sizeof(double));
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      data[ii+jj*numRows] = gsl_matrix_get(image, ii, jj);
    }
  }
  sigDescrImage = descr(&dTypeDouble, data, &numRows, &numCols, &null);

  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsPut(nodeName, "$1", &sigDescrImage, &null);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsPut error (%s). Message: %s.\n", nodeName, MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  return 0;

}


/******************************************************************************
 * Function: writeDHIMDSplusImage
 * Inputs: gsl_matrix *, int, char *
 * Returns: int
 * Description: This will write an image to the DHI tree in mdsplus.
 * For this raw data, expression can just be:
 * expression = "$1"
 ******************************************************************************/

int writeDHIMDSplusImage(gsl_matrix* image, char *nodeName, char *expression, int shotNumber,
			 char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    dTypeWord = DTYPE_WU,
    null = 0,
    sigDescrImage,
    ii, jj,
    numRows = image->size1,
    numCols = image->size2;

  double maxVal = gsl_matrix_max(image);
  short *data = (short *)malloc(numRows*numCols * sizeof(short));
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      data[ii+jj*numRows] = (short) (16000*gsl_matrix_get(image, ii, jj)/maxVal);
    }
  }
  sigDescrImage = descr(&dTypeWord, data, &numRows, &numCols, &null);

  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsPut(nodeName, "$1", &sigDescrImage, &null);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsPut error (%s). Message: %s.\n", nodeName, MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  return 0;

}


/******************************************************************************
 * Function: writeDHIMDSplusVector
 * Inputs: gsl_vector *, char *, char *, int
 * Returns: int
 * Description: This will write an image to the DHI tree in mdsplus.
 * Expression could be:
 * "build_signal(build_with_units($1,'m'),,)";
 ******************************************************************************/

int writeDHIMDSplusVector(gsl_vector *vecIn, char *nodeName, char *expression, int shotNumber,
			  char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    dTypeDouble = DTYPE_DOUBLE,
    null = 0,
    sigDescrVector,
    numRows = vecIn->size;
  
  sigDescrVector = descr(&dTypeDouble, vecIn->data, &numRows, &null);

  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsPut(nodeName, expression, &sigDescrVector, &null);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsPut error (%s). Message: %s.\n", nodeName, MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return -1;
  }
   
  connectionStatus = MdsClose(treeName, &shotNumber);
  
  return 0;

}


/******************************************************************************
 * Function: readDHIMDSplusImage
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_matrix *readDHIMDSplusImage(int shotNumber, char *nodeName, char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrShape,
    sigDescrData,
    numRows,
    numCols,
    ii, jj;
    
  float *shape,
    *data;
  
  char buf[1024];

  gsl_matrix *nullM = 0;
  
  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullM;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullM;
  }

  snprintf(buf,sizeof(buf)-1,"shape(%s)",nodeName);
  size = getSignalLength(buf);
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue(buf, &sigDescrShape, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsValue error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullM;

  }
  numRows = (int) shape[0];
  numCols = (int) shape[1];
  shape = (float *)malloc(size * sizeof(float));  
  gsl_matrix *mRet = gsl_matrix_alloc(numRows, numCols);
  size = getSignalLength(nodeName);
  data = (float *)malloc(size * sizeof(float));  
  sigDescrData = descr(&dtype_float, data, &size, &null);
  connectionStatus = MdsValue(nodeName, &sigDescrData, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullM;

  }

 
  connectionStatus = MdsClose(treeName, &shotNumber);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_set(mRet, ii, jj, data[ii+jj*numRows]);
    }
  }
  
  return mRet;

}


/******************************************************************************
 * Function: readDHIMDSplusVector
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_vector *readDHIMDSplusVector(int shotNumber, char *nodeName, char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrVector,
    ii;
    
  float *vectorData;
  
  gsl_vector *nullVec = 0;
  
  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullVec;
  }
  
  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullVec;
  }

  size = getSignalLength(nodeName);
  vectorData = (float *)malloc(size * sizeof(float));  
  sigDescrVector = descr(&dtype_float, vectorData, &size, &null);
  connectionStatus = MdsValue(nodeName, &sigDescrVector, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullVec;

  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  gsl_vector *vRet = gsl_vector_alloc(size);  
  connectionStatus = MdsClose(treeName, &shotNumber);
  for (ii = 0; ii < size; ii++) {
    gsl_vector_set(vRet, ii, vectorData[ii]);
  }
  
  return vRet;

}


/******************************************************************************
 * Function: readDHIMDSplusVectorDim
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_vector *readDHIMDSplusVectorDim(int shotNumber, char *nodeName, char *treeName, char *host) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrVector,
    ii;

  char buf[1024];

  float *vectorData;
  
  gsl_vector *nullVec = 0;
  
  connectionID = MdsConnect(host);
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullVec;
  }
  
  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullVec;
  }

  size = getSignalLength(nodeName);
  vectorData = (float *)malloc(size * sizeof(float));  
  sigDescrVector = descr(&dtype_float, vectorData, &size, &null);
  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName);
  connectionStatus = MdsValue(buf, &sigDescrVector, &null, 0);

  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullVec;

  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  gsl_vector *vRet = gsl_vector_alloc(size);  
  connectionStatus = MdsClose(treeName, &shotNumber);
  for (ii = 0; ii < size; ii++) {
    gsl_vector_set(vRet, ii, vectorData[ii]);
  }
  
  return vRet;

}






/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int mdsplusReadTest() {

  int shotNumber = 12345;
  int numRows = 5,
    numCols = 10,
    ii, jj;

  char *host = "localhost",
    *treeName = "my_tree";
  gsl_matrix *image = gsl_matrix_alloc(numRows, numCols);
  gsl_vector *rVec = gsl_vector_alloc(numRows);
  gsl_vector *zVec = gsl_vector_alloc(numCols);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_set(image, ii, jj, (double) (ii*10+jj));
    }
  }
  for (jj = 0; jj < numCols; jj++) {
    gsl_vector_set(zVec, jj, jj/50.0);
  }
  for (ii = 0; ii < numRows; ii++) {
    gsl_vector_set(rVec, ii, ii/20.0);
  }
  
  writeDHIMDSplusMatrix(image, "DHI:LINE_INT:RAW", "$1", shotNumber, treeName, host);
  writeDHIMDSplusVector(rVec, "DHI:LINE_INT:R", "build_signal(build_with_units($1,'m'),,)",
			shotNumber, treeName, host);
  writeDHIMDSplusVector(zVec, "DHI:LINE_INT:Z", "build_signal(build_with_units($1,'m'),,)",
			shotNumber, treeName, host);
  
  gsl_matrix *imageRead = readDHIMDSplusImage(shotNumber, "DHI:LINE_INT:RAW", treeName, host);

  printf("Image Write:\n");
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("|%g| ", gsl_matrix_get(image, ii, jj));
    }
    printf("\n------------------\n");
  }

  printf("Image Read:\n");
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("|%g| ", gsl_matrix_get(imageRead, ii, jj));
    }
    printf("\n------------------\n");
  }

  gsl_vector *rVecRead = readDHIMDSplusVector(shotNumber, "DHI:LINE_INT:R", treeName, host);

  printf("r vector Write:\n");
  for (ii = 0; ii < (rVec->size); ii++) {
    printf("%g\n", gsl_vector_get(rVec, ii));
    printf("--\n");
  }

  printf("r vector Read:\n");
  for (ii = 0; ii < (rVecRead->size); ii++) {
    printf("%g\n", gsl_vector_get(rVecRead, ii));
    printf("--\n");
  }

  gsl_vector *zVecRead = readDHIMDSplusVector(shotNumber, "DHI:LINE_INT:Z", treeName, host);

  printf("z vector Write:\n");
  for (ii = 0; ii < (zVec->size); ii++) {
    printf("%g\n", gsl_vector_get(zVec, ii));
    printf("--\n");
  }

  printf("z vector Read:\n");
  for (ii = 0; ii < (zVecRead->size); ii++) {
    printf("%g\n", gsl_vector_get(zVecRead, ii));
    printf("--\n");
  }

  return 0;
  
}

/**********************************************************************************
To populate this tree:
webertr@fuze2:~/Documents/my_tree$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/my_tree$ mdstcl 
TCL> @script

Then, to run the server:
webertr@fuze2:~/Documents/mdsplusCTest$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/mdsplusCTest$ mdsip -p 8000 -m -h mdsip.hosts

with mdsip.hosts:
/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=Thomas W. Fredian/Email=twf@psfc.mit.edu | twf
* | MAP_TO_LOCAL
* | webertr

Here is the script I ran to update the fuze tree:

fuze@fuze:~/Documents/weberTest$ more mdsplusUpdate.sh 
set tree fuze
create pulse 12345
edit fuze/shot=12345
add node .SIGNALS.HOLOGRAPHY:NL_DHI/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NL_DHI:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:NL_DHI:R/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NL_DHI:Y/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:DHI_IMAGE/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:DHI_IMAGE:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:DHI_IMAGE:Y/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:DHI_IMAGE:Z/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NE_DHI/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NE_DHI:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:NE_DHI:R/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NE_DHI:Z/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NE_DHI:ERROR/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:NE_DHI:ERROR:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:T_DHI/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:T_DHI:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:T_DHI:R/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:T_DHI:Z/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:T_DHI:ERROR/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:T_DHI:ERROR:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI:RAW/usage=NUMERIC
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI:R/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI:Z/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI:ERROR/usage=SIGNAL
add node .SIGNALS.HOLOGRAPHY:B_THETA_DHI:ERROR:RAW/usage=NUMERIC
add tag .SIGNALS.HOLOGRAPHY:NL_DHI NL_DHI
add tag .SIGNALS.HOLOGRAPHY:DHI_IMAGE DHI_IMAGE
add tag .SIGNALS.HOLOGRAPHY:DHI_IMAGE IMAGE_DHI
add tag .SIGNALS.HOLOGRAPHY:NE_DHI NE_DHI
add tag .SIGNALS.HOLOGRAPHY:T_DHI T_DHI
add tag .SIGNALS.HOLOGRAPHY:B_THETA_DHI B_THETA_DHI
put SIGNALS.HOLOGRAPHY:NL_DHI "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:NL_DHI:RAW,'m^-2'),,)"
put SIGNALS.HOLOGRAPHY:DHI_IMAGE "build_signal(SIGNALS.HOLOGRAPHY:DHI_IMAGE:RAW,,)"
put SIGNALS.HOLOGRAPHY:NE_DHI "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:NE_DHI:RAW,'m^-3'),,)"
put SIGNALS.HOLOGRAPHY:NE_DHI:ERROR "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:NE_DHI:ERROR:RAW,'m^-3'),,)"
put SIGNALS.HOLOGRAPHY:T "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:T_DHI:RAW,'eV'),,)"
put SIGNALS.HOLOGRAPHY:T:ERROR "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:T_DHI:ERROR:RAW,'eV'),,)"
put SIGNALS.HOLOGRAPHY:B_THETA_DHI "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:B_THETA_DHI:RAW,'T'),,)"
put SIGNALS.HOLOGRAPHY:B_THETA_DHI:ERROR "build_signal(build_with_units(SIGNALS.HOLOGRAPHY:B_THETA_DHI:ERROR:RAW,'T'),,)"
write
close



******************************************************************************/
