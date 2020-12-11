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
static int plotXRay(int shotNumber, std::string tempDataFile,
		    std::string tempScriptFile, int tLow, int tHigh);
static int plotXRay1(int shotNumber, std::string tempDataFile,
		     std::string tempScriptFile, int tLow, int tHigh);
static int plotXRay2(int shotNumber, std::string tempDataFile,
		     std::string tempScriptFile, int tLow, int tHigh);
static int plotNeutron(int shotNumber, std::string tempDataFile,
		       std::string tempScriptFile, int tLow, int tHigh);
static int plotNeutron1(int shotNumber, std::string tempDataFile,
			std::string tempScriptFile, int tLow, int tHigh);
static int plotBGLight(int shotNumber, std::string tempDataFile,
		       std::string tempScriptFile, int tLow, int tHigh);
static int plotXRay3(int shotNumber, std::string tempDataFile,
		     std::string tempScriptFile, int tLow, int tHigh);
static int plotXRayRatio1(int shotNumber, std::string tempDataFile,
			  std::string tempScriptFile, int tLow, int tHigh);
static int plotXRayRatio2(int shotNumber, std::string tempDataFile,
			  std::string tempScriptFile, int tLow, int tHigh);


/******************************************************************************
 * Global variables
 ******************************************************************************/
static gsl_vector *timeNeutronGlobal;
static gsl_vector *neutron10Global;
static gsl_vector *neutron12Global;
static gsl_vector *timeXRayGlobal;
static gsl_vector *timeXRayScopeGlobal;
static gsl_vector *timeXRayDetaqGlobal;
static gsl_vector *xRay1Global;
static gsl_vector *xRay2Global;
static gsl_vector *xRay3Global;
static gsl_vector *xRay4Global;
static gsl_vector *xRay5Global;

