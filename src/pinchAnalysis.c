#include "pinchAnalysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotModeData(int shotNumber, int zVal, int tmin, int tmax,
			char *nodeName, char *saveFile, int uniqueID);
static int plotAccelData(int shotNumber, int tmin, int tmax, int uniqueID);
static int plotNeutronData(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID);
static int plotIV(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID);
static int plotAxialIz(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID);

/******************************************************************************
 * Function: pinchAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will run call some functions to analyze pinch data
 ******************************************************************************/

int pinchAnalysis() {

  //int shotNumber = 180723015;
  //int shotNumber = 180222035;
  int shotNumber = 180723010;
  
  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  int timeCompI = 30,
    timeCompF = 60;
  

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    //plotModeData(shotNumber, 5, timeCompI, timeCompF, "\\b_p05_000_sm", "", 1);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    plotModeData(shotNumber, 15, timeCompI, timeCompF, "\\b_p15_000_sm", "", 2);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    //plotModeData(shotNumber, 25, timeCompI, timeCompF, "\\b_p25_000_sm", "", 3);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    //plotModeData(shotNumber, 35, timeCompI, timeCompF, "\\b_p35_000_sm", "", 4);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    //plotModeData(shotNumber, 45, timeCompI, timeCompF, "\\b_p45_000_sm", "", 5);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {
    //plotAccelData(shotNumber, timeAccelI, timeAccelF, 1);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    plotNeutronData(shotNumber, timeCompI, timeCompF, "", 1);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    //plotIV(shotNumber, timeCompI, timeCompF, "/home/webertr/Downloads/180222035IV.png", 1);
    plotAxialIz(shotNumber, timeCompI, timeCompF, "", 1);
    exit(0);
  }


  return 0;

}


/******************************************************************************
 * Function: plotModeData
 * Inputs: int, int, int, char *, char *
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * magnetic mode data
 ******************************************************************************/

static int plotModeData(int shotNumber, int zVal, int tmin, int tmax,
			char *nodeName, char *saveFile, int uniqueID) {

  int status;
  
  int sizeString = snprintf(NULL, 0, "script/tempMode_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/tempMode_%d.sh", uniqueID);

  sizeString = snprintf(NULL, 0, "data/ipMode_%d.txt", uniqueID);
  char *ipFile = (char *)malloc(sizeString + 1);
  snprintf(ipFile, sizeString+1, "data/ipMode_%d.txt", uniqueID);

  sizeString = snprintf(NULL, 0, "data/mode_%d.txt", uniqueID);
  char *modeFile = (char *)malloc(sizeString + 1);
  snprintf(modeFile, sizeString+1, "data/mode_%d.txt", uniqueID);

  char *ipNode = "\\i_p";

  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, nodeName);
  getAzimuthalArrayModes(azimuthArray);

  int sigSize = getSignalLengthMDSplus(ipNode, shotNumber);
  
  gsl_vector *ip = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, ipNode, ip, time);


  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);
  save2VectorData(time, ip, ipFile);

  
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
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set y2range[0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z = %d cm for %d' font '0,14'\n", zVal, shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "set y2label 'Pinch Current (kA)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'blue' title 'm=1',\\\n",
	  modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'green' title 'm=2',\\\n",
	  modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'yellow' title 'm=3',\\\n",
	  modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($2*1E-3) with line lw 3 dt 2 lc rgb 'black' \
title 'I_{P}' axes x1y2,\\\n", ipFile);
  fprintf(fp, "     '%s' using ($1*1E6):($2/0.002) with line lw 3 dt 2 lc rgb 'red' \
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
  remove(ipFile);
  remove(modeFile);

  free(gnuPlotFile);
  free(ipFile);
  free(modeFile);
  
  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);
  
  return 0;

}


/******************************************************************************
 * Function: plotAccelData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotAccelData(int shotNumber, int tmin, int tmax, int uniqueID) {

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

  int sizeString = snprintf(NULL, 0, "script/tempAccel_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/tempAccel_%d.sh", uniqueID);

  sizeString = snprintf(NULL, 0, "data/accel_%d.txt", uniqueID);
  char *accelFile = (char *)malloc(sizeString + 1);
  snprintf(accelFile, sizeString+1, "data/accel_%d.txt", uniqueID);

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
  remove(accelFile);

  free(gnuPlotFile);
  free(accelFile);

  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotNeutronData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

static int plotNeutronData(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID) {

  char *data1Node = "\\neutron_1",
    *data1Name = "ND #1",
    *data2Node = "\\neutron_2",
    *data2Name = "ND #2",
    *data3Node = "\\neutron_4",
    *data3Name = "ND #4",
    *data4Node = "\\neutron_5",
    *data4Name = "ND #5",
    *data5Node = "\\neutron_6",
    *data5Name = "ND #6",
    *data6Node = "\\neutron_7",
    *data6Name = "ND #7";

  int status;

  int sizeString = snprintf(NULL, 0, "script/tempNeutron_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/tempNeutron_%d.sh", uniqueID);

  sizeString = snprintf(NULL, 0, "data/neutron_%d.txt", uniqueID);
  char *neutronFile = (char *)malloc(sizeString + 1);
  snprintf(neutronFile, sizeString+1, "data/neutron_%d.txt", uniqueID);

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
  save7VectorData(time, data1, data2, data3, data4, data5, data6, neutronFile);


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
  fprintf(fp, "set title 'Neutron Diagnostics for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'Voltage (V)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2) with line lw 3 lc rgb 'black' \
title '%s',\\\n", neutronFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'red' \
title '%s',\\\n", neutronFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'blue' \
title '%s',\\\n", neutronFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'green' \
title '%s',\\\n", neutronFile, data4Name);
  fprintf(fp, "     '%s' using ($1*1E6):($6) with line lw 3 lc rgb 'orange' \
title '%s',\\\n", neutronFile, data5Name);
  fprintf(fp, "     '%s' using ($1*1E6):($7) with line lw 3 lc rgb 'yellow' \
title '%s'\n", neutronFile, data6Name);
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
  remove(neutronFile);

  free(gnuPlotFile);
  free(neutronFile);
  
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(data3);
  gsl_vector_free(data4);
  gsl_vector_free(data5);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotIV
 * Inputs: int, int, int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output the
 * total plasma current and voltage across the hot and cold plate
 ******************************************************************************/

