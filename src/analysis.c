#include "analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: plotPostShotData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotData(int shotNumber) {

  int sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, "\\b_n95_000_sm", data1, time);
  initializeMagneticData(shotNumber, "\\b_n10_000_sm", data2);

  plot2VectorData(time, data1, data2, "set xrange[0:50E-6]");

  gsl_vector_free(data1);
  gsl_vector_free(data2);
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
  //fprintf(fp, "set terminal png\n");
  //fprintf(fp, "set output 'data/accel180223035'\n");
  fprintf(fp, "set xrange[0:15]\n");
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Acceleration Region for Pulse #180223035' font '0,18'\n");
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
    sigSize = getSignalLengthMDSplus("\\b_p15_000", shotNumber),
    status;

  gsl_matrix *azimuthArray = gsl_matrix_alloc(8, sigSize);
  
  /* Declaring variables */
  gsl_vector_view p15_000 = gsl_matrix_row(azimuthArray, 0),
    p15_045 = gsl_matrix_row(azimuthArray, 1),
    p15_090 = gsl_matrix_row(azimuthArray, 2),
    p15_135 = gsl_matrix_row(azimuthArray, 3),
    p15_180 = gsl_matrix_row(azimuthArray, 4),
    p15_225 = gsl_matrix_row(azimuthArray, 5),
    p15_270 = gsl_matrix_row(azimuthArray, 6),
    p15_315 = gsl_matrix_row(azimuthArray, 7);

  gsl_vector *timeDetaq = gsl_vector_alloc(sigSize);

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";


  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\b_p15_000", &p15_000.vector, timeDetaq);
  initializeMagneticData(shotNumber, "\\b_p15_045", &p15_045.vector);
  initializeMagneticData(shotNumber, "\\b_p15_090", &p15_090.vector);
  initializeMagneticData(shotNumber, "\\b_p15_135", &p15_135.vector);
  initializeMagneticData(shotNumber, "\\b_p15_180", &p15_180.vector);
  initializeMagneticData(shotNumber, "\\b_p15_225", &p15_225.vector);
  initializeMagneticData(shotNumber, "\\b_p15_270", &p15_270.vector);
  initializeMagneticData(shotNumber, "\\b_p15_315", &p15_315.vector);


  /* Saving data */
  save5VectorData(timeDetaq, &p15_000.vector, &p15_045.vector, &p15_090.vector,
		  &p15_135.vector, modeFile);


  
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
  //fprintf(fp, "set output 'data/accel180223035'\n");
  fprintf(fp, "set xrange[0:40]\n");
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Acceleration Region for Pulse #180223035' font '0,18'\n");
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+14E-6)*1E6):($2) with line lw 3 lc rgb 'black' \
title 'z = 15cm, 0 Deg.',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($3) with line lw 3 lc rgb 'red' \
title 'z = 15cm, 45 Deg.',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($4) with line lw 3 lc rgb 'blue' \
title 'z = 15cm, 90 Deg.',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($5) with line lw 3 lc rgb 'yellow' \
title 'z = 15cm, 135 Deg.'\n", modeFile);
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
  gsl_vector_free(timeDetaq);

  return 0;


}
