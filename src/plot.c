#include "plot.h"

/******************************************************************************
 * Example Usage:
 * This is used to plot data using gnuplot
 ******************************************************************************/

/******************************************************************************
 * Function: plot1DVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot1DVectorData (gsl_vector *vecIn, char *plotOptions) {

  int ii, status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '-'\n");

  for (ii = 0; ii < vecIn->size; ii++) {

    fprintf(gnuplot, "%d %g\n", ii, gsl_vector_get(vecIn, ii));

  }

  fprintf(gnuplot, "e\n");

  fflush(gnuplot);

  /* Pausing */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  return 0;

}


/******************************************************************************
 * Function: plotVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotVectorData (gsl_vector *xVecIn, gsl_vector *yVecIn, char *plotOptions) {

  int ii, status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '-'\n");

  for (ii = 0; ii < xVecIn->size; ii++) {

    fprintf(gnuplot, "%g %g\n", gsl_vector_get(xVecIn, ii), gsl_vector_get(yVecIn, ii));

  }

  fprintf(gnuplot, "e\n");

  fflush(gnuplot);

  /* Pausing */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

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

int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, char *plotOptions, char *tempDataFile, 
		     char *tempScriptFile) {

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
 * Function: plot3VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot3VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, char *plotOptions, char *tempDataFile, 
		     char *tempScriptFile) {

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
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii),
	    gsl_vector_get(yVec3In, ii));

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
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile, y2Label);
  fprintf(fpScript, "     '%s' using 1:4 %s\n", tempDataFile, y3Label);
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
 * Function: plot4VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot4VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     char *plotOptions, char *tempDataFile, char *tempScriptFile) {

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
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
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

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile, y1Label);
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile, y2Label);
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile, y3Label);
  fprintf(fpScript, "     '%s' using 1:5 %s\n", tempDataFile, y4Label);
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
 * Function: plot5VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot5VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, char *plotOptions,
		     char *tempDataFile, char *tempScriptFile) {

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
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
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

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile, y1Label);
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile, y2Label);
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile, y3Label);
  fprintf(fpScript, "     '%s' using 1:5 %s,\\\n", tempDataFile, y4Label);
  fprintf(fpScript, "     '%s' using 1:6 %s\n", tempDataFile, y5Label);
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
 * Function: plot7VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot7VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, gsl_vector *yVec6In, char *y6Label,
		     gsl_vector *yVec7In, char *y7Label,
		     char *plotOptions, char *tempDataFile, char *tempScriptFile) {

  int ii,
    len = xVecIn->size;

  int lengths[8];

  lengths[0] = xVecIn->size;
  lengths[1] = yVec1In->size;
  lengths[2] = yVec2In->size;
  lengths[3] = yVec3In->size;
  lengths[4] = yVec4In->size;
  lengths[5] = yVec5In->size;
  lengths[6] = yVec6In->size;
  lengths[7] = yVec7In->size;
    
  for (ii = 0; ii < 7; ii++) {
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

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii),
	    gsl_vector_get(yVec3In, ii), gsl_vector_get(yVec4In, ii), 
	    gsl_vector_get(yVec5In, ii), gsl_vector_get(yVec6In, ii),
	    gsl_vector_get(yVec7In, ii));

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
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile, y2Label);
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile, y3Label);
  fprintf(fpScript, "     '%s' using 1:5 %s,\\\n", tempDataFile, y4Label);
  fprintf(fpScript, "     '%s' using 1:6 %s,\\\n", tempDataFile, y5Label);
  fprintf(fpScript, "     '%s' using 1:7 %s,\\\n", tempDataFile, y6Label);
  fprintf(fpScript, "     '%s' using 1:8 %s\n", tempDataFile, y7Label);
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
 * Function: plot8VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot8VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, gsl_vector *yVec6In, char *y6Label,
		     gsl_vector *yVec7In, char *y7Label, gsl_vector *yVec8In, char *y8Label, 
		     char *plotOptions, char *tempDataFile, char *tempScriptFile) {

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
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
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

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2 %s,\\\n", tempDataFile, y1Label);
  fprintf(fpScript, "     '%s' using 1:3 %s,\\\n", tempDataFile, y2Label);
  fprintf(fpScript, "     '%s' using 1:4 %s,\\\n", tempDataFile, y3Label);
  fprintf(fpScript, "     '%s' using 1:5 %s,\\\n", tempDataFile, y4Label);
  fprintf(fpScript, "     '%s' using 1:6 %s,\\\n", tempDataFile, y5Label);
  fprintf(fpScript, "     '%s' using 1:7 %s,\\\n", tempDataFile, y6Label);
  fprintf(fpScript, "     '%s' using 1:8 %s,\\\n", tempDataFile, y7Label);
  fprintf(fpScript, "     '%s' using 1:9 %s\n", tempDataFile, y8Label);
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
 * Function: plot6PlotsVectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot6PlotsVectorData (gsl_vector *xVec1In, gsl_vector *yVec1In, char *y1Label,
			  gsl_vector *xVec2In, gsl_vector *yVec2In, char *y2Label, 
			  gsl_vector *xVec3In, gsl_vector *yVec3In, char *y3Label, 
			  gsl_vector *xVec4In, gsl_vector *yVec4In, char *y4Label, 
			  gsl_vector *xVec5In, gsl_vector *yVec5In, char *y5Label,
			  gsl_vector *xVec6In, gsl_vector *yVec6In, char *y6Label, 
			  char *plotOptions, char *tempDataFile, 
			  char *tempScriptFile) {

  int ii,
    len = xVec1In->size;

  /* Writing file to hold data */
  remove(tempDataFile);
  remove(tempScriptFile);

  FILE *fpData = fopen(tempDataFile, "w");
  
  if ( (fpData == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  for (ii = 0; ii < len; ii++) {

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", 
	    gsl_vector_get(xVec1In, ii), gsl_vector_get(yVec1In, ii),
	    gsl_vector_get(xVec2In, ii), gsl_vector_get(yVec2In, ii), 
	    gsl_vector_get(xVec3In, ii), gsl_vector_get(yVec3In, ii),
	    gsl_vector_get(xVec4In, ii), gsl_vector_get(yVec4In, ii),
	    gsl_vector_get(xVec5In, ii), gsl_vector_get(yVec5In, ii),
	    gsl_vector_get(xVec6In, ii), gsl_vector_get(yVec6In, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);

  fprintf(fpScript, "set multiplot layout 3,2 rowsfirst\n");
  fprintf(fpScript, "plot '%s' using 1:2 %s\n", tempDataFile, y1Label);
  fprintf(fpScript, "plot '%s' using 3:4 %s\n", tempDataFile, y2Label);
  fprintf(fpScript, "plot '%s' using 5:6 %s\n", tempDataFile, y3Label);
  fprintf(fpScript, "plot '%s' using 7:8 %s\n", tempDataFile, y4Label);
  fprintf(fpScript, "plot '%s' using 9:10 %s\n", tempDataFile, y5Label);
  fprintf(fpScript, "plot '%s' using 11:12 %s\n", tempDataFile, y6Label);
  fprintf(fpScript, "unset multiplot\n");

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
 * Function: plot3PlotsVectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot3PlotsVectorData (gsl_vector *xVec1In, gsl_vector *yVec1In, char *y1Label, char *y1Key,
			  gsl_vector *xVec2In, gsl_vector *yVec2In, char *y2Label, char *y2Key,
			  gsl_vector *xVec3In, gsl_vector *yVec3In, char *y3Label, char *y3Key,
			  char *plotOptions, char *tempDataFile, 
			  char *tempScriptFile) {

  int ii,
    len1 = xVec1In->size,
    len2 = xVec2In->size,
    len3 = xVec3In->size,
    len;

  if ( (len1 <= len2) && (len1 <= len3)) {
    len = len1;
  } else if ((len2 <= len1) && (len2 <= len3)) {
    len = len2;
  } else {
    len = len3;
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

    fprintf(fpData, "%g\t%g\t%g\t%g\t%g\t%g\n",
	    gsl_vector_get(xVec1In, ii), gsl_vector_get(yVec1In, ii),
	    gsl_vector_get(xVec2In, ii), gsl_vector_get(yVec2In, ii), 
	    gsl_vector_get(xVec3In, ii), gsl_vector_get(yVec3In, ii));
  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);

  fprintf(fpScript, "set multiplot layout 3,1 rowsfirst\n");
  fprintf(fpScript, "%s\n", y1Key);
  fprintf(fpScript, "plot '%s' using 1:2 %s\n", tempDataFile, y1Label);
  fprintf(fpScript, "%s\n", y2Key);
  fprintf(fpScript, "plot '%s' using 3:4 %s\n", tempDataFile, y2Label);
  fprintf(fpScript, "%s\n", y3Key);
  fprintf(fpScript, "plot '%s' using 5:6 %s\n", tempDataFile, y3Label);
  fprintf(fpScript, "unset multiplot\n");

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
 * Function: plotImageDimensions
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: This will save the data as binary floats, then use popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageDimensions(gsl_matrix *mInput, gsl_vector *yVec, gsl_vector *xVec, char *plotOptions,
			char *tempDataFile, char *tempScriptFile) {

  /* Removing files to hold data */
  remove(tempDataFile);
  remove(tempScriptFile);

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
			 (float) gsl_vector_get(yVec,ii-1));
  }
  /* Setting x vector values to indices*/
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) gsl_vector_get(xVec,ii-1));
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
  fp2 = fopen(tempDataFile, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);

  fprintf(fpScript, "set palette rgb 33,13,10\n");
  fprintf(fpScript, "plot '%s' binary matrix with image title ''\n", tempDataFile);
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

int plotImageData (gsl_matrix *mInput, double dx, double dy, char *plotOptions,
		   char *tempDataFile, char *tempScriptFile) {

  /* Removing files to hold data */
  remove(tempDataFile);
  remove(tempScriptFile);

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
  fp2 = fopen(tempDataFile, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);

  fprintf(fpScript, "set palette rgb 33,13,10\n");
  fprintf(fpScript, "plot '%s' binary matrix with image title ''\n", tempDataFile);
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
 * Function: plotImageDataFile
 * Inputs: char *
 * Returns: int
 * Description: This will plot an image from a passed file. It uses popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageDataFile(char *fileName, double xScale, double yScale, double zScale,
		      char *plotOptions, char *tempScriptFile) {

  /* Removing files to hold data */
  remove(tempScriptFile);

  int useScale = 1;

  if ( (xScale == 1) && (yScale == 1) && (zScale == 1) )
    useScale = 0;

  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {
    printf("Error opening files gnuplot file!\n");
    exit(1);
  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "set palette rgb 33,13,10\n");
  fprintf(fpScript, "%s\n", plotOptions);
  
  if (useScale) {
    fprintf(fpScript, "plot '%s' binary matrix using ($1*%g):($2*%g):($3*%g) with image title ''\n", 
	    fileName, xScale, yScale, zScale);
  } else {
    fprintf(fpScript, "plot '%s' binary matrix with image title ''\n", fileName);
  }

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
 * Function: plotMatrixDataFile
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotMatrixColDataFile (char *fileName, int colNum, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '%s' using 0:%d with points title '1'\n", fileName, colNum+1);
  
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
 * Function: plot2MatrixDataFile
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot2MatrixColDataFile (char *fileName1, int colNum1, char *fileName2, int colNum2, 
			    char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '%s' using 0:%d with points title '1',\\\n", fileName1, colNum1+1);
  fprintf(gnuplot, "     '%s' using 0:%d with points title '2'\n", fileName2, colNum2+1);
  
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
 * Function: plotMatrixColVColDataFile
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotMatrixColVColDataFile (char *fileName, int colNumX, int colNumY, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '%s' using %d:%d with points pt 7 title ''\n", 
	  fileName, colNumX+1, colNumY+1);
  
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
 * Function: plotMatrixColVColErrorDataFile
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream. It will use
 * error bars
 ******************************************************************************/

int plotMatrixColVColErrorDataFile (char *fileName, int colNumX, int colNumY, 
				    int colNumError, double xScale, double yScale, 
				    double errorScale, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '%s' using ($%d*%g):($%d*%g):($%d*%g) pt 7 with yerrorbars title ''\n", 
	  fileName, colNumX+1, xScale, colNumY+1, yScale, colNumError+1, errorScale);
  
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
 * Function: plotMatrixColVColErrorData2Files2Axes
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream. It will use
 * error bars
 ******************************************************************************/

int plotMatrixColVColErrorData2Files2Axes(char *fileName1, int colNumX1, int colNumY1, 
					  int colNumError1, char *label1, double xScale1,
					  double yScale1, double errorScale1,
					  char *fileName2, int colNumX2, int colNumY2, 
					  int colNumError2, double xScale2, double yScale2, 
					  double errorScale2, char *label2, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, 
	  "plot '%s' using ($%d*%g):($%d*%g):($%d*%g) pt 7 with yerrorbars title '%s',\\\n", 
	  fileName1, colNumX1+1, xScale1, colNumY1+1, yScale1, colNumError1+1, 
	  errorScale1, label1);
  fprintf(gnuplot,
	  "     '%s' using ($%d*%g):($%d*%g):($%d*%g) pt 7 with yerrorbars title '%s' axes x1y2\n",
	  fileName2, colNumX2+1, xScale2, colNumY2+1, yScale2, colNumError2+1, 
	  errorScale2, label2);
  
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
 * Function: plotVectorDataWithError
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will created a data file, a script file, then run the
 * script.
 ******************************************************************************/

int plotVectorDataWithError(gsl_vector *xIn, gsl_vector *yIn, char *yLabel, gsl_vector *yError,
			    char *plotOptions, char *tempDataFile, char *tempScriptFile) {

  int ii,
    len = xIn->size;

  int lengths[6];

  lengths[0] = xIn->size;
  lengths[1] = yIn->size;
  lengths[2] = yError->size;

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

    fprintf(fpData, "%g\t%g\t%g\n", gsl_vector_get(xIn, ii), 
	    gsl_vector_get(yIn, ii), gsl_vector_get(yError, ii));

  }

  fclose(fpData);

  
  FILE *fpScript = fopen(tempScriptFile, "w");
  
  if ( (fpScript == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fpScript, "#!/usr/bin/env gnuplot\n");

  fprintf(fpScript, "%s\n", plotOptions);
  
  fprintf(fpScript, "plot '%s' using 1:2:3 with yerrorbars %s\n", tempDataFile, yLabel);
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
