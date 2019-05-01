#include "cpp/analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile);
static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile);
static int plotCompCurrent(int shotNumber, std::string tempDataFile, std::string tempScriptFile);
static int plotNeutron(int shotNumber, std::string neutronNode,
		       std::string tempDataFile, std::string tempScriptFile);


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
  
  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    plotIP(shotNumber, "data/ip1.txt", "data/ip1.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    plotVGap(shotNumber, "data/vgap2.txt", "data/vgap2.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    plotCompCurrent(shotNumber, "data/comp3.txt", "data/comp3.sh");
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    plotNeutron(shotNumber, "\\neutron_8_s", "data/neutron4.txt", "data/neutron4.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }

  if (0) {
    plotNeutron(shotNumber, "\\neutron_11_s", "data/neutron4.txt", "data/neutron4.sh");
  }

  return 0;

}


/******************************************************************************
 * Function: kiranaImageAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will analyze images from the kirana
 ******************************************************************************/

int kiranaImageAnalysis() {

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

static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *ip1;
  gsl_vector *ip2;
  gsl_vector *ip3;
  std::string ip1Label;
  std::string ip2Label;
  std::string ip3Label;

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

  std::string keyWords = "set title 'I_{P}'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]";
  
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

static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *vgap1;
  gsl_vector *vgap2;
  gsl_vector *vgap3;
  std::string vgap1Label;
  std::string vgap2Label;
  std::string vgap3Label;

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

  std::string keyWords = "set title 'V_{GAP}'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]";
  
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

static int plotCompCurrent(int shotNumber, std::string tempDataFile, std::string tempScriptFile) {

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

  time = readMDSplusVectorDim(shotNumber, "\\m_0_p5", "fuze");
  gsl_vector_scale(time, 1E6);

  p5 = readMDSplusVector(shotNumber, "\\m_0_p5", "fuze");
  gsl_vector_scale(p5, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'I_{P-5}'";
  p5Label = oss.str();
  oss.str("");

  p15 = readMDSplusVector(shotNumber, "\\m_0_p15", "fuze");
  gsl_vector_scale(p15, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'I_{P-15}'";
  p15Label = oss.str();
  oss.str("");

  p35 = readMDSplusVector(shotNumber, "\\m_0_p35", "fuze");
  gsl_vector_scale(p35, 5E2);
  oss << "with line lw 3 lc rgb 'green' title 'I_{P-35}'";
  p35Label = oss.str();
  oss.str("");

  p45 = readMDSplusVector(shotNumber, "\\m_0_p45", "fuze");
  gsl_vector_scale(p45, 5E2);
  oss << "with line lw 3 lc rgb 'blue' title 'I_{P-45}'";
  p45Label = oss.str();
  oss.str("");

  oss << "set title 'm=0 at different z values for " << shotNumber << "'\n"
      << "set xrange[0:100]\n"
      << "set ylabel 'Current (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[:]\n"
      << "set key left top";

  keyWords = oss.str();
  oss.str("");
  
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
		       std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *neutron1;
  gsl_vector *neutron2;
  gsl_vector *neutron3;
  std::string neutron1Label;
  std::string neutron2Label;
  std::string neutron3Label;

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

  std::string keyWords = "set title 'Neutron Detectors'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Signal (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left bottom\n"
    "set yrange[:]";
  
  plot3VectorData(time, neutron1, neutron1Label, neutron2, neutron2Label, neutron3, neutron3Label,
		  keyWords, tempDataFile, tempScriptFile);

  return 0;

}
