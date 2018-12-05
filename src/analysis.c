#include "analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotPostShotIFData(int shotNumber, int tmin, int tmax, char *saveFile);
static int plotPostShotCompData(int shotNumber, int tmin, int tmax, char *saveFile);
static int plotOffAxisDisplacement(int shotNumber);
static int plotPostShotSymmetryCheck(int shotNumber, int tmin, int tmax);
static int plotPostShotIPData(int shotNumber1, int shotNumber2, int shotNumber3,
			      char *tempDataFile, char *tempScriptFile);
static int plotPostShotNeutronData(int shotNumber1, int shotNumber2, int shotNumber3,
				   int detectorNum, char *tempDataFile, char *tempScriptFile);
static int plotPostShotAccelData(int shotNumber, char *tempDataFile, char *tempScriptFile);
static int plotPostShotAccelDataN95(int shotNumber, int tmin, int tmax);
static int plotPostShotIPData(int shotNumber1, int shotNumber2, int shotNumber3, 
			      char *tempDataFile, char *tempScriptFile);
static int plotPostShotIV(int shotNumber1, int shotNumber2, int shotNumber3, 
			  char *tempDataFile, char *tempScriptFile);
static int plotPostShotGVCurrent(int shotNumber, char *tempDataFile, char *tempScriptFile);
static int plotPostShotModeData(int shotNumber1, int shotNumber2, int shotNumber3, 
				char *tempDataFile, char *tempScriptFile);
static int plotPostShotIPinchData(int shotNumber1, int shotNumber2, int shotNumber3, 
				  char *tempDataFile, char *tempScriptFile);
static int plotPostShotMultiNeutronData(int shotNumber, char *tempDataFile, 
					char *tempScriptFile);

/******************************************************************************
 * Function: hologramAnalysis
 * Inputs: 
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int hologramAnalysis() {

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  //hologramMain(&param);

  char *plotOptions = "set size ratio -1\n"
    "set terminal png\n"
    "set output '/home/fuze/Downloads/candleFlame.png'\n"
    "set title 'Candle Flame Reconstruction' font 'Times Bold,14'\n"
    "set label front 'Radians' at graph 1.25,0.6 "
    "rotate by 270 font 'Times Bold, 14'\n"
    //"set xrange [0:4000]\n"
    //"set yrange [0:6000]\n"
    //"set xtics 0, 1000, 4000\n"
    "set xrange [2650:3622]\n"
    "set yrange [1902:3968]\n"
    "set xtics 2600, 300, 3622\n"
    "set xlabel 'x (pixels)' font 'Times Bold,14' offset 0,0\n"
    "set ylabel 'y (pixels)' font 'Times Bold,14' offset 1,0\n";

  plotImageDataFile(param.fileHologram, 1, 1, 1, plotOptions, "data/dhiScript.sh");
  //plotImageDataFile(param.fileTwinImage, 1, 1, 1, "set size ratio -1",
  //		    "data/dhiImage.dat", "data/dhiScript.sh");
  //plotImageDataFile(param.fileWrappedPhase, 1, 1, 1, "set size ratio -1",
  //		    "data/dhiImage.dat", "data/dhiScript.sh");
  //plotImageDataFile(param.fileLineIntPos, 1, 1, 1, "set size ratio -1",
  //		    "data/dhiImage.dat", "data/dhiScript.sh");
  //plotImageDataFile(param.fileLineInt, 1, 1, 1, "set size ratio -1", 
  //		    "data/dhiImage.dat", "data/dhiScript.sh");

  //plotMatrixColVColErrorDataFile(param.fileDensity, 0, 1+60*2, 1+60*2+1, "");
  //plotMatrixColVColDataFile(param.fileLeftInvert, 0, 60, "");
  //plotMatrixColDataFile(param.fileLineIntText, 10, "");
  //plotImageDataFile(param.fileHologram, 1, 1, 1,
  //	              "set size ratio -1\nset term png\nset output 'data/temp.png'",
  //                  "data/dhiImage.dat", "data/dhiScript.sh");
  //plot2MatrixColDataFile(param.fileRightInvert, 60, 
  //			 param.fileLeftInvert, 60, "");
  //plotImageDataFile(param.fileFullInvert, 1, 1, 1, "set size ratio -1\n",
  //                  "data/dhiImage.dat", "data/dhiScript.sh");

  
  /* 
   * Fancy plot of the hologram
   */
  if (0) {

    char *keywords = "set size ratio -1\n"
      //"set terminal png\n"
      //"set output '/home/fuze/Downloads/180710008_Line_Integrated.png'\n"
      "set title 'Pulse 180723022' font 'Times Bold,14'\n"
      "set xrange [13.624:14.3763]\n"
      "set yrange [-0.69125:0.682125]\n"
      "set xlabel 'z (cm)' font 'Times Bold,14' offset 0,0\n"
      "set ylabel 'b (cm)' font 'Times Bold,14' offset 1,0\n"
      "set xtics 13.5, 0.25, 14.5\n"
      "set label front 'Line integrated n_{e} (cm^{-2})' at graph 1.60,0.20 "
      "rotate by 90 font 'Times Bold, 14'\n";
    plotImageDataFile(param.fileLineIntPos, 100, 100, 1E-4, keywords,
		      "data/dhiScript.sh");

  }

  /*
   * Fancy plot for the abel inversion with error bars 
   */
  if (0) {

    char *errorKeywords = "set size ratio 1\n"
      "set term png\n"
      "set output '/home/fuze/Downloads/180723022ne.png'\n"
      "set title 'Radial density profile for Pulse 180723022' font 'Times Bold, 14'\n"
      "set tics font 'Times Bold, 14'\n"
      "set grid\n"
      //"set xrange [0:0.45]\n"
      "set yrange [0:]\n"
      "set xlabel 'r (cm)' font 'Times Bold, 18'\n"
      "set ylabel 'n_{e} (cm^{-3})' font 'Times Bold, 18'\n";
    plotMatrixColVColErrorDataFile(param.fileDensity, 0, 1+60*2, 1+60*2+1, 
				   1E2, 1E-6, 1E-6, errorKeywords);
    
  }


  /*
   * Plotting the azimuthal magnetic field
   */
  if (0) {

    char *bThetaKeywords = "set size ratio 1\n"
      "set terminal png\n"
      "set output '/home/fuze/Downloads/180723022BField.png'\n"
      "set title 'Radial B_{/Symbol q} profile for Pulse 180723022' font 'Time Bold, 16'\n"
      "set tics font 'Times Bold, 14'\n"
      "set grid\n"
      "set xrange [0:]\n"
      "set yrange [0:]\n"
      "set xlabel 'r (cm)' font 'Times Bold, 18'\n"
      "set ylabel 'B_{/Symbol q} (Tesla)' font 'Times Bold, 18'\n";

    plotMatrixColVColErrorDataFile(param.fileBTheta, 0, 1+10*2, 1+10*2+1, 
				   1E2, 1, 1, bThetaKeywords);
    
  }

  /*
   * Plotting the temperature 
   */
  if (0) {

    char *tempKeywords = "set size ratio 1\n"
      "set terminal png\n"
      "set output '/home/fuze/Downloads/180215012Temp.png'\n"
      "set title 'T_{e}(r) for Pulse #180215012' font 'Time Bold, 16'\n"
      "set tics font 'Times Bold, 14'\n"
      "set grid\n"
      "set xrange [0:0.45]\n"
      "set yrange [0:]\n"
      "set xlabel 'r (cm)' font 'Times Bold, 18'\n"
      "set ylabel 'T_{e} (eV)' font 'Times Bold, 18'\n";

    plotMatrixColVColErrorDataFile(param.fileTemperature, 0, 1+60*2, 1+60*2+1, 
				   1E2, 1, 1, tempKeywords);
    
  }


  /*
   * Plotting the temperature and b field on same axis
   */
  if (0) {

    char *tempKeywords = "set size ratio 1\n"
      //"set terminal png\n"
      //"set output '/home/webertr/Downloads/180215012Tempne.png'\n"
      "set title 'Radial density and temperature profile for 180215012' font 'Time Bold, 16'\n"
      "set tics font 'Times Bold, 14'\n"
      "set grid\n"
      "set xrange [0:0.45]\n"
      "set yrange [0:]\n"
      "set xlabel 'r (cm)' font 'Times Bold, 18'\n"
      "set y2label 'T (eV)' font 'Times Bold, 18'\n"
      "set y2tics nomirror tc lt 2\n"
      "set ylabel 'n_{e} (cm^{-3})' font 'Times Bold, 18'\n";

    plotMatrixColVColErrorData2Files2Axes(param.fileDensity, 0, 1+60*2, 1+60*2+1, "Density", 
					  1E2, 1E-6, 1E-6, param.fileTemperature, 
					  0, 1+60*2, 1+60*2+1, 1E2, 1, 1, 
					  "Temperature", tempKeywords);
    
  }

  
  return 0;

}


