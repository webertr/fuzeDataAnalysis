#include "cpp/tekScopeMonitor.h"

/******************************************************************************
 *
 * These are the functions that will monitor the fuze control system,
 * and pull data from a tektronix scope via serial communication
 *
 ******************************************************************************/


/* Static local functions */
static void printChidInfo(chid chid, std::string message);
static void stateEnumCB(struct event_handler_args eha);
static bool tekScopeUpdate();

/******************************************************************************
 * Function: 
 * Inputs: 
 * Returns: 
 * Description: main function to execut program
 ******************************************************************************/

int main() {

  /* Run tek scope monitor program */
  runTekScopeMonitor();

  /* Test tek scope monitor program */
  //testTekScopeMonitor();
  
  return 0;

}


/******************************************************************************
 * Function: printChidInfo
 * Inputs: 
 * Returns: int
 * Description: Helper function to print the chid information from a chid
 ******************************************************************************/

static void printChidInfo(chid chid, std::string message) {

  std::cout << "\n" << message << "\n";

  printf("pv: %s  type(%d) nelements(%ld) host(%s)",
	 ca_name(chid),ca_field_type(chid),ca_element_count(chid),
	 ca_host_name(chid));
  printf(" read(%d) write(%d) state(%d)\n",
	 ca_read_access(chid),ca_write_access(chid),ca_state(chid));

}


/******************************************************************************
 * Function: stateEnumCB
 * Inputs: 
 * Returns: int
 * Description: The callback to download the Tek Scope Data
 ******************************************************************************/

static void stateEnumCB(struct event_handler_args eha) {
  
  chid  chid = eha.chid;

  if(eha.status!=ECA_NORMAL) {

    printChidInfo(chid,"eventCallback");
    std::cout << "Error in light field call back\n";
    return;

  }

  long *stateEnum = (long *)eha.dbr;

  /* Exiting because light field push not equal to 1 */
  if ( (*stateEnum) != 8) {
    return;
  }

  printf("Downloading Tek Scope data and uploading to mdpslus: %s = %ld\n",
	 ca_name(eha.chid),*stateEnum);

  if (tekScopeUpdate()) {
    std::cout << "Successfully downloaded and updated\n";
    return;
  }

  std::cout << "Failed to download and update " << "\n";

  return;
  
}


/******************************************************************************
 * Function: tekScopeUpdate
 * Inputs: 
 * Returns: int
 * Description: The callback for a download and update tek scope data
 ******************************************************************************/

static bool tekScopeUpdate() {

  gsl_vector *timeBase = SerialTekScopeComm::getScopeTimeBase(1);
  gsl_vector *ch1Vec = SerialTekScopeComm::getScopeChData(1);
  gsl_vector *ch2Vec = SerialTekScopeComm::getScopeChData(2);
  gsl_vector *ch3Vec = SerialTekScopeComm::getScopeChData(3);
  gsl_vector *ch4Vec = SerialTekScopeComm::getScopeChData(4);

  saveVectorData(timeBase, "data/timeBase.txt");
  saveVectorData(ch1Vec, "data/ch1.txt");
  saveVectorData(ch2Vec, "data/ch2.txt");
  saveVectorData(ch3Vec, "data/ch3.txt");
  saveVectorData(ch4Vec, "data/ch4.txt");
  
  // Sleeping for 10 seconds
  sleep(20);

  int shotNumber = 0;
  std::string nodeName;
  std::string treeName = "fuze";
  
  nodeName = "\\TOP.SIGNALS.XRAY:XRAY_1:RAW";
  MDSplusAccess::writeMDSplusSignal(timeBase, ch1Vec, shotNumber, nodeName, treeName);
  nodeName = "\\TOP.SIGNALS.XRAY:XRAY_2:RAW";
  MDSplusAccess::writeMDSplusSignal(timeBase, ch2Vec, shotNumber, nodeName, treeName);
  nodeName = "\\TOP.SIGNALS.XRAY:XRAY_3:RAW";
  MDSplusAccess::writeMDSplusSignal(timeBase, ch3Vec, shotNumber, nodeName, treeName);
  nodeName = "\\TOP.SIGNALS.XRAY:XRAY_4:RAW";
  MDSplusAccess::writeMDSplusSignal(timeBase, ch4Vec, shotNumber, nodeName, treeName);
  
  return true;

}


/******************************************************************************
 * Function: runTekScopeMonitor
 * Inputs: 
 * Returns: int
 * Description: This is the main program that will be run to moniotor the
 * tek scope data after each pulse, and put it into mdsplus.
 ******************************************************************************/

int runTekScopeMonitor() {

  const int SIZE = 1;

  std::string pvNames[SIZE] = {"FuZE:ControlPLC:Control:State:Enumeration" };

  epicsCBFuncPointer cbFuncs[SIZE] = {stateEnumCB };

  chtype channelType[SIZE] = {DBR_LONG};

  monitorPVsWithCallback(pvNames, cbFuncs, channelType, SIZE);

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

bool testSpectroscopyMonitor() {


  std::cout << "All tekScopeMonitor.cpp tests passed\n";

  return true;

}

