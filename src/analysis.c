#include "analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

/******************************************************************************
 * Function: hologramAnalysis
 * Inputs: 
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int hologramAnalysis() {

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  hologramMain(&param);

  //plotImageDataFile(param.fileHologram, "set size ratio -1");
  plotImageDataFile(param.fileLineIntPos, "set size ratio -1");

  //plotMatrixColVColErrorDataFile(param.fileError, 0, 1+60*2, 1+60*2+1, "");

  //plotMatrixColVColDataFile(param.fileLeftInvert, 0, 60, "");
  //plotMatrixColDataFile(param.fileLineIntText, 10, "");

  //plotImageDataFile(param.fileHologram, 
  //	    "set size ratio -1\nset term png\nset output 'data/temp.png'");
  //plot2MatrixColDataFile(param.fileRightInvert, 60, 
  //			 param.fileLeftInvert, 60, "");

  //plotImageDataFile(param.fileFullInvert, "set size ratio -1\n");

  if (0) {

    char *keywords = "set terminal png\nset size ratio -1\n"
      "set output '/home/webertr/Downloads/180517033.png'\n"
      "set title 'Pulse 180517033\n"
      "set xrange [13.47:14.52]\n"
      "set yrange [-0.9:0.9]\n"
      "set xlabel 'z (cm)'\n"
      "set ylabel 'b (cm)'\n"
      "set label front 'Line integrated n_{e} (cm^{-2})' at graph 1.60,0.20 "
      "rotate by 90 font 'Times Bold, 14'\n";
    plotImageDataFile(param.fileLineIntPos, keywords);

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

  plotPostShotIV(shotNumber, -100, 800);
  //plotPostShotGVCurrent(shotNumber, -800, 0);
  //plotOffAxisDisplacement(shotNumber);

  int pid1 = fork();
  int pid2 = fork();

  if ( (pid1 == 0) && (pid2==0) ) {
    plotPostShotModeData(shotNumber, 30, 75, "\\b_p15_000_sm");
  }
  else if ( (pid1 == 0) && (pid2>0) ) {
    plotPostShotNeutronData(shotNumber, 30, 75);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2==0) ) {
    plotPostShotSymmetryCheck(shotNumber, 0, 75);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2>0) ) {
    plotPostShotAccelData(shotNumber, 0, 75);
    exit(0);
  }
  

  return 0;

}


/******************************************************************************
 * Function: plotPostShotModeData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

int plotPostShotModeData(int shotNumber, int tmin, int tmax, char *nodeName) {

  int status;

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";

  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, nodeName);
  getAzimuthalArrayModes(azimuthArray);
  double dhiTime = getDHITime(shotNumber);
  

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);


  
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
  //fprintf(fp, "set output '/home/webertr/Downloads/%d_Mode.png'\n", shotNumber);
  fprintf(fp, "set arrow from %g,0 to %g,1 nohead dt 4 lw 3 lc rgb 'orange'\n", 
	  dhiTime*1E6, dhiTime*1E6);
  fprintf(fp, "set label 'DHI trigger time' at %g,0.5 rotate by 90 font 'Times Bold, 12'\n", 
	  dhiTime*1E6+1.0);
  fprintf(fp, "set xrange[%d:%d]\n", tmin, tmax);
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set y2range[0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z = 15 cm for %d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "set y2label 'Pinch Current (kA)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'blue' \
title 'm=2',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'green' \
title 'm=3',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($2/0.002) with line lw 3 lc rgb 'black' \
title 'Pinch Current' axes x1y2\n", modeFile);
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

  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);
  
  return 0;

}


/******************************************************************************
 * Function: plotPostShotIV
 * Inputs: int, int, int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * total plasma current and voltage across the hot and cold plate
 ******************************************************************************/

int plotPostShotIV(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\v_gap",
    *data1Name = "V_{GAP}",
    *data2Node = "\\i_p",
    *data2Name = "I_{P}";

  int status;

  char *gnuPlotFile = "script/tempIV.sh",
    *ivFile = "data/iv.txt";

  
  /* Getting data */
  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);
  

  /* Saving data */
  save3VectorData(time, data1, data2, ivFile);


  
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
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'I_{P} and V_{GAP} for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'I_{P} (kA)' font ',16' offset 0,0\n");
  fprintf(fp, "set y2label 'V_{GAP} (kV)' font 'Times Bold,16' offset 0,0\n");
  fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2*1E-3) with line lw 3 lc rgb 'red' \
title '%s' axes x1y2,\\\n", ivFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3*1E-3) with line lw 3 lc rgb 'black' \
title '%s'\n", ivFile, data2Name);

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

  /* Freeing vectors */
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(time);

  
  return 0;

}