/******************************************************************************
 * Function: plasmaParameters
 * Inputs: 
 * Returns: int
 * Description: This will call some functions to calculate some plasma parameters
 ******************************************************************************/

int plasmaParameters() {

  gyroRadius(0.25E4, 5E3);
  
  return 0;

}


/******************************************************************************
 * Function: simluateAccel
 * Inputs: 
 * Returns: int
 * Description: This will call a function to simulate a charged particles
 * in the acceleration region of a marshall gun
 ******************************************************************************/

int simluateAccel() {

  char *fileName = "data/simul.txt";

  simulateParticleAccel(5E3, 100E3, fileName);
  
  plotMatrixColVColDataFile(fileName, 4, 2, "set xlabel 'Z(m)'\nset ylabel 'r(m)'");
  plotMatrixColVColDataFile(fileName, 1, 2, "set xlabel 't(sec)'\nset ylabel 'r(m)'");
  plotMatrixColVColDataFile(fileName, 1, 4, "set xlabel 't(sec)'\nset ylabel 'z(m)'");
  
  return 0;

}

/******************************************************************************
 * Function: plotPostAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

int plotPostAnalysis() {

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
    //plotPostShotIPData(shotNumber, shotNumber - 1, shotNumber - 2,
    //		       "data/ipData.txt", "data/ipDataScript.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    //plotPostShotModeData(shotNumber, shotNumber - 1, shotNumber - 2, 
    //			 "data/modeData.txt", "data/modeDataScript.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    //plotPostShotIV(shotNumber, shotNumber - 1, shotNumber - 2, 
    //		   "data/ivData.txt", "data/ivDataScript.sh");
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    //plotPostShotAccelData(shotNumber, "data/accelData.txt", "data/accelScript.sh");
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    //plotPostShotIPinchData(shotNumber, shotNumber - 1, shotNumber - 2,
    //			   "data/ipData.txt", "data/ipDataScript.sh");
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {
    //plotPostShotNeutronData(shotNumber, shotNumber - 1, shotNumber - 2, 2,
    //			    "data/pinchData.txt", "data/pinchDataScript.sh");
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    plotPostShotMultiNeutronData(shotNumber, "data/neutronMultiData.txt", 
				 "data/neutronMultiDataScript.sh");
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    //plotPostShotGVCurrent(shotNumber, "data/igvData.txt", "data/igvDataScript.sh");
    exit(0);
  }

  if (0) {
    int timeCompI, timeCompF, timeAccelI, timeAccelF;
    plotOffAxisDisplacement(shotNumber);
    plotPostShotIFData(shotNumber, timeCompI, timeCompF, "");
    plotPostShotNeutronData(shotNumber, shotNumber - 1, shotNumber - 2, 5,
			    "data/neutronData.txt", "data/neutronDataScript.sh");
    plotPostShotSymmetryCheck(shotNumber, timeAccelI, timeAccelF);
    plotPostShotIV(shotNumber, shotNumber - 1, shotNumber - 2, 
		   "data/ivData.txt", "data/ivDataScript.sh");
    plotPostShotAccelData(shotNumber, "data/accelData.txt", "data/accelScript.sh");
    plotPostShotAccelDataN95(shotNumber, timeAccelI, timeAccelF);
    plotPostShotGVCurrent(shotNumber, "data/igvData.txt", "data/igvDataScript.sh");
    plotPostShotCompData(shotNumber, timeCompI, timeCompF, "");
    plotPostShotIPData(shotNumber, shotNumber - 1, shotNumber - 2,
		       "data/ipData.txt", "data/ipDataScript.sh");
    plotPostShotModeData(shotNumber, shotNumber - 1, shotNumber - 2, 
			 "data/modeData.txt", "data/modeDataScript.sh");
    plotPostShotIPinchData(shotNumber, shotNumber - 1, shotNumber - 2,
    			   "data/ipData.txt", "data/ipDataScript.sh");

  }

  return 0;

}


/******************************************************************************
 * Function: plotPostShotIPData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

static int plotPostShotIPData(int shotNumber1, int shotNumber2, int shotNumber3, 
			      char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber1, "\\I_P", "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *ip1 = readDHIMDSplusVector(shotNumber1, "\\I_P", "fuze", "10.10.10.240");
  gsl_vector_scale(ip1, 1E-3);
  tempString = "with line lw 3 lc rgb 'black' title 'IP for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber1);
  char *ip1Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip1Label, sizeBuf+1, tempString, shotNumber1);

  gsl_vector *ip2 = readDHIMDSplusVector(shotNumber2, "\\I_P", "fuze", "10.10.10.240");
  gsl_vector_scale(ip2, 1E-3);
  tempString = "with line lw 3 lc rgb 'red' title 'IP for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber2);
  char *ip2Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip2Label, sizeBuf+1, tempString, shotNumber2);

  gsl_vector *ip3 = readDHIMDSplusVector(shotNumber3, "\\I_P", "fuze", "10.10.10.240");
  gsl_vector_scale(ip3, 1E-3);
  tempString = "with line lw 3 lc rgb 'blue' title 'IP for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber3);
  char *ip3Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip3Label, sizeBuf+1, tempString, shotNumber3);

  char *keyWords = "set title 'I_{P}'\n"
    "set xrange[0:400]\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange[0:]";
  
  plot3VectorData(time, ip1, ip1Label, ip2, ip2Label, ip3, ip3Label, 
		  keyWords, tempDataFile, tempScriptFile);

  free(ip1Label);
  free(ip2Label);
  free(ip3Label);
  gsl_vector_free(time);
  gsl_vector_free(ip1);
  gsl_vector_free(ip2);
  gsl_vector_free(ip3);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotIPinchData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

static int plotPostShotIPinchData(int shotNumber1, int shotNumber2, int shotNumber3, 
				  char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber1, "\\m_0_p15", "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *ip1 = readDHIMDSplusVector(shotNumber1, "\\m_0_p15", "fuze", "10.10.10.240");
  gsl_vector_scale(ip1, 500);
  tempString = "with line lw 3 lc rgb 'black' title 'I_{pinch} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber1);
  char *ip1Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip1Label, sizeBuf+1, tempString, shotNumber1);

  gsl_vector *ip2 = readDHIMDSplusVector(shotNumber2, "\\m_0_p15", "fuze", "10.10.10.240");
  gsl_vector_scale(ip2, 500);
  tempString = "with line lw 3 lc rgb 'red' title 'I_{pinch} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber2);
  char *ip2Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip2Label, sizeBuf+1, tempString, shotNumber2);

  gsl_vector *ip3 = readDHIMDSplusVector(shotNumber3, "\\m_0_p15", "fuze", "10.10.10.240");
  gsl_vector_scale(ip3, 500);
  tempString = "with line lw 3 lc rgb 'blue' title 'I_{pinch} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber3);
  char *ip3Label = (char *)malloc(sizeBuf + 1);
  snprintf(ip3Label, sizeBuf+1, tempString, shotNumber3);

  char *keyWords = "set title 'I_{pinch}'\n"
    "set xrange[0:400]\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange[0:]";
  
  plot3VectorData(time, ip1, ip1Label, ip2, ip2Label, ip3, ip3Label, 
		  keyWords, tempDataFile, tempScriptFile);

  free(ip1Label);
  free(ip2Label);
  free(ip3Label);
  gsl_vector_free(time);
  gsl_vector_free(ip1);
  gsl_vector_free(ip2);
  gsl_vector_free(ip3);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotModeData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

static int plotPostShotModeData(int shotNumber1, int shotNumber2, int shotNumber3, 
				char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber1, "\\M_1_P15", "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *mode1 = readDHIMDSplusVector(shotNumber1, "\\M_1_P15", "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'black' title 'm=1 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber1);
  char *mode1Label = (char *)malloc(sizeBuf + 1);
  snprintf(mode1Label, sizeBuf+1, tempString, shotNumber1);

  gsl_vector *mode2 = readDHIMDSplusVector(shotNumber2, "\\M_1_P15", "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'red' title 'm=1 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber2);
  char *mode2Label = (char *)malloc(sizeBuf + 1);
  snprintf(mode2Label, sizeBuf+1, tempString, shotNumber2);

  gsl_vector *mode3 = readDHIMDSplusVector(shotNumber3, "\\M_1_P15", "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'blue' title 'm=1 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber3);
  char *mode3Label = (char *)malloc(sizeBuf + 1);
  snprintf(mode3Label, sizeBuf+1, tempString, shotNumber3);

  char *keyWords = "set title 'Normalized m=1 data'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Normalized Mode Amplitude'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange[0:]";
  
  plot3VectorData(time, mode1, mode1Label, mode2, mode2Label, mode3, mode3Label, 
		  keyWords, tempDataFile, tempScriptFile);

  free(mode1Label);
  free(mode2Label);
  free(mode3Label);
  gsl_vector_free(time);
  gsl_vector_free(mode1);
  gsl_vector_free(mode2);
  gsl_vector_free(mode3);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotIV
 * Inputs: int, int, int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * total plasma current and voltage across the hot and cold plate
 ******************************************************************************/

