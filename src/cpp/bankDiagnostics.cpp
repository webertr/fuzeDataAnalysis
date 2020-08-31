#include "cpp/bankDiagnostics.h"

/******************************************************************************
 *
 * This is the source file to analyze the bank diagnostics
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
static int plotBankOut(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh);
static int plotBankCap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh);
static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh);
static int plotGV(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  double tLow, double tHigh);

/******************************************************************************
 * Global variables
 ******************************************************************************/
static gsl_vector *timeIgnitronGlobal;
static gsl_vector *ignitronOut1Global;
static gsl_vector *ignitronOut2Global;
static gsl_vector *ignitronOut3Global;
static gsl_vector *ignitronOut4Global;
static gsl_vector *ignitronOut5Global;
static gsl_vector *ignitronOut6Global;
static gsl_vector *ignitronCap1Global;
static gsl_vector *ignitronCap2Global;
static gsl_vector *ignitronCap3Global;
static gsl_vector *ignitronCap4Global;
static gsl_vector *ignitronCap5Global;
static gsl_vector *ignitronCap6Global;
static gsl_vector *timeVGapGlobal;
static gsl_vector *vgap1Global;
static gsl_vector *vgap2Global;
static gsl_vector *vgap3Global;
static gsl_vector *timeGVGlobal;
static gsl_vector *gv1Global;
static gsl_vector *gv2Global;
static gsl_vector *gv3Global;


#define TLOW 0
#define THIGH 300


/******************************************************************************
 * Function: softXRayRun
 * Inputs:
 * Returns: 
 * Description: Runs soft X-ray analysis
 ******************************************************************************/

