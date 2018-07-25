#include "pinchAnalysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotModeData(int shotNumber, int zVal, int tmin, int tmax,
			char *nodeName, char *saveFile, int uniqueID);

/******************************************************************************
 * Function: pinchAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will run call some functions to analyze pinch data
 ******************************************************************************/

int pinchAnalysis() {

  int shotNumber = 180723020;

  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  int timeCompI = 30,
    timeCompF = 60;
  

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    plotModeData(shotNumber, 15, timeCompI, timeCompF, "\\b_p15_000_sm", "", 1);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    plotModeData(shotNumber, 25, timeCompI, timeCompF, "\\b_p25_000_sm", "", 2);
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    plotModeData(shotNumber, 5, timeCompI, timeCompF, "\\b_p5_000_sm", "", 3);
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
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
  
  int sizeString = snprintf(NULL, 0, "script/temp_%d.sh", uniqueID);
  char *gnuPlotFile = (char *)malloc(sizeString + 1);
  snprintf(gnuPlotFile, sizeString+1, "script/temp_%d.sh", uniqueID);

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
  fprintf(fp, "plot '%s' using ($1*1E6):($3) with line lw 3 lc rgb 'blue' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($4) with line lw 3 lc rgb 'green' \
title 'm=2',\\\n", modeFile);
  fprintf(fp, "     '%s' using ($1*1E6):($5) with line lw 3 lc rgb 'yellow' \
title 'm=3',\\\n", modeFile);
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
