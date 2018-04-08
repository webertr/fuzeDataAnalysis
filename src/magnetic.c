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
 * Function: saveVectorData
 * Inputs: gsl_vector *, gsl_vector *
 * Returns: int
 * Description: Save vector data
 ******************************************************************************/

int saveVectorData (gsl_vector *xVec, gsl_vector *yVec, char *fileName) {

  FILE *fp = fopen(fileName, "w");

  if ( (fp == NULL) ) {

    printf("Error opening files!\n");
    exit(1);

  }

  int ii;
  for (ii = 0; ii < xVec->size; ii++) {

    fprintf(fp, "%g \t %g\n", gsl_vector_get(xVec, ii), 
	    gsl_vector_get(xVec, ii));

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

int initializeMagneticData (int shotNumber, char *nodeName, gsl_vector **data,
			    gsl_vector **time) {

  /* local vars */
  int dtype_dbl = DTYPE_DOUBLE;
  int null = 0;
  int connectionStatus;
  char buf[1024];       // Used to get DIM_OF of the array
  int signalDescriptor; // signal descriptor
  int timeDescriptor;   // descriptor for timeBase
  int sizeArray;        // length of signal
  int len;
  int connectionID;     // Connection ID for mdsplus connection

  /* In case there is already data here */
  gsl_vector_free(*data);
  gsl_vector_free(*time);
  
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
    return -1;

  }

  /* Getting signal length of */
  sizeArray = get_signal_length(nodeName);

  if ( sizeArray < 1 ) {

    fprintf(stderr,"Error retrieving length of signal\n");
    return -1;

  }

  *data = gsl_vector_alloc(sizeArray);
  *time = gsl_vector_alloc(sizeArray);

  /* create a descriptor for the timeBase */
  timeDescriptor = descr(&dtype_dbl, (*time)->data, &sizeArray, &null);

  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName);

  /* retrieve timeBase of signal */
  connectionStatus = MdsValue(buf, &timeDescriptor, &null, 0);

  /* Verify that time base was read okay */
  if ( !status_ok(connectionStatus) ) {

    /* error */
    fprintf(stderr,"Error retrieving timeBase\n");
    gsl_vector_free(*data);
    gsl_vector_free(*time);
    return -1;

  }

  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, (*data)->data, &sizeArray, &null);
 
  /* retrieve signal */
  connectionStatus = MdsValue(nodeName, &signalDescriptor, &null, &len);

  if ( !status_ok(connectionStatus) ) {
    
    gsl_vector_free(*data);
    gsl_vector_free(*time);
    fprintf(stderr,"Error retrieving signal\n");
    return -1;
    
  }

  return 0;

}