int bankDiagnosticsRun() {

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
    plotBankOut(shotNumber, "data/igout1.txt", "data/igout1.sh", TLOW, THIGH);
    exit(0);    
  }
  else if ( (pid1 == 0) && (pid2 > 0) ) {
    plotBankCap(shotNumber, "data/igcap1.txt", "data/igcap1.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) ) {
    plotVGap(shotNumber, "data/vgap2.txt", "data/vgap2.sh", TLOW, THIGH);
  }
  else if ( (pid1 > 0) && (pid2 > 0) ) {
    plotGV(shotNumber, "data/gv1.txt", "data/gv2.sh", -1.5, 1.5);
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
  
  timeIgnitronGlobal = readMDSplusVectorDim(shotNumber, "\\i_ibm_1_out", "fuze");
  ignitronOut1Global = readMDSplusVector(shotNumber, "\\i_ibm_1_out", "fuze");
  ignitronOut2Global = readMDSplusVector(shotNumber, "\\i_ibm_2_out", "fuze");
  ignitronOut3Global = readMDSplusVector(shotNumber, "\\i_ibm_3_out", "fuze");
  ignitronOut4Global = readMDSplusVector(shotNumber, "\\i_ibm_4_out", "fuze");
  ignitronOut5Global = readMDSplusVector(shotNumber, "\\i_ibm_5_out", "fuze");
  ignitronOut6Global = readMDSplusVector(shotNumber, "\\i_ibm_6_out", "fuze");
  ignitronCap1Global = readMDSplusVector(shotNumber, "\\i_ibm_1_cap", "fuze");
  ignitronCap2Global = readMDSplusVector(shotNumber, "\\i_ibm_2_cap", "fuze");
  ignitronCap3Global = readMDSplusVector(shotNumber, "\\i_ibm_3_cap", "fuze");
  ignitronCap4Global = readMDSplusVector(shotNumber, "\\i_ibm_4_cap", "fuze");
  ignitronCap5Global = readMDSplusVector(shotNumber, "\\i_ibm_5_cap", "fuze");
  ignitronCap6Global = readMDSplusVector(shotNumber, "\\i_ibm_6_cap", "fuze");
  timeVGapGlobal = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  vgap1Global = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");
  vgap2Global = readMDSplusVector(shotNumber-1, "\\V_GAP", "fuze");
  vgap3Global = readMDSplusVector(shotNumber-2, "\\V_GAP", "fuze");
  timeGVGlobal = readMDSplusVectorDim(shotNumber, "\\i_gv_1_valve", "fuze");
  gv1Global = readMDSplusVector(shotNumber, "\\i_gv_1_valve", "fuze");
  gv2Global = readMDSplusVector(shotNumber, "\\i_gv_2_valve", "fuze");
  gv3Global = readMDSplusVector(shotNumber, "\\i_gv_3_valve", "fuze");

  return 0;

}


/******************************************************************************
 * Function: clearGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will clear the global variables.
 ******************************************************************************/

static int clearGlobalVariables() {

  gsl_vector_free(timeIgnitronGlobal);
  gsl_vector_free(ignitronOut1Global);
  gsl_vector_free(ignitronOut2Global);
  gsl_vector_free(ignitronOut3Global);
  gsl_vector_free(ignitronOut4Global);
  gsl_vector_free(ignitronOut5Global);
  gsl_vector_free(ignitronOut6Global);
  gsl_vector_free(ignitronCap1Global);
  gsl_vector_free(ignitronCap2Global);
  gsl_vector_free(ignitronCap3Global);
  gsl_vector_free(ignitronCap4Global);
  gsl_vector_free(ignitronCap5Global);
  gsl_vector_free(ignitronCap6Global);
  gsl_vector_free(timeVGapGlobal);
  gsl_vector_free(vgap1Global);
  gsl_vector_free(vgap2Global);
  gsl_vector_free(vgap3Global);
  gsl_vector_free(timeGVGlobal);
  gsl_vector_free(gv1Global);
  gsl_vector_free(gv2Global);
  gsl_vector_free(gv3Global);

  return 0;
  
}


/******************************************************************************
 * Function: plotBankOut
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotBankOut(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh) {

  std::ostringstream oss;
  std::string ignitronOut1Label;
  std::string ignitronOut2Label;
  std::string ignitronOut3Label;
  std::string ignitronOut4Label;
  std::string ignitronOut5Label;
  std::string ignitronOut6Label;
  std::string rangeLabel;

  gsl_vector_scale(timeIgnitronGlobal, 1E6);

  gsl_vector_scale(ignitronOut1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'I Out 1 for " << shotNumber << "'";
  ignitronOut1Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronOut2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'I Out 2 for " << shotNumber << "'";
  ignitronOut2Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronOut3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'blue' title 'I Out 3 for " << shotNumber << "'";
  ignitronOut3Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronOut4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'I Out 4 for " << shotNumber << "'";
  ignitronOut4Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronOut5Global, 1E-3);
  oss << "with line lw 3 lc rgb 'yellow' title 'I Out 5 for " << shotNumber << "'";
  ignitronOut5Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronOut6Global, 1E-3);
  oss << "with line lw 3 lc rgb 'purple' title 'I Out 6 for " << shotNumber << "'";
  ignitronOut6Label = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Ignitron Bank Out Currents'\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key right top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot6VectorData(timeIgnitronGlobal, ignitronOut1Global, ignitronOut1Label,
		  ignitronOut2Global, ignitronOut2Label, ignitronOut3Global, ignitronOut3Label,
		  ignitronOut4Global, ignitronOut4Label, ignitronOut5Global, ignitronOut5Label,
		  ignitronOut6Global, ignitronOut6Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotBankCap
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotBankCap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh) {

  std::ostringstream oss;
  std::string ignitronCap1Label;
  std::string ignitronCap2Label;
  std::string ignitronCap3Label;
  std::string ignitronCap4Label;
  std::string ignitronCap5Label;
  std::string ignitronCap6Label;
  std::string rangeLabel;

  gsl_vector_scale(timeIgnitronGlobal, 1E6);

  gsl_vector_scale(ignitronCap1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'I Cap 1 for " << shotNumber << "'";
  ignitronCap1Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronCap2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'I Cap 2 for " << shotNumber << "'";
  ignitronCap2Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronCap3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'blue' title 'I Cap 3 for " << shotNumber << "'";
  ignitronCap3Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronCap4Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'I Cap 4 for " << shotNumber << "'";
  ignitronCap4Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronCap5Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'I Cap 5 for " << shotNumber << "'";
  ignitronCap5Label = oss.str();
  oss.str("");

  gsl_vector_scale(ignitronCap6Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'I Cap 6 for " << shotNumber << "'";
  ignitronCap6Label = oss.str();
  oss.str("");
  
  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Ignitron Bank Cap Currents'\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key right top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot6VectorData(timeIgnitronGlobal, ignitronCap1Global, ignitronCap1Label,
		  ignitronCap2Global, ignitronCap2Label, ignitronCap3Global, ignitronCap3Label,
		  ignitronCap4Global, ignitronCap4Label, ignitronCap5Global, ignitronCap5Label,
		  ignitronCap6Global, ignitronCap6Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotVgap
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh) {

  std::ostringstream oss;
  std::string vgap1Label;
  std::string vgap2Label;
  std::string vgap3Label;
  std::string rangeLabel;

  gsl_vector_scale(timeVGapGlobal, 1E6);

  gsl_vector_scale(vgap1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'V_{GAP} for " << shotNumber << "'";
  vgap1Label = oss.str();
  oss.str("");

  gsl_vector_scale(vgap2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'V_{GAP} for " << shotNumber-1 << "'";
  vgap2Label = oss.str();
  oss.str("");

  gsl_vector_scale(vgap3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'V_{GAP} for " << shotNumber-2 << "'";
  vgap3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'V_{GAP}'\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key right top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot3VectorData(timeVGapGlobal, vgap1Global, vgap1Label, vgap2Global, vgap2Label,
		  vgap3Global, vgap3Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotGV
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotGV(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  double tLow, double tHigh) {

  std::ostringstream oss;
  std::string gv1Label;
  std::string gv2Label;
  std::string gv3Label;
  std::string rangeLabel;

  gsl_vector_scale(timeGVGlobal, 1E3);

  gsl_vector_scale(gv1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'GV1 for " << shotNumber << "'";
  gv1Label = oss.str();
  oss.str("");

  gsl_vector_scale(gv2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'GV2 for " << shotNumber << "'";
  gv2Label = oss.str();
  oss.str("");

  gsl_vector_scale(gv3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'GV3 for " << shotNumber << "'";
  gv3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Gas Valve Current'\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time (ms)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot3VectorData(timeGVGlobal, gv1Global, gv1Label, gv2Global, gv2Label,
		  gv3Global, gv3Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testThisFunction();

int testBankDiagnostics() {

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
