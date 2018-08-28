#include "bankCheck.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


static int status_ok( int status );
static int get_signal_length(const char *signal);
static int getSignalLengthMDSplus(const char *signal, int shotNumber);
static int totalCurrentCapCheck(int shotNumber, char *nodeName, double limit);

/******************************************************************************
 * Function: bankCheck
 * Inputs: void
 * Returns: int
 * Description: Checking the bank diagnostics
 ******************************************************************************/

int bankCheck() {

  /*
   * What we use for dI/dt:
   * dI/dt from CAP 1:
   * \fuze_1612_a:input_01 * \I_TBM_01_CAP:CAL_FACT * \I_TBM_01_CAP:ATTEN_FACT * 1e-9
   *
   * dI/dt to machine 1:
   * \fuze_1612_a:input_02 * \I_TBM_01_OUT:CAL_FACT * \I_TBM_01_OUT:ATTEN_FACT * 1e-9
   *
   * 1-6 is a, then 7-12 is b
   * "I_TBM_01_OUT" = I to the machine ( < 50 kA, soft limit)
   * "I_TBM_01_CAP" = I off of the capacitor. 
   * "I_TBM_01_DIODE" = I through the diode
   */

  const int CAP_NODE_NUMBER = 12;
  char *capNodeName[] = 
    {"\\I_TBM_01_OUT", "\\I_TBM_02_OUT", "\\I_TBM_03_OUT", "\\I_TBM_04_OUT", "\\I_TBM_05_OUT", 
     "\\I_TBM_06_OUT", "\\I_TBM_07_OUT", "\\I_TBM_08_OUT", "\\I_TBM_09_OUT", "\\I_TBM_10_OUT", 
     "\\I_TBM_11_OUT", "\\I_TBM_12_OUT"};

  char *nodeNameDIDtCap[] = 
    {"\\FUZE_1612_A:INPUT_01*\\I_TBM_01_CAP:CAL_FACT*\\I_TBM_01_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_03*\\I_TBM_02_CAP:CAL_FACT*\\I_TBM_02_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_05*\\I_TBM_03_CAP:CAL_FACT*\\I_TBM_03_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_07*\\I_TBM_04_CAP:CAL_FACT*\\I_TBM_04_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_09*\\I_TBM_05_CAP:CAL_FACT*\\I_TBM_05_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_11*\\I_TBM_06_CAP:CAL_FACT*\\I_TBM_06_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_01*\\I_TBM_07_CAP:CAL_FACT*\\I_TBM_07_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_03*\\I_TBM_08_CAP:CAL_FACT*\\I_TBM_08_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_05*\\I_TBM_09_CAP:CAL_FACT*\\I_TBM_09_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_07*\\I_TBM_10_CAP:CAL_FACT*\\I_TBM_10_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_09*\\I_TBM_11_CAP:CAL_FACT*\\I_TBM_11_CAP:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_11*\\I_TBM_12_CAP:CAL_FACT*\\I_TBM_12_CAP:ATTEN_FACT*1e-9"};

  char *nodeNameDIDtOut[] = 
    {"\\FUZE_1612_A:INPUT_02*\\I_TBM_01_OUT:CAL_FACT*\\I_TBM_01_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_04*\\I_TBM_02_OUT:CAL_FACT*\\I_TBM_02_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_06*\\I_TBM_03_OUT:CAL_FACT*\\I_TBM_03_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_08*\\I_TBM_04_OUT:CAL_FACT*\\I_TBM_04_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_10*\\I_TBM_05_OUT:CAL_FACT*\\I_TBM_05_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_A:INPUT_12*\\I_TBM_06_OUT:CAL_FACT*\\I_TBM_06_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_02*\\I_TBM_07_OUT:CAL_FACT*\\I_TBM_07_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_04*\\I_TBM_08_OUT:CAL_FACT*\\I_TBM_08_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_06*\\I_TBM_09_OUT:CAL_FACT*\\I_TBM_09_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_08*\\I_TBM_10_OUT:CAL_FACT*\\I_TBM_10_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_10*\\I_TBM_11_OUT:CAL_FACT*\\I_TBM_11_OUT:ATTEN_FACT*1e-9",
     "\\FUZE_1612_B:INPUT_12*\\I_TBM_12_OUT:CAL_FACT*\\I_TBM_12_OUT:ATTEN_FACT*1e-9"};

  const double I_MAX = 50E3;
  const double DI_DT_MAX = 50E3;

  /*
   * Checking that the current from each capacitor is under 50 kA
   */
  int ii;
  for (ii = 0; ii < CAP_NODE_NUMBER; ii++) {
    printf("Current OK for cap %d: %d\n", ii+1,
	   totalCurrentCapCheck(180823047 , capNodeName[ii], I_MAX));
  }

  /*
   * Checking that the dI/dt through each thyristor is < 50 kA / usec
   */
  for (ii = 0; ii < CAP_NODE_NUMBER; ii++) {
    printf("dI/dt OK for cap %d: %d\n", ii+1,
	   totalCurrentCapCheck(180823047 , nodeNameDIDtCap[ii], DI_DT_MAX));
    printf("dI/dt OK for cap %d: %d\n", ii+1,
	   totalCurrentCapCheck(180823047 , nodeNameDIDtOut[ii], DI_DT_MAX));
  }


  return 0;

}


/******************************************************************************
 * Function: totalCurrentCapCheck
 * Inputs: int, char *, double
 * Returns: int
 * Description: Checking the total current from a capacitor to make sure
 * it is below the maximum allowed value
 ******************************************************************************/

static int totalCurrentCapCheck(int shotNumber, char *nodeName, double limit) {

  /* local vars */
  int dtype_dbl = DTYPE_DOUBLE;
  int null = 0;
  int connectionStatus;
  char buf[1024];                 // Used to get DIM_OF of the array
  int signalDescriptor;           // signal descriptor
  int timeDescriptor;             // descriptor for timeBase
  int len;
  int connectionID;               // Connection ID for mdsplus connection
  int bankOK = 1;
  const double I_MAX = limit;

  int sigSize = getSignalLengthMDSplus(nodeName, shotNumber);

  gsl_vector *time = gsl_vector_alloc(sigSize);
  gsl_vector *data = gsl_vector_alloc(sigSize);

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
  timeDescriptor = descr(&dtype_dbl, time->data, &sigSize, &null);

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
  signalDescriptor = descr(&dtype_dbl, data->data, &sigSize, &null);
 
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

  plotVectorData(time, data, "");

  int ii;
  for (ii = 0; ii < sigSize; ii++) {
    if (gsl_vector_get(data, ii) > I_MAX) {
      bankOK = 0;
      return bankOK;
    }
  }

  gsl_vector_free(data);
  gsl_vector_free(time);

  return bankOK;


}


/******************************************************************************
 * Function: getSignalLengthMDSplus
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 * if not successfull.
 ******************************************************************************/

static int getSignalLengthMDSplus(const char *signal, int shotNumber) {

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
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(status));
    MdsDisconnect();
    return -1;

  }

  MdsDisconnect();
  
  /* return signal length */
  return size;

}


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