static int plotPostShotIV(int shotNumber1, int shotNumber2, int shotNumber3, 
			  char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber1, "\\v_gap", "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *vgap1 = readDHIMDSplusVector(shotNumber1, "\\v_gap", "fuze", "10.10.10.240");
  gsl_vector_scale(vgap1, 1E-3);
  tempString = "with line lw 3 lc rgb 'black' title 'V_{Gap} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber1);
  char *vgap1Label = (char *)malloc(sizeBuf + 1);
  snprintf(vgap1Label, sizeBuf+1, tempString, shotNumber1);

  gsl_vector *vgap2 = readDHIMDSplusVector(shotNumber2, "\\v_gap", "fuze", "10.10.10.240");
  gsl_vector_scale(vgap2, 1E-3);
  tempString = "with line lw 3 lc rgb 'red' title 'V_{Gap} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber2);
  char *vgap2Label = (char *)malloc(sizeBuf + 1);
  snprintf(vgap2Label, sizeBuf+1, tempString, shotNumber2);

  gsl_vector *vgap3 = readDHIMDSplusVector(shotNumber3, "\\v_gap", "fuze", "10.10.10.240");
  gsl_vector_scale(vgap3, 1E-3);
  tempString = "with line lw 3 lc rgb 'blue' title 'V_{Gap} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber3);
  char *vgap3Label = (char *)malloc(sizeBuf + 1);
  snprintf(vgap3Label, sizeBuf+1, tempString, shotNumber3);

  char *keyWords = "set title 'V_{Gap}'\n"
    "set xrange[-10:500]\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange [:]";

  plot3VectorData(time, vgap1, vgap1Label, vgap2, vgap2Label, vgap3, vgap3Label, 
		  keyWords, tempDataFile, tempScriptFile);
  
  free(vgap1Label);
  free(vgap2Label);
  free(vgap3Label);
  gsl_vector_free(time);
  gsl_vector_free(vgap1);
  gsl_vector_free(vgap2);
  gsl_vector_free(vgap3);
  
  return 0;

}