static int plotIV(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID) {

  char *data1Node = "\\v_gap",
    *data1Name = "V_{GAP}",
    *data2Node = "\\i_p",
    *data2Name = "I_{P}";

  int status;

  int sizeString = snprintf(NULL, 0, "script/tempIV_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/tempIV_%d.sh", uniqueID);

  sizeString = snprintf(NULL, 0, "data/iv_%d.txt", uniqueID);
  char *ivFile = (char *)malloc(sizeString + 1);
  snprintf(ivFile, sizeString+1, "data/iv_%d.txt", uniqueID);
    
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
  if (strcmp(saveFile, "") != 0) {
    fprintf(fp, "set terminal png\n");
    fprintf(fp, "set output '%s'\n", saveFile);
  }

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
  remove(ivFile);

  free(gnuPlotFile);
  free(ivFile);
  
  /* Freeing vectors */
  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(time);

  
  return 0;

}


/******************************************************************************
 * Function: plotAxialIz
 * Inputs: 
 * Returns: int
 * Description: This will plot the m = 0 mode, or Iz for several difference
 * axial locations
 ******************************************************************************/

static int plotAxialIz(int shotNumber, int tmin, int tmax, char *saveFile, int uniqueID) {

  char *data1Node = "\\b_p05_000_sm",
    *data1Name = "z = 5 cm",
    *data2Node = "\\b_p15_000_sm",
    *data2Name = "z = 15 cm",
    *data3Node = "\\b_p25_000_sm",
    *data3Name = "z = 25 cm",
    *data4Node = "\\b_p35_000_sm",
    *data4Name = "z = 35 cm",
    *data5Node = "\\b_p45_000_sm",
    *data5Name = "z = 45 cm";

  int status;

  int sizeString = snprintf(NULL, 0, "script/tempAxialIZ_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/tempAxialIZ_%d.sh", uniqueID);

  sizeString = snprintf(NULL, 0, "data/axialIZ_%d.txt", uniqueID);
  char *axialIZFile = (char *)malloc(sizeString + 1);
  snprintf(axialIZFile, sizeString+1, "data/axialIZ_%d.txt", uniqueID);
    
  /* Getting data. We want 0 vs. 1 col */
  int sigSize = getSignalLengthMDSplus(data1Node, shotNumber);
    
  gsl_vector *iz1 = gsl_vector_alloc(sigSize),
    *iz2 = gsl_vector_alloc(sigSize),
    *iz3 = gsl_vector_alloc(sigSize),
    *iz4 = gsl_vector_alloc(sigSize),
    *iz5 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  gsl_matrix *azimuthArray1 = getAzimuthalArray(shotNumber, data1Node);
  getAzimuthalArrayModes(azimuthArray1);
  gsl_matrix_get_col(time, azimuthArray1, 0);
  gsl_matrix_get_col(iz1, azimuthArray1, 1);
  
  gsl_matrix *azimuthArray2 = getAzimuthalArray(shotNumber, data2Node);
  getAzimuthalArrayModes(azimuthArray2);
  gsl_matrix_get_col(iz2, azimuthArray2, 1);

  gsl_matrix *azimuthArray3 = getAzimuthalArray(shotNumber, data3Node);
  getAzimuthalArrayModes(azimuthArray3);
  gsl_matrix_get_col(iz3, azimuthArray3, 1);

  gsl_matrix *azimuthArray4 = getAzimuthalArray(shotNumber, data4Node);
  getAzimuthalArrayModes(azimuthArray4);
  gsl_matrix_get_col(iz4, azimuthArray4, 1);

  gsl_matrix *azimuthArray5 = getAzimuthalArray(shotNumber, data5Node);
  getAzimuthalArrayModes(azimuthArray5);
  gsl_matrix_get_col(iz5, azimuthArray5, 1);

  /* Saving data */
  save6VectorData(time, iz1, iz2, iz3, iz4, iz5, axialIZFile);


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
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'I_{Z} for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'I (kA)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E6):($2/0.002) with line lw 3 lc rgb 'red' title '%s',\\\n",
	  axialIZFile, data1Name);
  fprintf(fp, "     '%s' using ($1*1E6):($3/0.002) with line lw 3 lc rgb 'black' title '%s',\\\n",
	  axialIZFile, data2Name);
  fprintf(fp, "     '%s' using ($1*1E6):($4/0.002) with line lw 3 lc rgb 'green' title '%s',\\\n",
	  axialIZFile, data3Name);
  fprintf(fp, "     '%s' using ($1*1E6):($5/0.002) with line lw 3 lc rgb 'blue' title '%s',\\\n",
	  axialIZFile, data4Name);
  fprintf(fp, "     '%s' using ($1*1E6):($6/0.002) with line lw 3 lc rgb 'purple' title '%s'\n",
	  axialIZFile, data5Name);

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
  remove(axialIZFile);

  free(gnuPlotFile);
  free(axialIZFile);
  
  /* Freeing vectors */
  gsl_vector_free(iz1);
  gsl_vector_free(iz2);
  gsl_vector_free(time);
  gsl_matrix_free(azimuthArray1);
  gsl_matrix_free(azimuthArray2);

  
  return 0;

}
