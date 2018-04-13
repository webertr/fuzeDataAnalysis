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

  gsl_vector *data1 = 0,
    *data2 = 0,
    *time = 0;

  initializeMagneticDataAndTime(shotNumber, "\\b_n95_000_sm", &data1, &time);
  initializeMagneticData(shotNumber, "\\b_n10_000_sm", &data2);

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

  /* Declaring variables */
  gsl_vector *iP = 0,
    *cap1 = 0,
    *timeDetaq = 0,
    *timeCAMAC = 0;

  int status;
  
  char *gnuPlotFile = "script/temp.sh",
    *ipFile = "data/ip.txt",
    *cap1File = "data/cap1.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\I_P", &iP, &timeDetaq);
  initializeMagneticDataAndTime(shotNumber, "\\I_TBM_01_CAP", &cap1, &timeCAMAC);


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