#define TLOW 20
#define THIGH 30


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
  int pid2 = fork();

  if ( (pid1 == 0) && (pid2 == 0) ) {
    //plotNeutron1(shotNumber, "data/neutron1.txt", "data/neutron1.sh", TLOW, THIGH);
    exit(0);    
  }
  else if ( (pid1 == 0) && (pid2 > 0) ) {
    //plotXRay(shotNumber, "data/xray1.txt", "data/xray1.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) ) {
    plotXRayRatio1(shotNumber, "data/xray2.txt", "data/xray2.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) ) {
    plotXRayRatio2(shotNumber, "data/xray3.txt", "data/xray3.sh", TLOW, THIGH);
    exit(0);
  }

  if (0) {
    plotXRay1(shotNumber, "data/xray2.txt", "data/xray2.sh", TLOW, THIGH);
    plotXRay(shotNumber, "data/xray1.txt", "data/xray1.sh", TLOW, THIGH);
    plotXRay2(shotNumber, "data/xray3.txt", "data/xray3.sh", TLOW, THIGH);
    plotXRay3(shotNumber, "data/xray3.txt", "data/xray3.sh", TLOW, THIGH);
    plotNeutron(shotNumber, "data/neutron1.txt", "data/neutron1.sh", TLOW, THIGH);
    plotNeutron1(shotNumber, "data/neutron1.txt", "data/neutron1.sh", TLOW, THIGH);
    plotBGLight(shotNumber, "data/xray3.txt", "data/xray3.sh", TLOW, THIGH);
    plotXRayRatio1(shotNumber, "data/xray2.txt", "data/xray2.sh", TLOW, THIGH);
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
  gsl_vector_scale(timeNeutronGlobal, 1E6);
  neutron10Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  neutron12Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  timeXRayDetaqGlobal = readMDSplusVectorDim(shotNumber, "\\xray_2", "fuze");
  gsl_vector_scale(timeXRayDetaqGlobal, 1E6);
  timeXRayScopeGlobal = readMDSplusVectorDim(shotNumber, "\\xray_1", "fuze");
  gsl_vector_scale(timeXRayScopeGlobal, 1E6);
  timeXRayGlobal = readMDSplusVectorDim(shotNumber, "\\xray_1", "fuze");
  gsl_vector_scale(timeXRayGlobal, 1E6);
  xRay1Global = readMDSplusVector(shotNumber, "\\xray_1", "fuze");
  xRay2Global = readMDSplusVector(shotNumber, "\\xray_2", "fuze");
  xRay3Global = readMDSplusVector(shotNumber, "\\xray_3", "fuze");
  xRay4Global = readMDSplusVector(shotNumber, "\\xray_4", "fuze");
  //xRay5Global = readMDSplusVector(shotNumber, "\\xray_4", "fuze");

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
  gsl_vector_free(timeXRayScopeGlobal);
  gsl_vector_free(timeXRayDetaqGlobal);
  gsl_vector_free(xRay1Global);
  gsl_vector_free(xRay2Global);
  gsl_vector_free(xRay3Global);
  gsl_vector_free(xRay4Global);
  //gsl_vector_free(xRay5Global);

  return 0;
  
}


/******************************************************************************
 * Function: plotXRay
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRay(int shotNumber, std::string tempDataFile,
		    std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay1Label;
  std::string xRay2Label;
  std::string xRay3Label;
  std::string xRay4Label;
  std::string rangeLabel;

  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title '2 um Ti #1 for " << shotNumber << "'";
  xRay1Label = oss.str();
  oss.str("");
 
  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title '4.4um Ti #2 for " << shotNumber << "'";
  xRay2Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title '1.27um Ni #3 for " << shotNumber << "'";
  xRay3Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'blue' title '4.1um Al #4 for " << shotNumber << "'";
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

  plot4VectorData(timeXRayGlobal, xRay1Global, xRay1Label, xRay2Global, xRay2Label,
		  xRay3Global, xRay3Label, xRay4Global, xRay4Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotXRay1
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRay1(int shotNumber, std::string tempDataFile,
		    std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay2Label;
  std::string xRay4Label;
  std::string rangeLabel;

  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title '4.4um Ti #2 for " << shotNumber << "'";
  xRay2Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title '4.1um Al #4 for " << shotNumber << "'";
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

  plot2VectorData(timeXRayDetaqGlobal, xRay2Global, xRay2Label, xRay4Global,
		  xRay4Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotXRay2
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRay2(int shotNumber, std::string tempDataFile,
		    std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay1Label;
  std::string xRay3Label;
  std::string rangeLabel;

  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title '2 um Ti #1 for " << shotNumber << "'";
  xRay1Label = oss.str();
  oss.str("");
 
  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title '1.27um Ni #3 for " << shotNumber << "'";
  xRay3Label = oss.str();
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

  plot2VectorData(timeXRayScopeGlobal, xRay1Global, xRay1Label,
		  xRay3Global, xRay3Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotXRay3
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRay3(int shotNumber, std::string tempDataFile,
		     std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay2Label;
  std::string xRay4Label;
  std::string xRay5Label;
  std::string rangeLabel;
  std::string titleLabel;

  //gsl_vector_scale(xRay2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title '4.4um Ti #2 for " << shotNumber << "'";
  xRay2Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title '4.1um Al #4 for " << shotNumber << "'";
  xRay4Label = oss.str();
  oss.str("");

  gsl_vector_scale(xRay5Global, 0.05/1.25);
  oss << "with line lw 3 lc rgb 'green' title 'Room light for " << shotNumber << "'";
  xRay5Label = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'Soft X-ray Signals for " << shotNumber << "\n";
  titleLabel = oss.str();
  oss.str("");
  
  std::string keyWords = "set ylabel 'Voltage (V)'\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/x-ray.png'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[0:]\n";
  
  keyWords.append(rangeLabel);
  keyWords.append(titleLabel);

  plot3VectorDataXY(timeXRayDetaqGlobal, xRay2Global, xRay2Label,
		    timeXRayDetaqGlobal, xRay4Global, xRay4Label,
		    timeXRayScopeGlobal,xRay5Global, xRay5Label, keyWords,
		    tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotXRayRatio1
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRayRatio1(int shotNumber, std::string tempDataFile,
			  std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay1Label;
  std::string xRay2Label;
  std::string xRayRatioLabel;
  std::string rangeLabel;

  oss << "with line lw 3 lc rgb 'black' title '12.5um Al #1 for " << shotNumber << "'";
  xRay1Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title '12.5um Al #2 for " << shotNumber << "'";
  xRay2Label = oss.str();
  oss.str("");

  oss << "with dots lc rgb 'green' title '1 / 2 " << shotNumber << "'";
  xRayRatioLabel = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Soft X-ray Ratios 1 / 2'\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/1over2.png'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[0:3]\n";
  
  keyWords.append(rangeLabel);

  gsl_vector* ratio = gsl_vector_alloc(xRay1Global->size);
  double temp1, temp2;
  for (int ii = 0; ii < (int) xRay1Global->size; ii++) {
    temp1 = gsl_vector_get(xRay1Global, ii);
    temp2 = gsl_vector_get(xRay2Global, ii);
    gsl_vector_set(ratio, ii, temp1 / temp2);
  }

  gsl_vector_scale(xRay1Global, -1.0);
  gsl_vector_scale(xRay2Global, -1.0);

  plot3VectorData(timeXRayGlobal, xRay1Global, xRay1Label, xRay2Global, xRay2Label,
		  ratio, xRayRatioLabel,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(ratio);
  
  return 0;

}


/******************************************************************************
 * Function: plotXRayRatio2
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotXRayRatio2(int shotNumber, std::string tempDataFile,
			  std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay3Label;
  std::string xRay4Label;
  std::string xRayRatioLabel;
  std::string rangeLabel;

  oss << "with line lw 3 lc rgb 'black' title '7um Al #3 for " << shotNumber << "'";
  xRay3Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title '7um Al #4 for " << shotNumber << "'";
  xRay4Label = oss.str();
  oss.str("");

  oss << "with dots lc rgb 'green' title '3 / 4 " << shotNumber << "'";
  xRayRatioLabel = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Soft X-ray Ratios 3 / 4'\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/3over4.png'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[0:3]\n";
  
  keyWords.append(rangeLabel);

  gsl_vector* ratio = gsl_vector_alloc(xRay3Global->size);
  double temp1, temp2;
  for (int ii = 0; ii < (int) xRay3Global->size; ii++) {
    temp1 = gsl_vector_get(xRay3Global, ii);
    temp2 = gsl_vector_get(xRay4Global, ii);
    gsl_vector_set(ratio, ii, temp1 / temp2);
  }

  gsl_vector_scale(xRay3Global, -1.0);
  gsl_vector_scale(xRay4Global, -1.0);

  plot3VectorData(timeXRayGlobal, xRay3Global, xRay3Label, xRay4Global, xRay4Label,
		  ratio, xRayRatioLabel,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(ratio);
  
  return 0;

}


/******************************************************************************
 * Function: plotBGLight
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotBGLight(int shotNumber, std::string tempDataFile,
		       std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string xRay3Label;
  std::string xRay5Label;
  std::string rangeLabel;
 
  gsl_vector_scale(xRay3Global, 1.2/0.15);
  oss << "with line lw 3 lc rgb 'black' title 'Broken? #3 for " << shotNumber << "'";
  xRay3Label = oss.str();
  oss.str("");

  //gsl_vector_scale(xRay5Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'Room light for " << shotNumber << "'";
  xRay5Label = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Photodiode output'\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot2VectorData(timeXRayScopeGlobal,
		  xRay3Global, xRay3Label, xRay5Global, xRay5Label,
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

static int plotNeutron(int shotNumber, std::string tempDataFile,
		       std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string neutron10Label;
  std::string neutron12Label;
  std::string rangeLabel;

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
 * Function: plotNeutron1
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotNeutron1(int shotNumber, std::string tempDataFile,
		       std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  std::string neutron10Label;
  std::string rangeLabel;
  std::string titleLabel;

  //gsl_vector_scale(neutron10Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'Neutron 10 for " << shotNumber << "'";
  neutron10Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'Neutron Signals for " << shotNumber << "\n";
  titleLabel = oss.str();
  oss.str("");
  
  std::string keyWords = "set ylabel 'Voltage (V)'\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/neutron.png'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);
  keyWords.append(titleLabel);
  
  plotVectorData(timeNeutronGlobal, neutron10Global, neutron10Label,
		 keyWords, tempDataFile, tempScriptFile);

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
