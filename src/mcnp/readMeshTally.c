#include "mcnp/readMeshTally.h"

/******************************************************************************
 *
 * This has some functions to read a mesh tally file outpued from MCNP
 *
 ******************************************************************************/

static gsl_vector *getMeshTallyXVector(char *fileName);
static gsl_vector *getMeshTallyYVector(char *fileName);
static gsl_vector *getMeshTallyZVector(char *fileName);
static double *getMeshTallyData(char *fileName, int xSize, int ySize, int zSize);

/******************************************************************************
 * Function: getMeshTallyXVectorDataYZPoint
 * Inputs: char *fileName, int, int, int, gsl_vector, int, int
 * Returns: gsl_vector *
 * Description: Gets the x values for the 3D array of the mesh tally file
 ******************************************************************************/

static double *getMeshTallyData(char *fileName, int xSize, int ySize, int zSize) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  /* data(ii + jj*xSize + kk*xSize*ySize) */
  double *data = malloc(sizeof(double)*xSize*ySize);

  /* Reading a x,y array for a given z */
  double zMin, zMax;
  int resTest = 0;
  while ( resTest != 2 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
    resTest = sscanf(charBuf, "Z bin: %lf - %lf", &zMin, &zMax);
  }
    
  resTest = 0;
  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally Results:  X (across) by Y (down)") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }

  double xValue;
  int ii;
  for (ii = 0; ii < (xSize-1); ii++) {
    fscanf(fp, "%lf", &xValue);
    printf("X-value: %g\n", xValue);
  }

  double yValue;
  fscanf(fp, "%lf", &yValue);
  printf("Y-value: %g\n", yValue);

  double value;  
  for (ii = 0; ii < (xSize-1); ii++) {
    fscanf(fp, "%lf", &value);
    printf("Value: %g\n", value);
  }

  fscanf(fp, "%lf", &yValue);
  printf("Y-value: %g\n", yValue);

  for (ii = 0; ii < (xSize-1); ii++) {
    fscanf(fp, "%lf", &value);
    printf("Value: %g\n", value);
  }

  exit(0);
  
  return data;

}

/******************************************************************************
 * Function: getMeshTallyXVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the x values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyXVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "X direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *xVec = gsl_vector_alloc(count);
  fscanf(fp, "X direction:\t%lf", &value);
  gsl_vector_set(xVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(xVec, ii, value);
  }

  fclose(fp);
  
  return xVec;

}


/******************************************************************************
 * Function: getMeshTallyYVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the y values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyYVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  fscanf(fp, "%[^\n]\n", charBuf);
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "Y direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *yVec = gsl_vector_alloc(count);
  fscanf(fp, "Y direction:\t%lf", &value);
  gsl_vector_set(yVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(yVec, ii, value);
  }

  fclose(fp);
  
  return yVec;

}


/******************************************************************************
 * Function: getMeshTallyZVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the z values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyZVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  fscanf(fp, "%[^\n]\n", charBuf);
  fscanf(fp, "%[^\n]\n", charBuf);
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "Z direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *zVec = gsl_vector_alloc(count);
  fscanf(fp, "Z direction:\t%lf", &value);
  gsl_vector_set(zVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(zVec, ii, value);
  }

  fclose(fp);
  
  return zVec;

}



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int testReadMeshTally() {

  gsl_vector *xVec =
    getMeshTallyXVector("/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *yVec =
    getMeshTallyYVector("/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *zVec =
    getMeshTallyZVector("/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  double *data = 
    getMeshTallyData("/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht",
		     xVec->size, yVec->size, zVec->size);

  int ii;
  for (ii = 0; ii < (xVec->size); ii++) {
    printf("X-Vec(%d): %g\n", ii, gsl_vector_get(xVec, ii));
  }
  for (ii = 0; ii < (yVec->size); ii++) {
    printf("Y-Vec(%d): %g\n", ii, gsl_vector_get(yVec, ii));
  }
  for (ii = 0; ii < (zVec->size); ii++) {
    printf("Z-Vec(%d): %g\n", ii, gsl_vector_get(zVec, ii));
  }

  printf("data[0]: %g\n", data[0]);

  free(data);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);

  return 0;

}