/******************************************************************************
 * Function: plotPostShotNeutronData
 * Inputs: int, int, int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * total plasma current and voltage across the hot and cold plate
 ******************************************************************************/

static int plotPostShotNeutronData(int shotNumber1, int shotNumber2, int shotNumber3, 
				   int detectorNum, char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;
  
  tempString = "\\neutron_%d_s";
  sizeBuf = snprintf(NULL, 0, tempString, detectorNum);
  char *neutronNode = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode, sizeBuf+1, tempString, detectorNum);

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber1, neutronNode, "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *neutron1 = readDHIMDSplusVector(shotNumber1, neutronNode, "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'black' title 'N_{D%d} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, detectorNum, shotNumber1);
  char *neutron1Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron1Label, sizeBuf+1, tempString, detectorNum, shotNumber1);

  gsl_vector *neutron2 = readDHIMDSplusVector(shotNumber2, neutronNode, "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'red' title 'N_{D%d} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, detectorNum, shotNumber2);
  char *neutron2Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron2Label, sizeBuf+1, tempString, detectorNum, shotNumber2);

  gsl_vector *neutron3 = readDHIMDSplusVector(shotNumber3, neutronNode, "fuze", "10.10.10.240");
  tempString = "with line lw 3 lc rgb 'blue' title 'N_{D%d} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, detectorNum, shotNumber3);
  char *neutron3Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron3Label, sizeBuf+1, tempString, detectorNum, shotNumber3);

  tempString = "set title 'N_{D%d}'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange [:]";
  sizeBuf = snprintf(NULL, 0, tempString, detectorNum);
  char *keyWords = (char *)malloc(sizeBuf + 1);
  snprintf(keyWords, sizeBuf+1, tempString, detectorNum);


  plot3VectorData(time, neutron1, neutron1Label, neutron2, neutron2Label, neutron3, neutron3Label, 
		  keyWords, tempDataFile, tempScriptFile);
  
  free(neutron1Label);
  free(neutron2Label);
  free(neutron3Label);
  free(neutronNode);
  free(keyWords);
  gsl_vector_free(time);
  gsl_vector_free(neutron1);
  gsl_vector_free(neutron2);
  gsl_vector_free(neutron3);
  
  return 0;

}


