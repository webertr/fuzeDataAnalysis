#include <DHI/mdsplusDHI.h>

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
 * Function: readDHIMDSplusImage
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_matrix *readDHIMDSplusImage(int shotNumber, char *nodeName) {

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
  
  char *treeName = "my_tree",
    buf[1024];

  gsl_matrix *nullM = 0;
  
  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullM;
  }
  
  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullM;
  }

  snprintf(buf,sizeof(buf)-1,"shape(%s)",nodeName);
  size = getSignalLength(buf);
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue(buf, &sigDescrShape, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
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
 * Function: readDHIMDSplusDimension
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_vector *readDHIMDSplusDimension(int shotNumber, char *nodeName, int dimNum) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrShape,
    sigDescrDim,
    numRowsDim,
    numColsDim,
    ii;
    
  float *shape,
    *dim;
  
  char *treeName = "my_tree",
    buf[1024];

  gsl_vector *nullVec = 0;
  
  connectionID = MdsConnect("localhost");
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

  snprintf(buf,sizeof(buf)-1,"shape(DIM_OF(%s))",nodeName);
  size = getSignalLength(buf);
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue("shape(DIM_OF(DHI:LINE_INT))", &sigDescrShape, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullVec;

  }
  numRowsDim = (int) shape[0];
  numColsDim = (int) shape[1];
  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName);
  size = getSignalLength(buf);
  dim = (float *)malloc(size * sizeof(float));  
  sigDescrDim = descr(&dtype_float, dim, &size, &null);
  connectionStatus = MdsValue(buf, &sigDescrDim, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    MdsDisconnect();
    return nullVec;
  }

  gsl_vector *vRet = gsl_vector_alloc(numRowsDim);
  
  connectionStatus = MdsClose(treeName, &shotNumber);

  (void) numColsDim; // Suppresses compiler warning
  for (ii = 0; ii < numRowsDim; ii++) {
    gsl_vector_set(vRet, ii, dim[ii+dimNum*numRowsDim]);
  }
  
  return vRet;

}






/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int mdsplusReadTest() {

  gsl_matrix *test = readDHIMDSplusImage(1, "DHI:LINE_INT");

  int numRows = test->size1,
    numCols = test->size2,
    ii, jj;

  printf("Image:\n");
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("|%g| ", gsl_matrix_get(test, ii, jj));
    }
    printf("\n------------------\n");
  }

  gsl_vector *testVec1 = readDHIMDSplusDimension(1, "DHI:LINE_INT", 0);

  printf("r vector:\n");
  for (ii = 0; ii < (testVec1->size); ii++) {
    printf("%g\n", gsl_vector_get(testVec1, ii));
    printf("--\n");
  }

  gsl_vector *testVec2 = readDHIMDSplusDimension(1, "DHI:LINE_INT", 1);

  printf("z vector:\n");
  for (ii = 0; ii < (testVec2->size); ii++) {
    printf("%g\n", gsl_vector_get(testVec2, ii));
    printf("--\n");
  }

  return 0;
  
}

/***********************************
To populate this tree:
webertr@fuze2:~/Documents/my_tree$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/my_tree$ mdstcl 
TCL> edit my_tree/new
TCL> add node .DHI
TCL> add node .DHI:LINE_INT/usage=SIGNAL
TCL> add node .DHI:LINE_INT:RAW/usage=NUMERIC
TCL> add node .DHI:LINE_INT:R/usage=SIGNAL
TCL> add node .DHI:LINE_INT:Z/usage=SIGNAL
TCL> put .DHI:LINE_INT:RAW "[[1.1,1.1,1.1], [2.2, 2.2, 2.4], [5.5, 5.6, 5.7]]"
TCL> put DHI:LINE_INT:R "build_signal(build_with_units([0, .1, .2],'m'),,)"
TCL> put DHI:LINE_INT:Z "build_signal(build_with_units([0, .2, .4],'m'),,)"
TCL> put DHI:LINE_INT "build_signal(build_with_units(DHI:LINE_INT:RAW,'m^-3'),,[DHI:LINE_INT:R,DHI:LINE_INT:Z])"
TCL> write
TCL> close
TCL> set tree my_tree
TCL> create pulse 1
TCL> quit
webertr@fuze2:~/Documents/my_tree$ tdic 
TDI> treeopen("my_tree", 1)
265388041
TDI> data(DHI:LINE_INT)
[[1.1,1.1,1.1], [2.2,2.2,2.4], [5.5,5.6,5.7]]
TDI> data(DHI:LINE_INT:R)
[0.,.1,.2]
TDI> data(DHI:LINE_INT:Z)
[0.,.2,.4]
TDI> data(units_of(DHI:LINE_INT:Z))
"m"
TDI> data(units_of(DHI:LINE_INT))
"m^-3"
TDI> data(units_of(DHI:LINE_INT:R))
"m"
TDI> data(shape(DHI:LINE_INT:R))
[3]
TDI> data(shape(DHI:LINE_INT:Z))
[3]
TDI> data(shape(DHI:LINE_INT))
[3,3]
TDI> data(shape(DIM_OF(DHI:LINE_INT)))
[3,2]
TDI> data(shape(DIM_OF(DHI:LINE_INT:R)))
[3]
TDI> data(shape(DIM_OF(DHI:LINE_INT:Z)))
[3]


Then, to run the server:
webertr@fuze2:~/Documents/mdsplusCTest$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/mdsplusCTest$ mdsip -p 8000 -m -h mdsip.hosts


with mdsip.hosts:
/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=Thomas W. Fredian/Email=twf@psfc.mit.edu | twf
* | MAP_TO_LOCAL
* | webertr


Then, finally, 

gcc -Wall -I/usr/local/mdsplus/include -L/usr/local/mdsplus/lib -lMdsLib -lMdsShr main.c -o run


******************************************************************************/
