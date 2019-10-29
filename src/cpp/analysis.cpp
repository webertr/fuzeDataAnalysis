#include "cpp/analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  int tLow, int tHigh);
static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh);
static int plotCompCurrent(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
			   int tLow, int tHigh);
static int plotNeutron(int shotNumber, std::string neutronNode,
		       std::string tempDataFile, std::string tempScriptFile, int tLow, int tHigh);
static int plotAzimuthalArray(int shotNumber, std::string nodeName,
			      std::string tempDataFile, std::string tempScriptFile, 
			      int tLow, int tHigh);
static int plotM1Mode(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		      int tLow, int tHigh);
static int kiranaImageAnalysis();
static int plotPinchCurrentData(int shotNumber, std::string tempDataFile,
				std::string tempScriptFile, int tLow, int tHigh);
static int plotM0Mode(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		      int tLow, int tHigh);
static int plotPinchCurrentAverageData(int shotNumber, std::string tempDataFile,
				       std::string tempScriptFile, int tLow, int tHigh);
static int saveIBMData();

#define TLOW 20
#define THIGH 60

/******************************************************************************
 * Function: plotPostShotAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

int plotPostShotAnalysis() {

  int shotNumber,
    currShotNumber = getCurrentPulseNumber();

  printf("\nEnter Pulse Number> ");
  scanf("%d", &shotNumber);

  if (shotNumber <= 0) {
    shotNumber = currShotNumber+shotNumber;
  }

  getchar();

  saveIBMData();

  return 0;
  
  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    plotIP(shotNumber, "data/ip1.txt", "data/ip1.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    plotVGap(shotNumber, "data/vgap2.txt", "data/vgap2.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    plotCompCurrent(shotNumber, "data/comp3.txt", "data/comp3.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    plotM1Mode(shotNumber, "data/m14.txt", "data/m15.sh", TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    plotNeutron(shotNumber, "\\neutron_1_s", "data/neutron5.txt", "data/neutron5.sh",
		TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {
    plotNeutron(shotNumber, "\\neutron_4_s", "data/neutron6.txt", "data/neutron6.sh",
		TLOW, THIGH);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }

  if (0) {
    plotNeutron(shotNumber, "\\neutron_11_s", "data/neutron4.txt", "data/neutron4.sh", TLOW,
		THIGH);
    plotIP(shotNumber, "data/ip1.txt", "data/ip1.sh", TLOW, THIGH);
    plotVGap(shotNumber, "data/vgap2.txt", "data/vgap2.sh", TLOW, THIGH);
    plotCompCurrent(shotNumber, "data/comp3.txt", "data/comp3.sh", TLOW, THIGH);
    plotAzimuthalArray(shotNumber, "\\b_p15_000", "data/azimuth4.txt", "data/azimuth4.sh",
		       TLOW, THIGH);
    kiranaImageAnalysis();
    plotPinchCurrentData(shotNumber, "data/data.txt", "data/data.sh", 0, 100);
    plotM0Mode(shotNumber, "data/m0.txt", "data/m0.sh", 20, 50);
    plotPinchCurrentAverageData(shotNumber, "data/data.txt", "data/data.sh", 20, 50);
  }

  return 0;

}


/******************************************************************************
 * Function: kiranaImageAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will analyze images from the kirana
 ******************************************************************************/

static int kiranaImageAnalysis() {

  Kirana kirana("/home/fuze/webertrNAS/kiranaVideo/170915068.SVF");
  
  kirana.setCropRow(315, 410);
  //kirana.saveImage(0, "data/kiranaImage.dat");
  
  gsl_matrix *image = kirana.getImage(1);

  std::string keywords = "set palette gray\nset title 'corn'"; 
  plotImageData(image, 1, 1, keywords, "data/temp.dat", "data/temp.sh");

  return 0;

}