/******************************************************************************
 * Function: plotPostShotNeutronData
 * Inputs: int, int, int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * total plasma current and voltage across the hot and cold plate
 ******************************************************************************/

static int plotPostShotMultiNeutronData(int shotNumber, char *tempDataFile, 
					char *tempScriptFile) {

  size_t sizeBuf;
  char *tempStringNode,
    *tempStringTitle;
  int detectorNum;
 
  detectorNum = 1;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode1 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode1, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time1 = readDHIMDSplusVectorDim(shotNumber, neutronNode1, "fuze", "10.10.10.240");
  gsl_vector_scale(time1, 1E6);
  gsl_vector *neutron1 = readDHIMDSplusVector(shotNumber, neutronNode1, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron1Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron1Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  detectorNum = 2;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode2 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode2, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time2 = readDHIMDSplusVectorDim(shotNumber, neutronNode2, "fuze", "10.10.10.240");
  gsl_vector_scale(time2, 1E6);
  gsl_vector *neutron2 = readDHIMDSplusVector(shotNumber, neutronNode2, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron2Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron2Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  detectorNum = 2;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode3 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode3, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time3 = readDHIMDSplusVectorDim(shotNumber, neutronNode3, "fuze", "10.10.10.240");
  gsl_vector_scale(time3, 1E6);
  gsl_vector *neutron3 = readDHIMDSplusVector(shotNumber, neutronNode3, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron3Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron3Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  detectorNum = 4;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode4 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode4, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time4 = readDHIMDSplusVectorDim(shotNumber, neutronNode4, "fuze", "10.10.10.240");
  gsl_vector_scale(time4, 1E6);
  gsl_vector *neutron4 = readDHIMDSplusVector(shotNumber, neutronNode4, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron4Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron4Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  detectorNum = 5;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode5 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode5, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time5 = readDHIMDSplusVectorDim(shotNumber, neutronNode5, "fuze", "10.10.10.240");
  gsl_vector_scale(time5, 1E6);
  gsl_vector *neutron5 = readDHIMDSplusVector(shotNumber, neutronNode5, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron5Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron5Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  detectorNum = 6;
  tempStringNode = "\\neutron_%d";
  sizeBuf = snprintf(NULL, 0, tempStringNode, detectorNum);
  char *neutronNode6 = (char *)malloc(sizeBuf + 1);
  snprintf(neutronNode6, sizeBuf+1, tempStringNode, detectorNum);
  gsl_vector *time6 = readDHIMDSplusVectorDim(shotNumber, neutronNode6, "fuze", "10.10.10.240");
  gsl_vector_scale(time6, 1E6);
  gsl_vector *neutron6 = readDHIMDSplusVector(shotNumber, neutronNode6, "fuze", "10.10.10.240");
  tempStringTitle = "with line lw 3 lc rgb 'black' title 'N_{D%d}'";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *neutron6Label = (char *)malloc(sizeBuf + 1);
  snprintf(neutron6Label, sizeBuf+1, tempStringTitle, detectorNum, shotNumber);

  tempStringTitle = "set title 'N_{D}'\n"
    "set xrange[0:100]\n"
    "set ylabel 'Voltage (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange [:]";
  sizeBuf = snprintf(NULL, 0, tempStringTitle, detectorNum);
  char *keyWords = (char *)malloc(sizeBuf + 1);
  snprintf(keyWords, sizeBuf+1, tempStringTitle, detectorNum);

  plot6PlotsVectorData(time1, neutron1, neutron1Label, time2, neutron2, neutron2Label,
		       time3, neutron3, neutron3Label, time4, neutron4, neutron4Label,
		       time5, neutron5, neutron5Label, time6, neutron6, neutron6Label,
		       keyWords, tempDataFile, tempScriptFile);
  
  free(neutron1Label);
  free(neutron2Label);
  free(neutron3Label);
  free(neutron4Label);
  free(neutron5Label);
  free(neutron6Label);
  free(neutronNode1);
  free(neutronNode2);
  free(neutronNode3);
  free(neutronNode4);
  free(neutronNode5);
  free(neutronNode6);
  free(keyWords);
  gsl_vector_free(time1);
  gsl_vector_free(time2);
  gsl_vector_free(time3);
  gsl_vector_free(time4);
  gsl_vector_free(time5);
  gsl_vector_free(time6);
  gsl_vector_free(neutron1);
  gsl_vector_free(neutron2);
  gsl_vector_free(neutron3);
  gsl_vector_free(neutron4);
  gsl_vector_free(neutron5);
  gsl_vector_free(neutron6);
  
  return 0;

}


/******************************************************************************
 * Function: plotPostShotGVCurrent
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotGVCurrent(int shotNumber, char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber, "\\i_gv_1_valve", 
					     "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *igv1 = readDHIMDSplusVector(shotNumber, "\\i_gv_1_valve", "fuze", "10.10.10.240");
  gsl_vector_scale(igv1, 1E-3);
  tempString = "with line lw 3 lc rgb 'black' title 'I_{GV1} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *igv1Label = (char *)malloc(sizeBuf + 1);
  snprintf(igv1Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *igv2 = readDHIMDSplusVector(shotNumber, "\\i_gv_2_valve", "fuze", "10.10.10.240");
  gsl_vector_scale(igv2, 1E-3);
  tempString = "with line lw 3 lc rgb 'red' title 'I_{GV2} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *igv2Label = (char *)malloc(sizeBuf + 1);
  snprintf(igv2Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *igv2Dummy = readDHIMDSplusVector(shotNumber, "\\i_gv_2_dummy_load", 
					       "fuze", "10.10.10.240");
  gsl_vector_scale(igv2Dummy, 1E-3);
  tempString = "with line lw 3 lc rgb 'blue' title 'I_{GV2D} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *igv2DummyLabel = (char *)malloc(sizeBuf + 1);
  snprintf(igv2DummyLabel, sizeBuf+1, tempString, shotNumber);

  gsl_vector *igv3 = readDHIMDSplusVector(shotNumber, "\\i_gv_3_valve", "fuze", "10.10.10.240");
  gsl_vector_scale(igv3, 1E-3);
  tempString = "with line lw 3 lc rgb 'green' title 'I_{GV3} for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *igv3Label = (char *)malloc(sizeBuf + 1);
  snprintf(igv3Label, sizeBuf+1, tempString, shotNumber);


  char *keyWords = "set title 'I_{GV}'\n"
    "set xrange[-1500:500]\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange [:]";

  plot4VectorData(time, igv1, igv1Label, igv2, igv2Label, igv2Dummy, igv2DummyLabel, 
		  igv3, igv3Label, keyWords, tempDataFile, tempScriptFile);
  
  free(igv1Label);
  free(igv2Label);
  free(igv2DummyLabel);
  gsl_vector_free(time);
  gsl_vector_free(igv1);
  gsl_vector_free(igv2);
  gsl_vector_free(igv2Dummy);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotAccelData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotAccelData(int shotNumber, char *tempDataFile, char *tempScriptFile) {

  size_t sizeBuf;
  char *tempString;

  gsl_vector *time = readDHIMDSplusVectorDim(shotNumber, "\\b_n45_180_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(time, 1E6);

  gsl_vector *data1 = readDHIMDSplusVector(shotNumber, "\\b_n45_180_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(data1, 1E-3);
  tempString = "with line lw 3 lc rgb 'black' title 'n45 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *data1Label = (char *)malloc(sizeBuf + 1);
  snprintf(data1Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *data2 = readDHIMDSplusVector(shotNumber, "\\b_n35_000_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(data2, 1E-3);
  tempString = "with line lw 3 lc rgb 'red' title 'n35 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *data2Label = (char *)malloc(sizeBuf + 1);
  snprintf(data2Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *data3 = readDHIMDSplusVector(shotNumber, "\\b_n25_000_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(data3, 1E-3);
  tempString = "with line lw 3 lc rgb 'blue' title 'n25 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *data3Label = (char *)malloc(sizeBuf + 1);
  snprintf(data3Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *data4 = readDHIMDSplusVector(shotNumber, "\\b_n15_000_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(data4, 1E-3);
  tempString = "with line lw 3 lc rgb 'green' title 'n15 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *data4Label = (char *)malloc(sizeBuf + 1);
  snprintf(data4Label, sizeBuf+1, tempString, shotNumber);

  gsl_vector *data5 = readDHIMDSplusVector(shotNumber, "\\b_n05_000_sm", "fuze", "10.10.10.240");
  gsl_vector_scale(data5, 1E-3);
  tempString = "with line lw 3 lc rgb 'yellow' title 'n05 for %d'";
  sizeBuf = snprintf(NULL, 0, tempString, shotNumber);
  char *data5Label = (char *)malloc(sizeBuf + 1);
  snprintf(data5Label, sizeBuf+1, tempString, shotNumber);

  char *keyWords = "set title 'Acceleration Region\n"
    "set xrange[-800:0]\n"
    "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n"
    "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n"
    "set grid\n"
    "set xrange[0:100]\n"
    "set key left top\n"
    "set yrange [:]";

  plot5VectorData(time, data1, data1Label, data2, data2Label, data3, data3Label, data4, 
		  data4Label, data5, data5Label, keyWords, tempDataFile, tempScriptFile);

  free(data1Label);
  free(data2Label);
  free(data3Label);
  free(data4Label);
  free(data5Label);

  gsl_vector_free(time);
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotSymmetryCheck
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotSymmetryCheck(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\b_n40_180_sm",
    *data1Name = "N40-180",
    *data2Node = "\\b_n40_000_sm",
    *data2Name = "N40-0",
    *data3Node = "\\b_n30_180_sm",
    *data3Name = "N30-180",
    *data4Node = "\\b_n30_000_sm",
    *data4Name = "N30-0",
    *data5Node = "\\b_n20_180_sm",
    *data5Name = "N20-180",
    *data6Node = "\\b_n20_000_sm",
    *data6Name = "N20-0";

  int status;

  char *gnuPlotFile = "script/tempSym.sh",
    *accelFile = "data/sym.txt";

  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *data3 = gsl_vector_alloc(sigSize),
    *data4 = gsl_vector_alloc(sigSize),
    *data5 = gsl_vector_alloc(sigSize),
    *data6 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);
  initializeMagneticData(shotNumber, data3Node, data3);
  initializeMagneticData(shotNumber, data4Node, data4);
  initializeMagneticData(shotNumber, data5Node, data5);
  initializeMagneticData(shotNumber, data6Node, data6);


  /* Saving data */
  save7VectorData(time, data1, data2, data3, data4, data5, data6, accelFile);


  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set xrange[%d:%d]\n", tmin, tmax);
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Axial Symmetry Check for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'black' \
title '%s',\\\n", accelFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 dt 2 lc rgb 'black' \
title '%s',\\\n", accelFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'red' \
title '%s',\\\n", accelFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 dt 2 lc rgb 'red' \
title '%s',\\\n", accelFile, data4Name);
  fprintf(fp, "     '%s' using ($1*1E6):($6) with line lw 3 lc rgb 'blue' \
title '%s',\\\n", accelFile, data5Name);
  fprintf(fp, "     '%s' using ($1*1E6):($7) with line lw 3 dt 2 lc rgb 'blue' \
title '%s'\n", accelFile, data6Name);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  remove(gnuPlotFile);
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotOffAxisDisplacement
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

static int plotOffAxisDisplacement(int shotNumber) {

  int status;

  char *nodeName = "\\b_p15_000_sm";

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";

  double dhiTime = getDHITime(shotNumber);

  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, nodeName);
  gsl_matrix *disp = getOffAxisDisplacement(azimuthArray);

  

  /* Saving data */
  saveMatrixData(disp, modeFile);


  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal png\n");
  //fprintf(fp, "set output '/home/fuze/Downloads/%d_Mode.png'\n", shotNumber);
  fprintf(fp, "set arrow from %g,graph 0 to %g,graph 1 nohead dt 4 lw 3 lc rgb 'orange'\n", 
	  dhiTime*1E6, dhiTime*1E6);
  fprintf(fp, "set label ' DHI trigger time' at %g,graph 0.95 font 'Times Bold, 12'\n", 
	  dhiTime*1E6);
  fprintf(fp, "set xrange[%d:%d]\n", (int) (dhiTime*1E6-5), (int) (dhiTime*1E6+5));
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Displacement at z = 15 cm for %d ({/Symbol D}r=m1/m0*R_{W}/2)' \
font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Displacement (cm)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'red' \
title 'x',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'blue' \
title 'y'\n", modeFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

 
  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);
  
  return 0;

}


/******************************************************************************
 * Function: invertFlatTopProfile
 * Inputs: int
 * Returns: int
 * Description: This will create a synthetic flat top profile, and invert it.
 ******************************************************************************/

int invertFlatTopProfile() {

  // Diameter = 1.65 cm
  // Line integrated density = 2.2E17 cmE-2

  int numRows = 300,
    numCols = 300,
    shotNumber = 180516014;

  double val;

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.numRows = numRows;
  param.numCols = numCols;
  param.deltaY = 0.00803917;
  
  gsl_matrix *densityProfile = gsl_matrix_alloc(numRows, numCols);
  
  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      
      if ( (ii < 5) || (ii > 294)) {
	val = 0.0;
      } else if ( (ii < 80) ) {
	val = 2.2E17*(1-gsl_pow_2((ii-80)/75.0));
      } else if ( (ii > 220) ) {
	val = 2.2E17*(1-gsl_pow_2((ii-220)/75.0));
      } else if ( ii == 150) {
	val = 2.2E17*1.01;
      } else {
	val = 2.2E17;
      }
    
      gsl_matrix_set(densityProfile, ii, jj, val);

    }
  }  
  
  saveMatrixData(densityProfile, param.fileLineIntText);
  saveImageData(densityProfile, param.fileLineInt);

  param.deltaN = 0.02;
  param.centroidNum = 10;
  param.offsetIter = 10;

  gsl_matrix *invertedImage = invertImageDHI(densityProfile, &param);
  saveImageData(invertedImage, param.fileFullInvert);

  gsl_matrix *mSave = readMatrixTextFile(param.fileLeftInvert);

  gsl_vector* xSave = gsl_vector_alloc(numRows);
  gsl_vector* ySave = gsl_vector_alloc(numRows);

  for (ii = 0; ii < numRows; ii++) {
    gsl_vector_set(xSave, ii, ii*param.deltaY);
    gsl_vector_set(ySave, ii, gsl_matrix_get(mSave, ii, 145));
  }

  save2VectorData(xSave, ySave, "data/fitRadialProfile180516014.txt");


  //int colPlot = 150;
  //plotMatrixColDataFile(param.fileLeftInvert, colPlot,"set title 'Left Inverted'\n");
  //plotMatrixColDataFile(param.fileRightInvert, colPlot, "set title 'Right Inverted'\n");


  /* Creating gnuplot file */

  char *gnuPlotFile = "data/gnuplot.sh";
  int status;

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/fitvsData.png'\n");
  //fprintf(fp, "set xrange[0:15]\n");
  fprintf(fp, "set yrange[0:1.4E17]\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Inverted fit data from pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'n_{e} (cm^{-3})' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1):($2) with points ls 2 title 'inverted data'\n", 
	  "data/fitRadialProfile180516014.txt");
  //fprintf(fp, "plot '%s' using ($0*%g):($%d) with points ls 2 title 'inverted data'\n", 
  //	  param.fileLeftInvert, param.deltaY, 145);
  fprintf(fp, "pause -1\n");

/*   fprintf(fp, "#!/usr/bin/env gnuplot\n"); */
/*   fprintf(fp, "set terminal pngcairo\n"); */
/*   fprintf(fp, "set output 'data/fitvsData.png'\n"); */
/*   //fprintf(fp, "set xrange[0:15]\n"); */
/*   fprintf(fp, "set key right top\n"); */
/*   fprintf(fp, "set grid\n"); */
/*   fprintf(fp, "set title 'Data vs. fit data for Pulse #%d' font '0,18'\n", shotNumber); */
/*   fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n"); */
/*   fprintf(fp, "set ylabel 'Line integrated n_{e} (cm^{-2})' font ',16' offset 0,0\n"); */
/*   fprintf(fp, "plot '%s' using ($0*%g):($%d) with line lw 3 lc rgb 'black' \ */
/* title 'fit data',\\\n", param.fileLineIntText, param.deltaY, colPlot); */
/*   fprintf(fp, "     '%s' using ($0*%g):($%d) with points ls 1 \ */
/* title 'data'\n", "data/lineIntegrated180516014.txt", param.deltaY, 45); */
/*   fprintf(fp, "pause -1\n"); */
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }



  //plotImageDataFile(param.fileFullInvert, 1, 1, 1, "set cbrange [0:1.2]\n",
  //                  "data/invertFlatTopScript.sh");

  return 0;


}


