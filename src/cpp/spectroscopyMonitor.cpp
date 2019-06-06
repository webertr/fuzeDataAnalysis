#include "cpp/spectroscopyMonitor.h"

/******************************************************************************
 *
 * These are the functions that will monitor the fuze control system,
 * and pull the ICCD spectroscopy data after each pulse, and push it to the
 * the mdpslus tree.
 *
 ******************************************************************************/

/* Static variables to hold the PV values */
static long shotNumber;                // FuZE:DataServer:ShotNumber
static std::string shotNumberFileName; //
static double iccdCenterWavelength;    // FuZE:ControlPLC:ICCDCenterWavelength
static double spectrometerZ;           // FuZE:ControlPLC:SpectrometerZPosition
static short telescopeAngle;           // FuZE:ControlPLC:TelescopeAngle => 1=0 Deg.; 2=45 Deg.;
                                       // 3 = 0 and 45 Deg.
static short iccdGrating;              // FuZE:ControlPLC:ICCDGrating => 1 = 3600 g/mm;
                                       // 2 = 2400 g/mm; 3 = 300 g/mm
static short iccdActiveSlit;           // FuZE:ControlPLC:ICCDActiveSlit => 1 = Front, Manual;
                                       // 2 = Side, Manual


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

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    long *pshotNumber = (long *)eha.dbr;
    shotNumber = *pshotNumber;

    shotNumberFileName = getFileFromShotNumber(shotNumber);

    std::cout << "Shot number set to: " << shotNumber << "\n";

    return;

}


/******************************************************************************
 * Function: iccdCenterWavelengthCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the iccdCenterWavelength pv
 ******************************************************************************/

static void iccdCenterWavelengthCB(struct event_handler_args eha) {

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    double *piccdCenterWavelength = (double *)eha.dbr;
    iccdCenterWavelength = *piccdCenterWavelength;

    std::cout << "ICCD center wavelength set to: " << iccdCenterWavelength << "\n";

    return;

}

/******************************************************************************
 * Function: spectrometerZCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the spectrometerZ pv
 ******************************************************************************/

static void spectrometerZCB(struct event_handler_args eha) {

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    double *pspectrometerZ = (double *)eha.dbr;
    spectrometerZ = *pspectrometerZ;

    std::cout << "Spectrometer z set to: " << spectrometerZ << "\n";

    return;

}


/******************************************************************************
 * Function: telescopeAngleCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the telescopeAngle pv
 ******************************************************************************/

static void telescopeAngleCB(struct event_handler_args eha) {

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    short *ptelescopeAngle = (short *)eha.dbr;
    telescopeAngle = *ptelescopeAngle;

    std::cout << "Telescope Angle set to: " << telescopeAngle << "\n";

    return;

}


/******************************************************************************
 * Function: iccdGratingCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the iccdGrating pv
 ******************************************************************************/

static void iccdGratingCB(struct event_handler_args eha) {

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    short *piccdGrating = (short *)eha.dbr;
    iccdGrating = *piccdGrating;

    std::cout << "ICCD Grating set to: " << iccdGrating << "\n";

    return;

}

/******************************************************************************
 * Function: iccdActiveSlitCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a change to the iccdActiveSlit pv
 ******************************************************************************/

static void iccdActiveSlitCB(struct event_handler_args eha) {

    chid chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
      printChidInfo(chid,"eventCallback");
      return;
    }
    
    short *piccdActiveSlit = (short *)eha.dbr;
    iccdActiveSlit = *piccdActiveSlit;

    std::cout << "ICCD active slit set to: " << iccdActiveSlit << "\n";

    return;

}

/******************************************************************************
 * Function: lightFieldCB
 * Inputs: 
 * Returns: int
 * Description: The callback for a download the light field data
 ******************************************************************************/