/******************************************************************************
 * Function: plotIP
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *ip1;
  gsl_vector *ip2;
  gsl_vector *ip3;
  std::string ip1Label;
  std::string ip2Label;
  std::string ip3Label;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\I_P", "fuze");
  gsl_vector_scale(time, 1E6);

  ip1 = readMDSplusVector(shotNumber, "\\I_P", "fuze");
  gsl_vector_scale(ip1, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'IP for " << shotNumber << "'";
  ip1Label = oss.str();
  oss.str("");

  ip2 = readMDSplusVector(shotNumber-1, "\\I_P", "fuze");
  gsl_vector_scale(ip2, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'IP for " << shotNumber-1 << "'";
  ip2Label = oss.str();
  oss.str("");

  ip3 = readMDSplusVector(shotNumber-2, "\\I_P", "fuze");
  gsl_vector_scale(ip3, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'IP for " << shotNumber-2 << "'";
  ip3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'I_{P}'\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";

  keyWords.append(rangeLabel);

  plot3VectorData(time, ip1, ip1Label, ip2, ip2Label, ip3, ip3Label,
		  keyWords, tempDataFile, tempScriptFile);

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
  gsl_vector *time;
  gsl_vector *vgap1;
  gsl_vector *vgap2;
  gsl_vector *vgap3;
  std::string vgap1Label;
  std::string vgap2Label;
  std::string vgap3Label;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  gsl_vector_scale(time, 1E6);

  vgap1 = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");
  gsl_vector_scale(vgap1, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'V_{GAP} for " << shotNumber << "'";
  vgap1Label = oss.str();
  oss.str("");

  vgap2 = readMDSplusVector(shotNumber-1, "\\V_GAP", "fuze");
  gsl_vector_scale(vgap2, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'V_{GAP} for " << shotNumber-1 << "'";
  vgap2Label = oss.str();
  oss.str("");

  vgap3 = readMDSplusVector(shotNumber-2, "\\V_GAP", "fuze");
  gsl_vector_scale(vgap3, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'V_{GAP} for " << shotNumber-2 << "'";
  vgap3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'V_{GAP}'\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot3VectorData(time, vgap1, vgap1Label, vgap2, vgap2Label, vgap3, vgap3Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotCompCurrent
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotCompCurrent(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
			   int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5;
  gsl_vector *p15;
  gsl_vector *p35;
  gsl_vector *p45;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p5_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5 = getM0Mode(shotNumber, "\\b_p5_000");
  gsl_vector_scale(p5, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'I_{P-5}'";
  p5Label = oss.str();
  oss.str("");

  p15 = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(p15, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'I_{P-15}'";
  p15Label = oss.str();
  oss.str("");

  p35 = getM0Mode(shotNumber, "\\b_p35_000");
  gsl_vector_scale(p35, 5E2);
  oss << "with line lw 3 lc rgb 'green' title 'I_{P-35}'";
  p35Label = oss.str();
  oss.str("");

  p45 = getM0Mode(shotNumber, "\\b_p45_000");
  gsl_vector_scale(p45, 5E2);
  oss << "with line lw 3 lc rgb 'blue' title 'I_{P-45}'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=0 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'Current (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[:]\n"
      << "set key left top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p5, p5Label, p15, p15Label, p35, p35Label, p45, p45Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotM1Mode
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotM1Mode(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		      int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5;
  gsl_vector *p15;
  gsl_vector *p35;
  gsl_vector *p45;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p5_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5 = getM1Mode(shotNumber, "\\b_p5_000");
  oss << "with line lw 3 lc rgb 'black' title 'm=1 at P5'";
  p5Label = oss.str();
  oss.str("");

  p15 = getM1Mode(shotNumber, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'red' title 'm=1 at P15'";
  p15Label = oss.str();
  oss.str("");

  p35 = getM1Mode(shotNumber, "\\b_p35_000");
  oss << "with line lw 3 lc rgb 'green' title 'm=1 at P35'";
  p35Label = oss.str();
  oss.str("");

  p45 = getM1Mode(shotNumber, "\\b_p45_000");
  oss << "with line lw 3 lc rgb 'blue' title 'm=1 at P45'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=1 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'Normalized mode'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:1]\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p5, p5Label, p15, p15Label, p35, p35Label, p45, p45Label,
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

static int plotNeutron(int shotNumber, std::string neutronNode,
		       std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *neutron1;
  gsl_vector *neutron2;
  gsl_vector *neutron3;
  std::string neutron1Label;
  std::string neutron2Label;
  std::string neutron3Label;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, neutronNode, "fuze");
  gsl_vector_scale(time, 1E6);

  neutron1 = readMDSplusVector(shotNumber, neutronNode, "fuze");
  oss << "with line lw 3 lc rgb 'black' title 'ND for " << shotNumber << "'";
  neutron1Label = oss.str();
  oss.str("");

  neutron2 = readMDSplusVector(shotNumber-1, neutronNode, "fuze");
  oss << "with line lw 3 lc rgb 'red' title 'ND for " << shotNumber-1 << "'";
  neutron2Label = oss.str();
  oss.str("");

  neutron3 = readMDSplusVector(shotNumber-2, neutronNode, "fuze");
  oss << "with line lw 3 lc rgb 'green' title 'ND for " << shotNumber-2 << "'";
  neutron3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Neutron Detectors'\n"
    "set ylabel 'Signal (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left bottom\n"
    "set yrange[:]\n";

  keyWords.append(rangeLabel);
  
  plot3VectorData(time, neutron1, neutron1Label, neutron2, neutron2Label, neutron3, neutron3Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotAzimuthalArray
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotAzimuthalArray(int shotNumber, std::string nodeName,
			      std::string tempDataFile, std::string tempScriptFile,
			      int tLow, int tHigh) {

  std::ostringstream oss;
  std::string p_000Label,
    p_045Label,
    p_090Label,
    p_135Label,
    p_180Label,
    p_225Label,
    p_270Label,
    p_315Label;

  std::string rangeLabel;

  gsl_matrix *azimuthalArray = get8AzimuthalArray(shotNumber, nodeName);  

  int vecSize = azimuthalArray->size1;

  gsl_vector* time = gsl_vector_alloc(vecSize),
    *p_000 = gsl_vector_alloc(vecSize),
    *p_045 = gsl_vector_alloc(vecSize),
    *p_090 = gsl_vector_alloc(vecSize),
    *p_135 = gsl_vector_alloc(vecSize),
    *p_180 = gsl_vector_alloc(vecSize),
    *p_225 = gsl_vector_alloc(vecSize),
    *p_270 = gsl_vector_alloc(vecSize),
    *p_315 = gsl_vector_alloc(vecSize);

  gsl_matrix_get_col(time, azimuthalArray, 0);
  gsl_vector_scale(time, 1E6);
  gsl_matrix_get_col(p_000, azimuthalArray, 1);
  gsl_matrix_get_col(p_045, azimuthalArray, 2);
  gsl_matrix_get_col(p_090, azimuthalArray, 3);
  gsl_matrix_get_col(p_135, azimuthalArray, 4);
  gsl_matrix_get_col(p_180, azimuthalArray, 5);
  gsl_matrix_get_col(p_225, azimuthalArray, 6);
  gsl_matrix_get_col(p_270, azimuthalArray, 7);
  gsl_matrix_get_col(p_315, azimuthalArray, 8);

  oss << "with line lw 3 lc rgb 'black' title '0 Deg.'";
  p_000Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title '45 Deg.'";
  p_045Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'green' title '90 Deg.'";
  p_090Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'orange' title '135 Deg.'";
  p_135Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'yellow' title '180 Deg.'";
  p_180Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'purple' title '225 Deg.'";
  p_225Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'brown' title '270 Deg.'";
  p_270Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'pink' title '315 Deg.'";
  p_315Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Azimuthal Array'\n"
    "set xlabel 'Time ({/Symbol m}sec)\n"
    "set key left top\n"; 

  keyWords.append(rangeLabel);

  plot8VectorData(time, p_000, p_000Label, p_045, p_045Label, p_090, p_090Label, 
		  p_135, p_135Label, p_180, p_180Label, p_225, p_225Label, p_270, p_270Label,
		  p_315, p_315Label, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p_000);
  gsl_vector_free(p_045);
  gsl_vector_free(p_090);
  gsl_vector_free(p_135);
  gsl_vector_free(p_180);
  gsl_vector_free(p_225);
  gsl_vector_free(p_270);
  gsl_vector_free(p_315);

  gsl_matrix_free(azimuthalArray);

  return 0;

}


/******************************************************************************
 * Function: plotPinchCurrentData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchCurrentData(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
				int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p15M1;
  gsl_vector *p15M0;
  std::string p15M0Label;
  std::string p15M1Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p15M0 = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(p15M0, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0'";
  p15M0Label = oss.str();
  oss.str("");
  
  p15M1 = getM1Mode(shotNumber, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'red' title 'm=1' axes x1y2";
  p15M1Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm= 0 and 1 at z=15 cm for shotnumber: " << shotNumber << "'\n"
    //<< "set terminal png\n"
    //<< "set output '/home/webertr/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set y2label 'Normalized m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set y2range[0:1]\n"
      << "set y2tics nomirror tc lt 2\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, p15M0, p15M0Label, p15M1, p15M1Label, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p15M1);
  gsl_vector_free(p15M0);

  return 0;

}


/******************************************************************************
 * Function: plotM0Mode
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotM0Mode(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		      int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5;
  gsl_vector *p15;
  gsl_vector *p35;
  gsl_vector *p45;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p5_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5 = getM0Mode(shotNumber, "\\b_p5_000");
  gsl_vector_scale(p5, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0 at P5'";
  p5Label = oss.str();
  oss.str("");

  p15 = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(p15, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'm=0 at P15'";
  p15Label = oss.str();
  oss.str("");

  p35 = getM0Mode(shotNumber, "\\b_p35_000");
  gsl_vector_scale(p35, 5E2);
  oss << "with line lw 3 lc rgb 'green' title 'm=0 at P35'";
  p35Label = oss.str();
  oss.str("");

  p45 = getM0Mode(shotNumber, "\\b_p45_000");
  gsl_vector_scale(p45, 5E2);
  oss << "with line lw 3 lc rgb 'blue' title 'm=0 at P45'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=0 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p5, p5Label, p15, p15Label, p35, p35Label, p45, p45Label,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p5);
  gsl_vector_free(p15);
  gsl_vector_free(p35);
  gsl_vector_free(p45);

  return 0;

}


/******************************************************************************
 * Function: plotPinchCurrentAverageData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchCurrentAverageData(int shotNumber, std::string tempDataFile,
				       std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5M0;
  gsl_vector *p15M0;
  gsl_vector *p25M0;
  gsl_vector *p35M0;
  gsl_vector *p45M0;
  gsl_vector *pM0;
  gsl_vector *p5M1;
  gsl_vector *p15M1;
  gsl_vector *p25M1;
  gsl_vector *p35M1;
  gsl_vector *p45M1;
  gsl_vector *pM1;
  
  std::string pM0Label;
  std::string pM1Label;
  std::string keyWords;
  std::string rangeLabel;

  double avg = 0;

  int ii, sizeVec;
  
  time = readMDSplusVectorDim(shotNumber, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5M0 = getM0Mode(shotNumber, "\\b_p5_000");
  p15M0 = getM0Mode(shotNumber, "\\b_p15_000");
  p25M0 = getM0Mode(shotNumber, "\\b_p25_000");
  p35M0 = getM0Mode(shotNumber, "\\b_p35_000");
  p45M0 = getM0Mode(shotNumber, "\\b_p45_000");

  p5M1 = getM1Mode(shotNumber, "\\b_p5_000");
  p15M1 = getM1Mode(shotNumber, "\\b_p15_000");
  p25M1 = getM1Mode(shotNumber, "\\b_p25_000");
  p35M1 = getM1Mode(shotNumber, "\\b_p35_000");
  p45M1 = getM1Mode(shotNumber, "\\b_p45_000");

  sizeVec = p5M0->size;
  pM0 = gsl_vector_alloc(sizeVec);
  pM1 = gsl_vector_alloc(sizeVec);
  
  /* Averaging all axial positions together */
  for (ii = 0; ii < sizeVec; ii++) {
    
    avg = gsl_vector_get(p5M0, ii) + gsl_vector_get(p15M0, ii) + gsl_vector_get(p25M0, ii) \
      + gsl_vector_get(p35M0, ii) + gsl_vector_get(p45M0, ii);
    gsl_vector_set(pM0, ii, avg/5.0);

    avg = gsl_vector_get(p5M1, ii) + gsl_vector_get(p15M1, ii) + gsl_vector_get(p25M1, ii) \
      + gsl_vector_get(p35M1, ii) + gsl_vector_get(p45M1, ii);
    gsl_vector_set(pM1, ii, avg/5.0);
    
  }
  
  gsl_vector_scale(pM0, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0'";
  pM0Label = oss.str();
  oss.str("");
  
  oss << "with line lw 3 lc rgb 'red' title 'm=1' axes x1y2";
  pM1Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm= 0 and 1 averaged over z=5-45cm for #" << shotNumber << "'\n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set y2label 'Normalized m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set y2range[0:1]\n"
      << "set y2tics nomirror tc lt 2\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, pM0, pM0Label, pM1, pM1Label, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p5M0);
  gsl_vector_free(p15M0);
  gsl_vector_free(p25M0);
  gsl_vector_free(p35M0);
  gsl_vector_free(pM0);
  gsl_vector_free(p5M1);
  gsl_vector_free(p15M1);
  gsl_vector_free(p25M1);
  gsl_vector_free(p35M1);
  gsl_vector_free(pM1);

  return 0;

}