/******************************************************************************
 * Function: plotPostShotGVCurrent
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotGVCurrent(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\i_gv_2_valve",
    *data1Name = "I GV2",
    *data2Node = "\\i_gv_2_dummy_load",
    *data2Name = "GV2 Dummy Load";

  int status;

  char *gnuPlotFile = "script/tempGVCurrent.sh",
    *gvFile = "data/gvCurrent.txt";

  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, data1Node, data1, time);
  initializeMagneticData(shotNumber, data2Node, data2);


  /* Saving data */
  save3VectorData(time, data1, data2, gvFile);


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
  fprintf(fp, "set title 'Gas Valve Current for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'I (Amps)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'black' \
title '%s',\\\n", gvFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'red' \
title '%s'\n", gvFile, data2Name);
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
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotPostShotAccelData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotAccelData(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\b_n45_180_sm",
    *data1Name = "n45",
    *data2Node = "\\b_n35_000_sm",
    *data2Name = "n35",
    *data3Node = "\\b_n25_000_sm",
    *data3Name = "n25",
    *data4Node = "\\b_n15_000_sm",
    *data4Name = "n15",
    *data5Node = "\\b_n05_000_sm",
    *data5Name = "n05";

  int status;

  char *gnuPlotFile = "script/tempAccel.sh",
    *accelFile = "data/accel.txt";

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
  fprintf(fp, "set title 'Acceleration Region for Pulse #%d' font '0,18'\n", shotNumber);
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


/******************************************************************************
 * Function: plotPostShotNeutronData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotNeutronData(int shotNumber, int tmin, int tmax) {

  char *data1Node = "\\neutron_1",
    *data1Name = "ND #1",
    *data2Node = "\\neutron_2",
    *data2Name = "ND #2",
    *data3Node = "\\neutron_5",
    *data3Name = "ND #5",
    *data4Node = "\\neutron_6",
    *data4Name = "ND #6",
    *data5Node = "\\neutron_7",
    *data5Name = "ND #7";

  int status;

  char *gnuPlotFile = "script/tempNeutron.sh",
    *accelFile = "data/neutron.txt";

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
  fprintf(fp, "set title 'Neutron Diagnostics for Pulse #%d' font '0,18'\n", shotNumber);
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


/******************************************************************************
 * Function: plotPostShotSymmetryCheck
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotSymmetryCheck(int shotNumber, int tmin, int tmax) {

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
 * Function: plotIPApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot IP for a talk that I'm giving
 * on April 28th, 2018
 ******************************************************************************/

int plotIPApril2018Talk(int shotNumber) {

  int sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);
  
  /* Declaring variables */
  gsl_vector *iP = gsl_vector_alloc(sigSize),
    *cap1 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize),
    *timeCAMAC = gsl_vector_alloc(sigSize);

  int status;
  
  char *gnuPlotFile = "script/temp.sh",
    *ipFile = "data/ip.txt",
    *cap1File = "data/cap1.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\I_P", iP, timeDetaq);
  initializeMagneticDataAndTime(shotNumber, "\\I_TBM_01_CAP", cap1, timeCAMAC);


  /* Saving data */
  save2VectorData(timeDetaq, iP, ipFile);
  save2VectorData(timeCAMAC, cap1, cap1File);



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
  fprintf(fp, "set xrange[-15E-6:600E-6]\n");
  fprintf(fp, "plot '%s' using 1:($2/1E3) title 'IP',\\\n", ipFile);
  fprintf(fp, "     '%s' using ($1-16E-6):(12*$2/1E3) title 'Cap 1'\n", cap1File);
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
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(iP);
  gsl_vector_free(cap1);
  gsl_vector_free(timeDetaq);
  gsl_vector_free(timeCAMAC);

  return 0;

}


/******************************************************************************
 * Function: plotAccelApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for acceleration data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotAccelApril2018Talk() {

  int shotNumber = 180223035,
    status,
    sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);

  /* Declaring variables */
  gsl_vector *n45 = gsl_vector_alloc(sigSize),
    *n35 = gsl_vector_alloc(sigSize),
    *n25 = gsl_vector_alloc(sigSize),
    *n15 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize);

  
  char *gnuPlotFile = "script/temp.sh",
    *accelFile = "data/accel.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\b_n45_000_sm", n45, timeDetaq);
  initializeMagneticData(shotNumber, "\\b_n35_000_sm", n35);
  initializeMagneticData(shotNumber, "\\b_n25_000_sm", n25);
  initializeMagneticData(shotNumber, "\\b_n15_000_sm", n15);


  /* Saving data */
  save5VectorData(timeDetaq, n45, n35, n25, n15, accelFile);


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
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/accel180223035'\n");
  fprintf(fp, "set xrange[0:15]\n");
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Acceleration Region for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+14E-6)*1E6):($2) with line lw 3 lc rgb 'black' \
title 'z = -45 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($3) with line lw 3 lc rgb 'red' \
title 'z = -35 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($4) with line lw 3 lc rgb 'blue' \
title 'z = -25 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($5) with line lw 3 lc rgb 'yellow' \
title 'z = -15 cm'\n", accelFile);
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
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(n45);
  gsl_vector_free(n35);
  gsl_vector_free(n25);
  gsl_vector_free(n15);
  gsl_vector_free(timeDetaq);

  return 0;

}


