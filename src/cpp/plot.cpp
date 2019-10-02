#include "cpp/plot.h"

/******************************************************************************
 *
 * This is the source file to plot data from the FuZE experiment
 *
 ******************************************************************************/

/******************************************************************************
 * Function: plot1DVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot1DVectorData(gsl_vector *yVecIn, std::string yLabel, std::string plotOptions, 
		     std::string tempDataFile, std::string tempScriptFile) {

  int ii, len = yVecIn->size;

  
  /* Writing file to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%d\t%g\n", ii, gsl_vector_get(yVecIn, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s\n", tempDataFile.c_str(), yLabel.c_str());
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


/******************************************************************************
 * Function: plotVectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, std::string yLabel,
		   std::string plotOptions, std::string tempDataFile, std::string tempScriptFile) {

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
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVecIn, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s\n", tempDataFile.c_str(), yLabel.c_str());
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


/******************************************************************************
 * Function: plot2VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, std::string plotOptions, 
		    std::string tempDataFile, std::string tempScriptFile) {

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
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile.c_str(), y1Label.c_str());
  fprintf(fpScript, "     '%s' using 1:3 %s\n", tempDataFile.c_str(), y2Label.c_str());
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


/******************************************************************************
 * Function: plot3VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot3VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[4];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
  lengths[3] = yVec3In->size;
    
  for (ii = 0; ii < 3; ii++) {
    if (lengths[ii] != lengths[ii+1]) {
      printf("Vectors not all the same length");
      return -1;
    }
  }
  
  /* Writing file to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii), gsl_vector_get(yVec3In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile.c_str(), y1Label.c_str());
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile.c_str(), y2Label.c_str());
  fprintf(fpScript, "     '%s' using 1:4 %s\n", tempDataFile.c_str(), y3Label.c_str());
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


/******************************************************************************
 * Function: plot4VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot4VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    gsl_vector *yVec4In, std::string y4Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[5];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
  lengths[3] = yVec3In->size;
  lengths[4] = yVec4In->size;
  
  for (ii = 0; ii < 4; ii++) {
    if (lengths[ii] != lengths[ii+1]) {
      printf("Vectors not all the same length");
      return -1;
    }
  }
  
  /* Writing file to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii), 
	    gsl_vector_get(yVec3In, ii), gsl_vector_get(yVec4In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile.c_str(), y1Label.c_str());
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile.c_str(), y2Label.c_str());
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile.c_str(), y3Label.c_str());
  fprintf(fpScript, "     '%s' using 1:5 %s\n", tempDataFile.c_str(), y4Label.c_str());
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

/******************************************************************************
 * Function: plot5VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot5VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    gsl_vector *yVec4In, std::string y4Label,
		    gsl_vector *yVec5In, std::string y5Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[6];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
  lengths[3] = yVec3In->size;
  lengths[4] = yVec4In->size;
  lengths[5] = yVec5In->size;
  
  for (ii = 0; ii < 5; ii++) {
    if (lengths[ii] != lengths[ii+1]) {
      printf("Vectors not all the same length");
      return -1;
    }
  }
  
  /* Writing file to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii), 
	    gsl_vector_get(yVec3In, ii), gsl_vector_get(yVec4In, ii), 
	    gsl_vector_get(yVec5In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile.c_str(), y1Label.c_str());
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile.c_str(), y2Label.c_str());
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile.c_str(), y3Label.c_str());
  fprintf(fpScript, "     '%s' using 1:5 %s,\\\n", tempDataFile.c_str(), y4Label.c_str());
  fprintf(fpScript, "     '%s' using 1:6 %s\n", tempDataFile.c_str(), y5Label.c_str());
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


/******************************************************************************
 * Function: plot8VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot8VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    gsl_vector *yVec4In, std::string y4Label,
		    gsl_vector *yVec5In, std::string y5Label,
		    gsl_vector *yVec6In, std::string y6Label,
		    gsl_vector *yVec7In, std::string y7Label,
		    gsl_vector *yVec8In, std::string y8Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[9];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
  lengths[3] = yVec3In->size;
  lengths[4] = yVec4In->size;
  lengths[5] = yVec5In->size;
  lengths[6] = yVec6In->size;
  lengths[7] = yVec7In->size;
  lengths[8] = yVec8In->size;
  
  for (ii = 0; ii < 8; ii++) {
    if (lengths[ii] != lengths[ii+1]) {
      printf("Vectors not all the same length");
      return -1;
    }
  }
  
  /* Writing file to hold data */
  remove(tempDataFile.c_str());
  remove(tempScriptFile.c_str());

  FILE *fpData = fopen(tempDataFile.c_str(), "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii), 
	    gsl_vector_get(yVec3In, ii), gsl_vector_get(yVec4In, ii),
	    gsl_vector_get(yVec5In, ii), gsl_vector_get(yVec6In, ii),
	    gsl_vector_get(yVec7In, ii), gsl_vector_get(yVec8In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile.c_str(), "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions.c_str());
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile.c_str(), y1Label.c_str());
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile.c_str(), y2Label.c_str());
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile.c_str(), y3Label.c_str());
  fprintf(fpScript, "     '%s' using 1:5 %s,\\\n", tempDataFile.c_str(), y4Label.c_str());
  fprintf(fpScript, "     '%s' using 1:6 %s,\\\n", tempDataFile.c_str(), y5Label.c_str());
  fprintf(fpScript, "     '%s' using 1:7 %s,\\\n", tempDataFile.c_str(), y6Label.c_str());
  fprintf(fpScript, "     '%s' using 1:8 %s,\\\n", tempDataFile.c_str(), y7Label.c_str());
  fprintf(fpScript, "     '%s' using 1:9 %s\n", tempDataFile.c_str(), y8Label.c_str());
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