static int saveIBMData() {

  std::string fileNameOut = "/home/fuze/Downloads/Ouput_191028003.csv";
  std::string fileNameGap = "/home/fuze/Downloads/Gap_191028003.csv";

  int shotNumber = 191028003;
  int sizeRowOut;
  int sizeRowGap;
  int ii;

  gsl_vector *timeOut;
  gsl_vector *timeGap;
  gsl_vector *outRaw1;
  gsl_vector *outRaw2;
  gsl_vector *outRaw3;
  gsl_vector *outRaw4;
  gsl_vector *outRaw5;
  gsl_vector *outRaw6;
  gsl_vector *vgap;

  remove(fileNameOut.c_str());
  remove(fileNameGap.c_str());
  FILE *fpDataOut = fopen(fileNameOut.c_str(), "w");
  FILE *fpDataGap = fopen(fileNameGap.c_str(), "w");

  if ( (fpDataOut == NULL) || (fpDataGap == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  timeGap = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  vgap = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");

  timeOut = readMDSplusVectorDim(shotNumber, "\\I_IBM_1_OUT:RAW", "fuze");
  outRaw1 = readMDSplusVector(shotNumber, "\\I_IBM_1_OUT:RAW", "fuze");
  outRaw2 = readMDSplusVector(shotNumber, "\\I_IBM_2_OUT:RAW", "fuze");
  outRaw3 = readMDSplusVector(shotNumber, "\\I_IBM_3_OUT:RAW", "fuze");
  outRaw4 = readMDSplusVector(shotNumber, "\\I_IBM_4_OUT:RAW", "fuze");
  outRaw5 = readMDSplusVector(shotNumber, "\\I_IBM_5_OUT:RAW", "fuze");
  outRaw6 = readMDSplusVector(shotNumber, "\\I_IBM_6_OUT:RAW", "fuze");
  
  sizeRowOut = (int) timeOut->size;
  sizeRowGap = (int) timeGap->size;

  for (ii = 0; ii < sizeRowOut; ii++) {

    fprintf(fpDataOut, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(timeOut, ii), 
	    gsl_vector_get(outRaw1, ii), gsl_vector_get(outRaw2, ii), gsl_vector_get(outRaw3, ii), 
	    gsl_vector_get(outRaw4, ii), gsl_vector_get(outRaw5, ii), gsl_vector_get(outRaw6, ii));

  }

  for (ii = 0; ii < sizeRowGap; ii++) {

    fprintf(fpDataGap, "%g\t%g\n", gsl_vector_get(timeGap, ii), 
	    gsl_vector_get(vgap, ii));

  }

  gsl_vector_free(timeOut);
  gsl_vector_free(timeGap);
  gsl_vector_free(outRaw1);
  gsl_vector_free(outRaw2);
  gsl_vector_free(outRaw3);
  gsl_vector_free(outRaw4);
  gsl_vector_free(outRaw5);
  gsl_vector_free(outRaw6);
  gsl_vector_free(vgap);

  return 0;

}
