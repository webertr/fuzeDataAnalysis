#include "cpp/spectroscopyMonitor.h"

/******************************************************************************
 *
 * These are the functions that will monitor the fuze control system,
 * and pull the ICCD spectroscopy data after each pulse, and push it to the
 * the mdpslus tree.
 *
 ******************************************************************************/

/* Static variables to hold the PV values */
static long shotNumber;
static std::string shotNumberFileName;

/* Static local functions */
static std::string getFileFromShotNumber(long shotNumber);
static bool checkFileExists(std::string fileName);
static void printChidInfo(chid chid, std::string message);
static void shotNumberCB(struct event_handler_args eha);

/******************************************************************************
 * Function: getFileFromShotNumber
 * Inputs: 
 * Returns: boolean
 * Description: Checks to see if a passed file exists. If it does, return true
 ******************************************************************************/

static std::string getFileFromShotNumber(long shotNumber) {

  /* 
   * 190521009 should be
   * "/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521009.spe"
   */

  std::string returnString;
  std::stringstream stringStream;

  //std::string baseFileName = "/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/";
  std::string baseFileName =   "/home/fuze/SpectroscopyData/";

  int baseShotNumber = shotNumber/1000;
  stringStream << baseShotNumber;
  std::string baseShotNumberString = stringStream.str();
  stringStream.str("");

  stringStream << shotNumber;
  std::string shotNumberString = stringStream.str();
  stringStream.str("");

  returnString.append(baseFileName);
  returnString.append(baseShotNumberString);
  returnString.append("/");
  returnString.append(shotNumberString);
  returnString.append(".spe");
  
  return returnString;

}

/******************************************************************************
 * Function: checkFileExists
 * Inputs: 
 * Returns: boolean
 * Description: Checks to see if a passed file exists. If it does, return true
 ******************************************************************************/

static bool checkFileExists(std::string fileName) {

  std::ifstream ifile(fileName.c_str(), std::ifstream::in);

  if (ifile.is_open()) {
    return true;
  } else {
    return false;
  }

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
 * Function: shotNumberCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the shotnumber pv
 ******************************************************************************/

static void shotNumberCB(struct event_handler_args eha) {

    chid	chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {

	printChidInfo(chid,"eventCallback");

    } else {
      long *pshotNumber = (long *)eha.dbr;
      shotNumber = *pshotNumber;
      printf("Shot Number change: %s = %ld\n",ca_name(eha.chid),shotNumber);
    }

    shotNumberFileName = getFileFromShotNumber(shotNumber);

    return;

}


/******************************************************************************
 * Function: runSpectroscopyMonitor
 * Inputs: 
 * Returns: int
 * Description: This is the main program that will be run to moniotor the
 * spectroscoy and iccd data after each pulse, and put it into mdsplus.
 ******************************************************************************/

int runSpectroscopyMonitor() {

  const int SIZE = 1;

  std::string pvNames[SIZE] = {"FuZE:DataServer:ShotNumber"};

  epicsCBFuncPointer cbFuncs[SIZE] = {shotNumberCB};

  monitorLongPVsWithCallback(pvNames, cbFuncs, SIZE);

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static void eventCallback(struct event_handler_args eha) {

    chid	chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
	printChidInfo(chid,"eventCallback");
    } else {
      //char	*pdata = (char *)eha.dbr;
      //printf("Event Callback: %s = %s\n",ca_name(eha.chid),pdata);
      long *pdata = (long *)eha.dbr;
      printf("Event Callback: %s = %ld\n",ca_name(eha.chid),*pdata);
    }
}

static bool testSpectroscopyMonitorBasic();
static bool testCheckFileExists();
static bool testGetFileFromShotNumber();

bool testSpectroscopyMonitor() {

  if (!testGetFileFromShotNumber()) {
    std::cout << "Test Spectroscopy Monitor Get File From ShotNumber FAILED\n";
    return false;
  }

  if (!testCheckFileExists()) {
    std::cout << "Test Spectroscopy Monitor Check File Exists FAILED\n";
    return false;
  }

  if (!testSpectroscopyMonitorBasic()) {
    std::cout << "Test Spectroscopy Monitor Basic FAILED\n";
    return false;
  }

  std::cout << "All spectroscopyMonitor.cpp tests passed\n";

  return true;

}


static bool testSpectroscopyMonitorBasic() {

  const int SIZE = 3;

  std::string pvNames[SIZE] = {"FuZE:ControlPLC:PSQLError", 
			       "FuZE:ControlPLC:InnerParkerDial", 
			       "FuZE:ControlPLC:MDSplusError"};

  epicsCBFuncPointer cbFuncs[SIZE] = {eventCallback, eventCallback, eventCallback};

  monitorLongPVsWithCallback(pvNames, cbFuncs, SIZE);

  return true;

}

static bool testCheckFileExists() {

  /* Should exists */
  //std::string fileNameExists = 
  //"/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521009.spe";
  std::string fileNameExists = "/home/fuze/SpectroscopyData/190521/190521009.spe";

  /* should not exists */
  //std::string fileNameNoExists = 
  //"/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521010.spe";
  std::string fileNameNoExists = "/home/fuze/SpectroscopyData/190521/190521010.spe";

  if ( checkFileExists(fileNameExists) &&
       !checkFileExists(fileNameNoExists) ) {
    return true;
  } else {
    return false;
  }

}


static bool testGetFileFromShotNumber() {

  long shotNumber = 190521009;
  //std::string shotNumberResults = 
  //"/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521009.spe";
  std::string shotNumberResults =  "/home/fuze/SpectroscopyData/190521/190521009.spe";

  shotNumberFileName = getFileFromShotNumber(shotNumber);

  if ( (shotNumberFileName == shotNumberResults) ) {
    return true;
  } else{
    return false;
  }

}
