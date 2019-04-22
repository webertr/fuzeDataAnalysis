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


/******************************************************************************
 * Function: plotImageData
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: This will save the data as binary floats, then use popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageData(gsl_matrix *mInput, double dx, double dy, std::string plotOptions,
		  std::string tempDataFile, std::string tempScriptFile) {

  /* Removing files to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Allocating the matrix to save */  
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values to indices*/
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii*dy);
  }
  /* Setting x vector values to indices*/
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii*dx);
  }
  /* Setting matrix values */
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }

  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen(tempDataFile.c_str(), "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "set palette rgb 33,13,10\n");
  fprintf(fpScript, "%s\n", plotOptions.c_str());
  fprintf(fpScript, "plot '%s' binary matrix with image title ''\n", tempDataFile.c_str());
  fprintf(fpScript, "pause -1\n");

  fclose(fpScript);

  chmod(tempScriptFile.c_str(), S_IRWXG);
  chmod(tempScriptFile.c_str(), S_IRWXO);
  chmod(tempScriptFile.c_str(), S_IRWXU);

  char pathBuf[100];
  char *realPath = realpath(tempScriptFile.c_str(), pathBuf);
  
  system(realPath);    
 
  /* Pausing so user can look at plot */
  printf("\nPress any key, then ENTER to continue> \n");
  getchar();

  return 0;

}
