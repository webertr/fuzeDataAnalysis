#include "dataAnalysis.h"

/******************************************************************************
 * Function: status_ok
 * Inputs: int
 * Returns: int
 * Description: Returns 1 if OK, 0 otherwise. Status is OK if the LSB is set.
******************************************************************************/

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
 * Function: getElement
 * Inputs: dataVector*, int
 * Returns: double
 * Description: 
 ******************************************************************************/

double getElement (dataVector *dataVecIn, int elementNum) {

  return gsl_vector_get(dataVecIn->data, elementNum);

}


/******************************************************************************
 * Function: saveData
 * Inputs: dataVector*, char *
 * Returns: int
 * Description: Usage:
 ******************************************************************************/

int saveData (dataVector *dataVecIn) {

  char fileName[50];
  char *data = "data/";
  char *plot = "_plot.txt";
  char name[20];
  strcpy(name, dataVecIn->nodeName);
  strcpy(name, &(name[1]));

  strcpy(fileName, data);
  strcat(fileName, name);
  strcat(fileName, plot);

  FILE *fp = fopen(fileName, "w");

  if ( (fp == NULL) ) {

    printf("Error opening files!\n");
    exit(1);

  }

  int ii;
  for (ii = 0; ii < dataVecIn->length; ii++) {

    fprintf(fp, "%f \t %f\n", (float) (dataVecIn->deltaT * ii), 
	    (float) dataVecIn->getElement(dataVecIn, ii));

  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: getMagneticData
 * Inputs: dataVector*, char*
 * Returns: dataVector *
 * Description: Example usage:
 * dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");
 * printf("Data: %d, %f\n", data->length, data->deltaT);
 * printf("Node Name: %s\n", data->nodeName);
 * printf("20th Element: %f\n", data->getElement(data, 20));
 ******************************************************************************/

dataVector* initializeMagneticData (int shotNumber, char *nodeName) {

  /* local vars */
  int dtype_float = DTYPE_FLOAT;
  int null = 0;
  int connectionStatus;
  char buf[1024];       // Used to get DIM_OF of the array
  float *timeBase;      // array of floats used for timeBase
  int signalDescriptor; // signal descriptor
  int timeDescriptor;   // descriptor for timeBase
  int sizeArray;        // length of signal
  int len;
  int connectionID;     // Connection ID for mdsplus connection
  double deltaT;        // A single time step;
  
  /* Initializing struct to return everything */
  dataVector *structReturn = malloc(sizeof(dataVector));

  /* Setting node name */
  strcpy(structReturn->nodeName, nodeName);

  /* Setting shot number */
  structReturn->shotNumber = shotNumber;

  /* Setting get element function */
  structReturn->getElement = &getElement;

  /* Setting get element function */
  structReturn->saveData = &saveData;

  /* Return this if there is an error */
  dataVector *nullReturn = NULL;
  
  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {

    fprintf(stderr, "Connection Failed\n");
    return nullReturn;

  }

  /* Opening a tree */
  connectionStatus = MdsOpen("fuze", &shotNumber);

  /* Checking to see if opened correctly */
  if ( !status_ok(connectionStatus) ) {

    fprintf(stderr,"Error opening tree for shot: %d.\n",shotNumber);
    return nullReturn;

  }

  /* Getting signal length of */
  sizeArray = get_signal_length(nodeName);

  if ( sizeArray < 1 ) {

    fprintf(stderr,"Error retrieving length of signal\n");
    return nullReturn;

  }

  /* Setting length member in struct */
  structReturn->length = sizeArray;
  
  /* use malloc() to allocate memory for the timeBase */
  timeBase = (float *)malloc(sizeArray * sizeof(float));

  if ( !timeBase ) {
    
    fprintf(stderr,"Error allocating memory for timeBase\n");
    return nullReturn;

  }
 
  /* create a descriptor for the timeBase */
  timeDescriptor = descr(&dtype_float, timeBase, &sizeArray, &null);

  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName);

  /* retrieve timeBase of signal */
  connectionStatus = MdsValue(buf, &timeDescriptor, &null, 0);

  /* Verify that time base was read okay */
  if ( !status_ok(connectionStatus) ) {

    /* error */
    fprintf(stderr,"Error retrieving timeBase\n");
    free( (void *)timeBase );
    return nullReturn;

  }

  /* Getting time difference in micro seconds */
  deltaT = (timeBase[sizeArray]-timeBase[0])*1E6/sizeArray;

  /* Setting timebase of struct */
  structReturn->deltaT = deltaT;
  
  /* Freeing time base, no longer needed */
  free( (void *)timeBase );

  gsl_vector_float* tempVec = gsl_vector_float_alloc(sizeArray);
  gsl_vector* structVec = gsl_vector_alloc(sizeArray);
  
  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_float, tempVec->data, &sizeArray, &null);
 
  /* retrieve signal */
  connectionStatus = MdsValue(nodeName, &signalDescriptor, &null, &len);

  if ( !status_ok(connectionStatus) ) {
    
    gsl_vector_float_free(tempVec);
    gsl_vector_free(structVec);
    fprintf(stderr,"Error retrieving signal\n");
    return nullReturn;
    
  }

  /* Converting float vector to double vector */
  int ii;
  for (ii = 0; ii < sizeArray; ii++) {

    gsl_vector_set(structVec, ii,
		   (double) gsl_vector_float_get(tempVec, ii));

  }

  /* Setting vector data in struct */
  structReturn->data = structVec;
  
  /* Freeing temporary vector data */
  gsl_vector_float_free(tempVec);
  
  return structReturn;

}


/******************************************************************************
 * Function: dataStructPlotTest
 * Inputs: int, char *
 * Returns: int
 * Description: This will just show how to use the data struct in this framework.
 ******************************************************************************/

int dataStructPlotTest(int shotNumber, char *nodeName) {

  /* 
   * struct containing all the data
   */
  dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");

  printf("Data: %d, Delta T: %f\n", data->length, data->deltaT);

  printf("Node Name: %s\n", data->nodeName);

  printf("20th Element: %f\n", data->getElement(data, 20));

  /* Saves as the data->nodeName + _plot.txt */
  data->saveData(data);

  system("script/plot_struct_test.sh");

  return 0;

}
