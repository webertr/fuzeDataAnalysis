#include "save.h"


/******************************************************************************
 * Function: saveImageData
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: Save binary matrix data to be read by gnuplot such as:
 * MS = zeros(length(x)+1,length(y)+1);
 * MS(1,1) = length(x);
 * MS(1,2:end) = y;
 * MS(2:end,1) = x;
 * MS(2:end,2:end) = M';
 * % Write data into the file
 * fid = fopen(file,'w');
 * fwrite(fid,MS,'float');
 * fclose(fid);
 * plot 'color_map.bin' binary matrix with image
 * Example:
 * plot 'data/test.dat' binary matrix with image title "Line Integrated"
 ******************************************************************************/

int saveImageData(gsl_matrix *mInput, char *fileName) {

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
			 (float) ii);
  }
  /* Setting x vector values to indices*/
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
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
  fp2 = fopen(fileName, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: save2VectorData
 * Inputs: gsl_vector*, gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 2 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, char *fileName) {

  int ii;
  
  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < vec1In->size; ii++) {

    fprintf(fp, "%g\t%g\n", gsl_vector_get(vec1In, ii), gsl_vector_get(vec2In, ii));

  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: save5VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 5 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int save5VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, char *fileName) {

  int ii;
  
  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < vec1In->size; ii++) {

    fprintf(fp, "%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(vec1In, ii), 
	    gsl_vector_get(vec2In, ii), gsl_vector_get(vec3In, ii), gsl_vector_get(vec4In, ii),
	    gsl_vector_get(vec5In, ii));

  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: save6VectorData
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, 
 * gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 6 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int save6VectorData(gsl_vector *vec1In, gsl_vector *vec2In, gsl_vector *vec3In,
		    gsl_vector *vec4In, gsl_vector *vec5In, gsl_vector *vec6In, 
		    char *fileName) {

  int ii;
  
  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < vec1In->size; ii++) {

    fprintf(fp, "%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(vec1In, ii), 
	    gsl_vector_get(vec2In, ii), gsl_vector_get(vec3In, ii), gsl_vector_get(vec4In, ii),
	    gsl_vector_get(vec5In, ii), gsl_vector_get(vec6In, ii));

  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: saveMatrixData
 * Inputs: gsl_matrix*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the matrix into a series of columns for each column
 ******************************************************************************/

int saveMatrixData(gsl_matrix *mIn, char *fileName) {

  int ii, jj;
  
  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < mIn->size1; ii++) {

    for (jj = 0; jj < mIn->size2; jj++) {

      fprintf(fp, "%g\t", gsl_matrix_get(mIn, ii, jj));

    }
    fprintf(fp, "\n");
  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: saveVectorData
 * Inputs: gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 1 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int saveVectorData(gsl_vector *vecIn, char *fileName) {

  int ii;
  
  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < vecIn->size; ii++) {

    fprintf(fp, "%g\n", gsl_vector_get(vecIn, ii));

  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: readImageData
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: Reads an imaged that was saved with the column and row information
 * encoded in the file by, for example, saveImageData
 ******************************************************************************/

int readImageData(gsl_matrix *mInput, char *fileName) {

  int numRows = mInput->size1,
    numCols = mInput->size2,
    ii, jj;

  /* Allocating the matrix to read data into */  
  gsl_matrix_float *tempMatrix = gsl_matrix_float_alloc(numRows+1, numCols+1);

  FILE *fp;
  fp = fopen(fileName, "rb");
  gsl_matrix_float_fread(fp, tempMatrix);
  fclose(fp);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_set(mInput, ii, jj,
		     gsl_matrix_float_get(tempMatrix, ii+1, jj+1));
    }
  }

  gsl_matrix_float_free(tempMatrix);

  return 0;

}


/******************************************************************************
 * Function: saveImageDataWithPosition
 * Inputs: gsl_matrix *, gsl_vector *, gsl_vector *, char *
 * Returns: int
 * Description: Save binary matrix data to be read by gnuplot such as:
 * MS = zeros(length(x)+1,length(y)+1);
 * MS(1,1) = length(x);
 * MS(1,2:end) = y;
 * MS(2:end,1) = x;
 * MS(2:end,2:end) = M';
 * % Write data into the file
 * fid = fopen(file,'w');
 * fwrite(fid,MS,'float');
 * fclose(fid);
 * plot 'color_map.bin' binary matrix with image
 * Example:
 * plot 'data/lineIntegrated.dat' binary matrix with image title "Line Integrated"
 ******************************************************************************/

int saveImageDataWithPosition(gsl_matrix *mInput, gsl_vector* xVec, gsl_vector* yVec,
			      char *fileName) {

  int numRows = yVec->size;
  int numCols = xVec->size;

  /* Creates matrix to get written */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) gsl_vector_get(yVec, ii-1));
  }
  /* Setting x vector values */
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) gsl_vector_get(xVec, ii-1));
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
  fp2 = fopen(fileName, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: saveVectorBinary
 * Inputs: gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 1 vectors in a single binary file
 ******************************************************************************/

int saveVectorBinary(gsl_vector *vecIn, char *fileName) {

  if (remove(fileName) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName, "wb");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 3 vector save!\n");
    exit(1);

  }

  gsl_vector_fwrite(fp, vecIn);

  fclose(fp);

  return 0;

}
