#include "save.h"


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