static void lightFieldCB(struct event_handler_args eha) {

  gsl_vector *fiberCenters;
  gsl_vector *fiberEdges;

  long lastShotNumber;

  chid  chid = eha.chid;

  if(eha.status!=ECA_NORMAL) {

    printChidInfo(chid,"eventCallback");
    std::cout << "Error in light field call back\n";
    return;

  }

  long *plfMDSplus = (long *)eha.dbr;

  /* Exiting because light field push not equal to 1 */
  if ( (*plfMDSplus) != 1) {
    return;
  }

  printf("Light Field Upload to MDSPlus: %s = %ld\n",ca_name(eha.chid),*plfMDSplus);

  /* Checking to see if there is a file available */
  if ( !checkFileExists(shotNumberFileName)) {
    std::cout << "No light field file available for shot number "
	      << shotNumber << "\n";
    return;
  }
        
  /* Getting light field .spe file data from current shot */
  LightField lfObject = LightField(shotNumberFileName);
    
  /* Checking to see if the fiber centers were found. if not, abort */
  if (lfObject.chordsOK) {
    std::cout << "Fiber centers found. Uploading data for shot number "
	      << shotNumber << "\n";
  } else {

    /* Use the fiber centers and fiber edges from the previous shot */
    std::cout << "Fiber centers not found. Looking for previous fiber centers\n";

    /* Find the last shot number with Spectroscopy data */
    /* Get last 100 shot numbers */
    const int nShotNumbers = 100;
    gsl_vector_long *recentShotNumbers = getRecentShotNumbers(nShotNumbers);

    /* Iterating through 100 shot numbers to try and find fiber center and edge data */
    int ii;
    for (ii = 0; ii < (int) recentShotNumbers->size; ii++) {
      
      lastShotNumber = gsl_vector_long_get(recentShotNumbers, ii);
      fiberCenters = readMDSplusVector(lastShotNumber, "\\ICCD:FIBERCENTERS", "fuze");
      fiberEdges = readMDSplusVector(lastShotNumber, "\\ICCD:FIBEREDGES", "fuze");

      if ( (fiberCenters != 0) && (fiberEdges != 0) ) {
	std::cout << "Found data for shot number " << lastShotNumber << "\n";
	break;
      }

    }

    /* vector point = 0 means there was not data in the mdsplus tree for any of the 100 shots */
    if (fiberCenters == 0) {
      std::cout << "Previous fiber centers not found\n"
		<< "Not uploading mdsplus data\n";
      return;
    }

    /* vector point = 0 means there was not data in the mdsplus tree for any of the 100 shots */
    if (fiberEdges == 0) {
      std::cout << "Previous fiber edges not found\n"
		<< "Not uploading mdsplus data\n";
      return;
    }

    /* Setting fiber Centers from mdsplus data to current lf object */
    if (lfObject.setFiberCenters(fiberCenters)) {
      std::cout << "Not uploading mdsplus data. Couldn't set fiber centers.\n";
      return;
    }

    /* Setting fiber edges from mdsplus data to current lf object */      
    if( lfObject.setFiberEdges(fiberEdges)) {
      std::cout << "Not uploading mdsplus data. Couldn't set fiber edges\n";
      return;
    }

    lfObject.populateChords();

  }

  /* Uploading LF data to mdsplus */
  writeMDSplusMatrix(lfObject.imageUShort, shotNumber, "\\ICCD:RAW", "fuze");
  writeMDSplusVector(lfObject.waveLength, shotNumber, "\\ICCD:LAMBDA", "fuze");
  writeMDSplusVector(lfObject.rows, shotNumber, "\\ICCD:ROWS", "fuze");
  writeMDSplusVector(lfObject.fiberCenters, shotNumber, "\\ICCD:FIBERCENTERS", "fuze");
  writeMDSplusVector(lfObject.fiberEdges, shotNumber, "\\ICCD:FIBEREDGES", "fuze");

  /* Uploading chords to mdsplus */
  writeMDSplusVector(lfObject.chord1, shotNumber, "\\ICCD_01:RAW", "fuze");
  writeMDSplusVector(lfObject.chord2, shotNumber, "\\ICCD_02:RAW", "fuze");
  writeMDSplusVector(lfObject.chord3, shotNumber, "\\ICCD_03:RAW", "fuze");
  writeMDSplusVector(lfObject.chord4, shotNumber, "\\ICCD_04:RAW", "fuze");
  writeMDSplusVector(lfObject.chord5, shotNumber, "\\ICCD_05:RAW", "fuze");
  writeMDSplusVector(lfObject.chord6, shotNumber, "\\ICCD_06:RAW", "fuze");
  writeMDSplusVector(lfObject.chord7, shotNumber, "\\ICCD_07:RAW", "fuze");
  writeMDSplusVector(lfObject.chord8, shotNumber, "\\ICCD_08:RAW", "fuze");
  writeMDSplusVector(lfObject.chord9, shotNumber, "\\ICCD_09:RAW", "fuze");
  writeMDSplusVector(lfObject.chord10, shotNumber, "\\ICCD_10:RAW", "fuze");
  writeMDSplusVector(lfObject.chord11, shotNumber, "\\ICCD_11:RAW", "fuze");
  writeMDSplusVector(lfObject.chord12, shotNumber, "\\ICCD_12:RAW", "fuze");
  writeMDSplusVector(lfObject.chord13, shotNumber, "\\ICCD_13:RAW", "fuze");
  writeMDSplusVector(lfObject.chord14, shotNumber, "\\ICCD_14:RAW", "fuze");
  writeMDSplusVector(lfObject.chord15, shotNumber, "\\ICCD_15:RAW", "fuze");
  writeMDSplusVector(lfObject.chord16, shotNumber, "\\ICCD_16:RAW", "fuze");
  writeMDSplusVector(lfObject.chord17, shotNumber, "\\ICCD_17:RAW", "fuze");
  writeMDSplusVector(lfObject.chord18, shotNumber, "\\ICCD_18:RAW", "fuze");
  writeMDSplusVector(lfObject.chord19, shotNumber, "\\ICCD_19:RAW", "fuze");
  writeMDSplusVector(lfObject.chord20, shotNumber, "\\ICCD_20:RAW", "fuze");
  
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

  const int SIZE = 7;

  std::string pvNames[SIZE] = {"FuZE:DataServer:ShotNumber",
			       "FuZE:ControlPLC:LightFieldMDSplusUp",
			       "FuZE:ControlPLC:ICCDCenterWavelength",
			       "FuZE:ControlPLC:SpectrometerZPosition",
			       "FuZE:ControlPLC:TelescopeAngle",
			       "FuZE:ControlPLC:ICCDGrating",
			       "FuZE:ControlPLC:ICCDActiveSlit"};

  epicsCBFuncPointer cbFuncs[SIZE] = {shotNumberCB, lightFieldCB, iccdCenterWavelengthCB,
				      spectrometerZCB, telescopeAngleCB, iccdGratingCB,
				      iccdActiveSlitCB};

  chtype channelType[SIZE] = {DBR_LONG, DBR_LONG, DBR_DOUBLE, DBR_DOUBLE,
			      DBR_ENUM, DBR_ENUM, DBR_ENUM};

  monitorPVsWithCallback(pvNames, cbFuncs, channelType, SIZE);

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

  chtype channelType[SIZE] = {DBR_LONG, DBR_LONG, DBR_LONG};

  if (0) {
    monitorPVsWithCallback(pvNames, cbFuncs, channelType, SIZE);
  }

  return true;

}

static bool testCheckFileExists() {

  /*
   * Error with this:
   * Shot number set to: 190604001
   * ICCD center wavelength set to: 420
   * Spectrometer z set to: 0.15
   * Telescope Angle set to: 3 
   * ICCD Grating set to: 3
   * ICCD active slit set to: 1
   * Light Field Upload to MDSPlus: FuZE:ControlPLC:LightFieldMDSplusUp = 1
   * Error reading SPE file for frame width
   * fuze@ops:~/Spectroscopy/Data/190604$ ls
   * 190604002-raw.spe  190604002.spe  190604003-raw.spe  190604003.spe  
   * 190604004-raw.spe  190604004.spe
   */

  /* Should exists */
  //std::string fileNameExists = 
  //"/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521009.spe";
  std::string fileNameExists = "/home/fuze/SpectroscopyData/190521/190521009.spe";

  /* should not exists */
  //std::string fileNameNoExists = 
  //"/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521010.spe";
  std::string fileNameNoExists = "/home/fuze/SpectroscopyData/190521/190521010.spe";

  std::cout << checkFileExists("/home/fuze/SpectroscopyData/190604/190604002.spe") << "\n";

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
