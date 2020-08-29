#include "cpp/softXRay.h"

/******************************************************************************
 *
 * This is the source file to analyze soft X-ray data
 *
 ******************************************************************************/

/******************************************************************************
 * Example Usage:
 *
 *
 ******************************************************************************/

/******************************************************************************
 * Static Function declarations
 ******************************************************************************/
static int clearGlobalVariables();
static int setGlobalVariables(int shotNumber);
static int plotXRay(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh);
static int plotNeutron(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh);

/******************************************************************************
 * Global variables
 ******************************************************************************/
static gsl_vector *timeNeutronGlobal;
static gsl_vector *neutron10Global;
static gsl_vector *neutron12Global;
static gsl_vector *timeXRayGlobal;
static gsl_vector *xRay2Global;
static gsl_vector *xRay4Global;

#define TLOW 15
#define THIGH 25


/******************************************************************************
 * Function: softXRayRun
 * Inputs:
 * Returns: 
 * Description: Runs soft X-ray analysis
 ******************************************************************************/

int softXRayRun() {

  int shotNumber,
    currShotNumber = getCurrentPulseNumber();

  printf("\nEnter Pulse Number> ");
  scanf("%d", &shotNumber);
  
  if (shotNumber <= 0) {
    shotNumber = currShotNumber+shotNumber;
  }

  getchar();

  setGlobalVariables(shotNumber);

  int pid1 = fork();

  if ( pid1 == 0 ) {
    plotNeutron(shotNumber, "data/neutron1.txt", "data/neutron1.sh", TLOW, THIGH);
    exit(0);    
  }
  else if ( pid1 > 0 ) {
    plotXRay(shotNumber, "data/xray1.txt", "data/xray1.sh", TLOW, THIGH);
    exit(0);
  }
  
  clearGlobalVariables();
  
  return 0;
  
}


/******************************************************************************
 * Function: setGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will set the global variables
 ******************************************************************************/

static int setGlobalVariables(int shotNumber) {
  
  timeNeutronGlobal = readMDSplusVectorDim(shotNumber, "\\neutron_10_s", "fuze");
  neutron10Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  neutron12Global = readMDSplusVector(shotNumber, "\\neutron_12_s", "fuze");
  timeXRayGlobal = readMDSplusVectorDim(shotNumber, "\\neutron_10_s", "fuze");
  xRay2Global = readMDSplusVector(shotNumber, "\\xray_2", "fuze");
  xRay4Global = readMDSplusVector(shotNumber, "\\xray_4", "fuze");

  return 0;

}


/******************************************************************************
 * Function: clearGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will clear the global variables.
 ******************************************************************************/

static int clearGlobalVariables() {

  gsl_vector_free(timeNeutronGlobal);
  gsl_vector_free(neutron10Global);
  gsl_vector_free(neutron12Global);
  gsl_vector_free(timeXRayGlobal);
  gsl_vector_free(xRay2Global);
  gsl_vector_free(xRay4Global);

  return 0;
  
}


/******************************************************************************
 * Function: plotXRay
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRay(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay2Label;
  std::string xRay4Label;
  std::string rangeLabel;

  gsl_vector_scale(timeXRayGlobal, 1E6);

  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'Ti/X-Ray 2 for " << shotNumber << "'";
  xRay2Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'Al/X-Ray 4 for " << shotNumber << "'";
  xRay4Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Soft X-ray Signals'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot2VectorData(timeXRayGlobal, xRay2Global, xRay2Label, xRay4Global, xRay4Label,
  		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotNeutron
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotNeutron(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh) {

  std::ostringstream oss;
  std::string neutron10Label;
  std::string neutron12Label;
  std::string rangeLabel;

  gsl_vector_scale(timeNeutronGlobal, 1E6);

  //gsl_vector_scale(neutron10Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'Neutron 10 for " << shotNumber << "'";
  neutron10Label = oss.str();
  oss.str("");

  //gsl_vector_scale(neutron12Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'Neutron 12 for " << shotNumber << "'";
  neutron12Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Neutron Signals'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot2VectorData(timeNeutronGlobal, neutron10Global, neutron10Label, neutron12Global,
		  neutron12Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testThisFunction();

int testSoftXRay() {

  if (!testThisFunction()) {
    std::cout << "Testing this function FAILED\n";
    return false;
  }

  std::cout << "All softXRay.cpp tests passed\n";

  return true;

}


static bool testThisFunction() {

  return false;

}