/******************************************************************************
 * Function: plotModeApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for mode data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotModeApril2018Talk() {

  int shotNumber = 180222040,
    status;

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";

  
  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000_sm");
  getAzimuthalArrayModes(azimuthArray);

  

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);


  
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
  fprintf(fp, "set terminal pngcairo\n");
  fprintf(fp, "set output 'data/modeData.png'\n");
  fprintf(fp, "set xrange[15:45]\n");
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set y2range[0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set arrow from 15,0.2 to 45,0.2 nohead dt 4 lw 3 lc rgb 'green'\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z=15 cm for pulse #%d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "set y2label 'Pinch Current (kA)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+15.2E-6)*1E6):($3) with line dt 2 lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($4) with line dt 3 lw 3 lc rgb 'blue' \
title 'm=2',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($2/0.002) with line lw 3 lc rgb 'black' \
title 'Pinch Current' axes x1y2\n", modeFile);
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
 * Function: plotDHIApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for DHI Data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotDHIApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";
  
  /* 
   * struct containing all the holography parameters.
   */
  //holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  /* Obtained line integrated data and do an abel inversion */
  //hologramMain(&param);

  
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
  fprintf(fp, "set palette rgb 33,13,10\n");
  fprintf(fp, "set terminal pngcairo size 19cm,25cm\n");
  fprintf(fp, "set output 'data/dhiData.png'\n");
  fprintf(fp, "set xrange[14.1:14.9]\n");
  fprintf(fp, "set yrange[-0.86:0.86]\n");
  fprintf(fp, "set label front 'Line integrated n_{e} (cm^{-2})' at graph 1.40,0.30 ");
  fprintf(fp, "rotate by 90 font 'Times Bold, 20'\n");
  fprintf(fp, "set size ratio -1\n");
  fprintf(fp, "set xtics 14.1, 0.2, 14.9\n");  
  fprintf(fp, "set title 'Pulse 180215012' font 'Times Bold, 20'\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Axial Position (cm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Impact Parameter (cm)' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/lineIntegratedPosition.dat' binary matrix with image title ''\n");
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
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotSpecCIIIImageApril2018Talk
 * Inputs: 
 * Returns: int
 * Description: This will use gnu plot to plot a ciii image for spectroscoy data
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotSpecCIIIImageApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";
  
  saveLFImageWithWavelength("/home/webertr/Spectroscopy/Data/180222/180222  036.spe",
			    "data/lightField.dat");
  
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
  fprintf(fp, "set palette rgb 33,13,10\n");
  //fprintf(fp, "set terminal pngcairo size 19cm,25cm\n");
  //fprintf(fp, "set output 'data/ciiiImage.png'\n");
  fprintf(fp, "set size ratio -1\n");
  fprintf(fp, "set xrange[229.65:229.8]\n");
  fprintf(fp, "set xtics 229.65, 0.05, 229.8\n");  
  fprintf(fp, "set yrange[0:1024]\n");
  fprintf(fp, "set cbrange [0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Wavelength (nm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Pixel' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "set title 'C^{+2} emission for Pulse 180222036' font 'Times Bold, 20'\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/lightField.dat' binary matrix with image title ''\n");
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
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotCIIILineApril2018Talk
 * Inputs: 
 * Returns: int
 * Description: This will use gnu plot to plot a ciii image for spectroscoy data
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotCIIILineApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";

  lightFieldParameters param = LIGHT_FIELD_PARAMETERS_DEFAULT;

  strcpy(param.speFile, "/home/webertr/Spectroscopy/Data/180222/180222  036.spe");
  
  gsl_matrix *image = 0;
  gsl_vector *waveLength = 0;
  
  getLightFieldData(&image, &waveLength, &param);

  gsl_vector *sumRow = gsl_vector_alloc(image->size1);

  int ii, jj;
  double val;
  for (ii = 0; ii < image->size1; ii++) {
    val = 0;
    for (jj = 792; jj <  811; jj++) {
      val = gsl_matrix_get(image, ii, jj)+val;
    }
    gsl_vector_set(sumRow, ii, val);
  }

  /* int center[20]={43,98,141,193,246,296,342,393,445,491,534,585,635,687,735,779,836,882,936,983}; */
  /* for (ii = 0; ii < sumRow->size; ii++) { */
  /*   //printf("Value (%d): %g\n", ii, gsl_vector_get(sumRow, ii)); */
  /*   // Centers: [43,98,141,193,246,296,342,393,445,491,534,585,635,687,735,779,836,882,936,983] */
  /*   // 586 looks like a good one */
  /*   // 837 is the other one I'm trying */
  /* } */
  
  int start = 788,
    stop = 810;

  int centerRow = 837;
  
  gsl_vector *ciiiLine = gsl_vector_alloc(stop - start);
  gsl_vector *ciiiWL = gsl_vector_alloc(stop -start);

  for (jj = 0; jj < ciiiLine->size; jj++) {
    gsl_vector_set(ciiiLine, jj, gsl_matrix_get(image, centerRow, jj+start));
    gsl_vector_set(ciiiWL, jj, gsl_vector_get(waveLength, jj+start));
  }

  double lineMax = gsl_vector_max(ciiiLine);
  for (jj = 0; jj < ciiiLine->size; jj++) {
    gsl_vector_set(ciiiLine, jj, gsl_vector_get(ciiiLine, jj)/lineMax);
  }

  
  double ampParam = 0.95,
    centerParam = 229.705,
    sigmaParam = 0.0184,
    offsetParam = 0.05;

  gsl_vector *gaussFit = gsl_vector_alloc(ciiiWL->size);
  
  fitGaussian(ciiiWL, ciiiLine, gaussFit, &ampParam, &centerParam, &sigmaParam, &offsetParam, 0);

  printf("Fit amplitude: %g\n", ampParam);
  printf("Fit center WL (nm): %g nm\n", centerParam);
  printf("Fit sigma WL (nm): %g nm\n", sigmaParam);
  printf("Fit offset: %g\n", offsetParam);
  printf("Fit ion Temperature: %g keV\n", carbonIonTemperature(centerParam, sigmaParam));
  
  FILE *fp1;
  fp1 = fopen("data/ciiiLine.txt", "w");
  for (jj = 0; jj < ciiiLine->size; jj++) {
    fprintf(fp1, "%g\t%g\t%g\n", gsl_vector_get(ciiiWL,jj),
	    gsl_vector_get(ciiiLine, jj),gsl_vector_get(gaussFit, jj));
  }
  fclose(fp1);
  
  
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
  fprintf(fp, "set palette rgb 33,13,10\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/ciiiLine.png'\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Wavelength (nm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Photon Counts (arb.)' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "set title 'C^{+2} emission for Pulse 180222036' font 'Times Bold, 20'\n");
  fprintf(fp, "set size ratio 0.75\n");
  fprintf(fp, "set yrange[0:1.1]\n");
  fprintf(fp, "set xrange[229.65:229.77]\n");
  fprintf(fp, "set xtics 229.65, 0.025, 229.8\n");
  fprintf(fp, "set yrange[0:1.1]\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/ciiiLine.txt' using 1:2 with points ps 2 pt 7 lc rgb 'black' ");
  fprintf(fp, "title '1s^{2}2p^{2} {/Symbol \256} 1s^{2}2s2p',\\\n");
  fprintf(fp, "     'data/ciiiLine.txt' using 1:3 with lines dt 2 lw 3 title 'Gaussian fit'\n");
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
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotModeCompApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for mode data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotModeCompApril2018Talk() {

  int shotNumber = 180222040,
    status,
    sigSize = getSignalLengthMDSplus("\\b_p15_000_sm", shotNumber);

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt",
    *mdsFile = "data/mdsplus.txt";


  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000_sm");
  getAzimuthalArrayModes(azimuthArray);
  gsl_vector *time = gsl_vector_alloc(sigSize);
  gsl_vector *data = gsl_vector_alloc(sigSize);
  initializeMagneticDataAndTime(shotNumber, "\\m_3_p15_norm_sm", data, time);

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);
  save2VectorData(time, data, mdsFile);

  
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
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/modeData.png'\n");
  fprintf(fp, "set xrange[15:45]\n");
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z=15 cm for pulse #%d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+15.2E-6)*1E6):($5) with line dt 2 lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($2) with line dt 3 lw 3 lc rgb 'blue' \
title 'm=1 mds'\n", mdsFile);
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
 * Function: plotOffAxisDisplacement
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

int plotOffAxisDisplacement(int shotNumber) {

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

  //plotImageData(densityProfile, "set title 'Line integrated data'\n");

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



  //plotImageDataFile(param.fileFullInvert, "set cbrange [0:1.2]\n");

  return 0;


}
