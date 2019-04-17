#include "cpp/plot.h"

/******************************************************************************
 *
 * This is the source file to plot data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: plotVectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, const char *yLabel,
		   const char *plotOptions, const char *tempDataFile, const char *tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[2];

  lengths[0] = xVecIn->size;
  lengths[1] = yVecIn->size;
    
  if (lengths[0] != lengths[1]) {
    printf("Vectors not all the same length");
    return -1;
  }
  
  /* Writing file to hold data */
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVecIn, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2 %s\n", tempDataFile, yLabel);
  fprintf(fpScript, "pause -1\n");
  
  fclose(fpScript);

  chmod(tempScriptFile, S_IRWXG);
  chmod(tempScriptFile, S_IRWXO);
  chmod(tempScriptFile, S_IRWXU);

  char pathBuf[100];
  char *realPath = realpath(tempScriptFile, pathBuf);
  
  system(realPath);    
 
  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();


  return 0;

}


/******************************************************************************
 * Function: plot2VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, const char *y1Label,
		    gsl_vector *yVec2In, const char *y2Label, const char *plotOptions, 
		    const char *tempDataFile, const char *tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[3];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
    
  for (ii = 0; ii < 2; ii++) {
    if (lengths[ii] != lengths[ii+1]) {
      printf("Vectors not all the same length");
      return -1;
    }
  }
  
  /* Writing file to hold data */
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile, y1Label);
  fprintf(fpScript, "     '%s' using 1:3 %s\n", tempDataFile, y2Label);
  fprintf(fpScript, "pause -1\n");
  
  fclose(fpScript);

  chmod(tempScriptFile, S_IRWXG);
  chmod(tempScriptFile, S_IRWXO);
  chmod(tempScriptFile, S_IRWXU);

  char pathBuf[100];
  char *realPath = realpath(tempScriptFile, pathBuf);
  
  system(realPath);    
 
  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();


  return 0;

}