/******************************************************************************
 * Function: plotPostShotIFData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotIFData(int shotNumber, int tmin, int tmax, char *saveFile) {

  char *data1Node = "\\ne_1",
    *data1Name = "NE #1",
    *data2Node = "\\ne_2",
    *data2Name = "NE #2",
    *data3Node = "\\ne_3",
    *data3Name = "NE #3",
    *data4Node = "\\ne_4",
    *data4Name = "NE #4";

  int status;

  char *gnuPlotFile = "script/tempNePost.sh",
    *neFile = "data/nePost.txt";

  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *data3 = gsl_vector_alloc(sigSize),
    *data4 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);
  initializeMagneticData(shotNumber, data3Node, data3);
  initializeMagneticData(shotNumber, data4Node, data4);


  /* Saving data */
  save5VectorData(time, data1, data2, data3, data4, neFile);


  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  if (strcmp(saveFile, "") != 0) {
    fprintf(fp, "set terminal png\n");
    fprintf(fp, "set output '%s'\n", saveFile);
  }
  fprintf(fp, "set xrange[%d:%d]\n", tmin, tmax);
  fprintf(fp, "set key left bottom\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'IF data for pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'Voltage (V)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'black' \
title '%s',\\\n", neFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'red' \
title '%s',\\\n", neFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'blue' \
title '%s',\\\n", neFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'yellow' \
title '%s'\n", neFile, data4Name);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }


  remove(gnuPlotFile);
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotCompData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotCompData(int shotNumber, int tmin, int tmax, char *saveFile) {

  char *data1Node = "\\b_p5_180_sm",
    *data1Name = "p5",
    *data2Node = "\\b_p15_180_sm",
    *data2Name = "p15",
    *data3Node = "\\b_p25_180_sm",
    *data3Name = "p25",
    *data4Node = "\\b_p35_180_sm",
    *data4Name = "p35",
    *data5Node = "\\b_p45_180_sm",
    *data5Name = "p45";

  int status;

  char *gnuPlotFile = "script/tempComp.sh",
    *compFile = "data/comp.txt";

  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *data3 = gsl_vector_alloc(sigSize),
    *data4 = gsl_vector_alloc(sigSize),
    *data5 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);
  initializeMagneticData(shotNumber, data3Node, data3);
  initializeMagneticData(shotNumber, data4Node, data4);
  initializeMagneticData(shotNumber, data5Node, data5);


  /* Saving data */
  save6VectorData(time, data1, data2, data3, data4, data5, compFile);


  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  if (strcmp(saveFile, "") != 0) {
    fprintf(fp, "set terminal png\n");
    fprintf(fp, "set output '%s'\n", saveFile);
  }
  fprintf(fp, "set xrange[%d:%d]\n", tmin, tmax);
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Compression Region for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'I (kA)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2/0.002) with line lw 3 lc rgb 'black' \
title '%s',\\\n", compFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3/0.002) with line lw 3 lc rgb 'red' \
title '%s',\\\n", compFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4/0.002) with line lw 3 lc rgb 'blue' \
title '%s',\\\n", compFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5/0.002) with line lw 3 lc rgb 'green' \
title '%s',\\\n", compFile, data4Name);
  fprintf(fp, "     '%s' using ($1*1E6):($6/0.002) with line lw 3 lc rgb 'yellow' \
title '%s'\n", compFile, data5Name);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }


  remove(gnuPlotFile);

  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotAccelDataN95
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotPostShotAccelDataN95(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\b_n95_180_sm",
    *data1Name = "n95",
    *data2Node = "\\b_n85_000_sm",
    *data2Name = "n85",
    *data3Node = "\\b_n75_000_sm",
    *data3Name = "n75",
    *data4Node = "\\b_n16_000_sm",
    *data4Name = "n65",
    *data5Node = "\\b_n55_000_sm",
    *data5Name = "n55";

  int status;

  char *gnuPlotFile = "script/tempAccelN95.sh",
    *accelFile = "data/accelN95.txt";

  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *data3 = gsl_vector_alloc(sigSize),
    *data4 = gsl_vector_alloc(sigSize),
    *data5 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);
  initializeMagneticData(shotNumber, data3Node, data3);
  initializeMagneticData(shotNumber, data4Node, data4);
  initializeMagneticData(shotNumber, data5Node, data5);


  /* Saving data */
  save6VectorData(time, data1, data2, data3, data4, data5, accelFile);


  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set xrange[%d:%d]\n", tmin, tmax);
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'N95 Acceleration Region for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'black' \
title '%s',\\\n", accelFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'red' \
title '%s',\\\n", accelFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'blue' \
title '%s',\\\n", accelFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'green' \
title '%s',\\\n", accelFile, data4Name);
  fprintf(fp, "     '%s' using ($1*1E6):($6) with line lw 3 lc rgb 'yellow' \
title '%s'\n", accelFile, data5Name);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }


  remove(gnuPlotFile);

  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);
  gsl_vector_free(time);

  return 0;

}
