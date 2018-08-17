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
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		     char *plotOptions) {

  int ii, status;

  char *tempFile = "data/temp.txt";


  
  /* Writing file to hold data */
  
  if (remove(tempFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(tempFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files!\n");
    exit(1);

  }

  for (ii = 0; ii < xVecIn->size; ii++) {

    fprintf(fp, "%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii));

  }

  fclose(fp);






  /* 
   * Creating child process to then call "gnuplot" in shell, and then to pipe the standard output
   * to it.
   */
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot 'data/temp.txt' using 1:2 title 'Vector 1',\
                    'data/temp.txt' using 1:3 title 'Vector 2'\n");
  
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
 * Function: plot5VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*
 * char *
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot5VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		     gsl_vector *yVec3In, gsl_vector *yVec4In, gsl_vector *yVec5In,
		     char *y1Label, char *y2Label, char *y3Label, char *y4Label,
		     char *y5Label, char *plotOptions) {

  int ii, status;

  char *tempFile = "data/temp.txt";


  
  /* Writing file to hold data */
  
  if (remove(tempFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(tempFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files!\n");
    exit(1);

  }

  for (ii = 0; ii < xVecIn->size; ii++) {

    fprintf(fp, "%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii),
	    gsl_vector_get(yVec3In, ii), gsl_vector_get(yVec4In, ii), 
	    gsl_vector_get(yVec5In, ii));

  }

  fclose(fp);






  /* 
   * Creating child process to then call "gnuplot" in shell, and then to pipe the standard output
   * to it.
   */
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot 'data/temp.txt' using 1:2 title '%s',\\\n", y1Label);
  fprintf(gnuplot, "     'data/temp.txt' using 1:3 title '%s',\\\n", y2Label);
  fprintf(gnuplot, "     'data/temp.txt' using 1:4 title '%s',\\\n", y3Label);
  fprintf(gnuplot, "     'data/temp.txt' using 1:5 title '%s',\\\n", y4Label);
  fprintf(gnuplot, "     'data/temp.txt' using 1:6 title '%s'\n", y5Label);
 
  
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
 * Function: plotImageData
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: This will save the data as binary floats, then use popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageData (gsl_matrix *mInput, double dx, double dy, char *plotOptions) {

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
  fp2 = fopen("data/temp.dat", "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);

  fprintf(gnuplot, "set palette rgb 33,13,10\n");
  fprintf(gnuplot, "plot 'data/temp.dat' binary matrix with image title ''\n");

  fflush(gnuplot);

  /* Pausing before kill process that has gnuplot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  remove("data/temp.dat");

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
		      char *plotOptions) {

  int status,
    useScale = 1;

  if ( (xScale == 1) && (yScale == 1) && (zScale == 1) )
    useScale = 0;

  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "set palette rgb 33,13,10\n");
  fprintf(gnuplot, "%s\n", plotOptions);
  
  if (useScale) {
    fprintf(gnuplot, "plot '%s' binary matrix using ($1*%g):($2*%g):($3*%g) with image title ''\n", 
	    fileName, xScale, yScale, zScale);
  } else {
    fprintf(gnuplot, "plot '%s' binary matrix with image title ''\n", fileName);
  }

  fflush(gnuplot);

  /* Pausing before kill process that has gnuplot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  remove("data/temp.dat");

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
