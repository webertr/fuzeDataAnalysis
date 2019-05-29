#include "cpp/spectroscopyMonitor.h"

/******************************************************************************
 *
 * These are the functions that will monitor the fuze control system,
 * and pull the ICCD spectroscopy data after each pulse, and push it to the
 * the mdpslus tree.
 *
 ******************************************************************************/


/******************************************************************************
 * Function: runSpectroscopyMonitor
 * Inputs: 
 * Returns: int
 * Description: This is the main program that will be run to moniotor the
 * spectroscoy and iccd data after each pulse, and put it into mdsplus.
 ******************************************************************************/

int runSpectroscopyMonitor() {

  const int SIZE = 3;

  std::string pvNames[SIZE] = {"FuZE:ControlPLC:PSQLError", 
			       "FuZE:ControlPLC:InnerParkerDial", 
			       "FuZE:ControlPLC:MDSplusError"};

  epicsCBFuncPointer cbFuncs[SIZE] = {eventCallback, eventCallback, eventCallback};

  monitorLongPVsWithCallback(pvNames, cbFuncs, SIZE);

  return 0;

}


/******************************************************************************
 * Function: runSpectroscopyMonitor
 * Inputs: 
 * Returns: int
 * Description: This is the main program that will be run to moniotor the
 * spectroscoy and iccd data after each pulse, and put it into mdsplus.
 ******************************************************************************/

int runSpectroscopyMonitor() {

  const int SIZE = 3;

  std::string pvNames[SIZE] = {"FuZE:ControlPLC:PSQLError", 
			       "FuZE:ControlPLC:InnerParkerDial", 
			       "FuZE:ControlPLC:MDSplusError"};

  epicsCBFuncPointer cbFuncs[SIZE] = {eventCallback, eventCallback, eventCallback};

  monitorLongPVsWithCallback(pvNames, cbFuncs, SIZE);

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testSpectroscopyMonitorBasic();

int testSpectroscopyMonitor() {

  if (!testSpectroscopyMonitor()) {
    std::cout << "Test Spectroscopy Monitor FAILED\n";
    return -1;
  }

  std::cout << "All spectroscopyMonitor.cpp tests passed\n";

  return 0;

}


static int testSpectroscopyMonitorBasic() {


  const int SIZE = 3;

  std::string pvNames[SIZE] = {"FuZE:ControlPLC:PSQLError", 
			       "FuZE:ControlPLC:InnerParkerDial", 
			       "FuZE:ControlPLC:MDSplusError"};

  epicsCBFuncPointer cbFuncs[SIZE] = {eventCallback, eventCallback, eventCallback};

  monitorLongPVsWithCallback(pvNames, cbFuncs, SIZE);

  return 1;

}
