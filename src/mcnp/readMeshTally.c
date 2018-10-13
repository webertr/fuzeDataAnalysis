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
static double ***getMeshTally3DData(char *fileName);
static int save3DData(double ***data3D, gsl_vector *xVec, gsl_vector *yVec, gsl_vector *zVec,
		      char *fileName);
static double ***read3DData(char *fileName, gsl_vector **xVecIn, gsl_vector **yVecIn,
			    gsl_vector **zVecIn);


/******************************************************************************
 * Function: save3DData
 * Inputs: double ***, char *
 * Returns: int
 * Description: Gets the data in a 3D arrray
 ******************************************************************************/

static int save3DData(double ***data3D, gsl_vector *xVec, gsl_vector *yVec, gsl_vector *zVec,
		      char *fileName) {

  int xSize = xVec->size,
    ySize = yVec->size,
    zSize = zVec->size;

  FILE *fp;

  double *data = malloc(sizeof(double)*xSize*ySize*zSize);
  
  int ii, jj, kk;
  for (ii = 0; ii < xSize; ii++) {
    for (jj = 0; jj < ySize; jj++) {
      for (kk = 0; kk < zSize; kk++) {
	data[ii+jj*xSize+kk*xSize*ySize] = data3D[ii][jj][kk];
      }
    }
  }

  fp = fopen(fileName, "wb");

  fwrite(&xSize, sizeof(int), 1, fp);
  fwrite(&ySize, sizeof(int), 1, fp);
  fwrite(&zSize, sizeof(int), 1, fp);

  fwrite(xVec->data, sizeof(double), xSize, fp);
  fwrite(yVec->data, sizeof(double), ySize, fp);
  fwrite(zVec->data, sizeof(double), zSize, fp);
  
  fwrite(data, sizeof(double), xSize*ySize*zSize, fp);

  fclose(fp);

  free(data);
  
  return 0;
  
}

/******************************************************************************
 * Function: read3DData
 * Inputs: int, int, int char *
 * Returns: double ***
 * Description: Gets the data in a 3D arrray
 ******************************************************************************/

static double ***read3DData(char *fileName, gsl_vector **xVecIn, gsl_vector **yVecIn,
			    gsl_vector **zVecIn) {

  int ii, jj, kk;
  
  FILE *fp;

  fp = fopen(fileName, "rb");

  int *xSizeRead = (int *) malloc(sizeof(int));
  int *ySizeRead = (int *) malloc(sizeof(int));
  int *zSizeRead = (int *) malloc(sizeof(int));

  fread(xSizeRead, sizeof(int), 1, fp);
  fread(ySizeRead, sizeof(int), 1, fp);
  fread(zSizeRead, sizeof(int), 1, fp);

  int xSize = *xSizeRead;
  int ySize = *ySizeRead;
  int zSize = *zSizeRead;

  double *dataX = (double *) malloc(sizeof(double)*xSize);
  fread(dataX, sizeof(double), xSize, fp);
  double *dataY = (double *) malloc(sizeof(double)*ySize);
  fread(dataY, sizeof(double), ySize, fp);
  double *dataZ = (double *) malloc(sizeof(double)*zSize);
  fread(dataZ, sizeof(double), zSize, fp);

  gsl_vector *xVec = gsl_vector_alloc(xSize);
  for (ii = 0; ii < xSize; ii++) {
    gsl_vector_set(xVec, ii, dataX[ii]);
  }
  gsl_vector *yVec = gsl_vector_alloc(ySize);
  for (ii = 0; ii < ySize; ii++) {
    gsl_vector_set(yVec, ii, dataY[ii]);
  }
  gsl_vector *zVec = gsl_vector_alloc(zSize);
  for (ii = 0; ii < zSize; ii++) {
    gsl_vector_set(zVec, ii, dataZ[ii]);
  }
  
  double *data = (double *) malloc(sizeof(double)*xSize*ySize*zSize);
  
  fread(data, sizeof(double), xSize*ySize*zSize, fp);

  fclose(fp);

  double ***data3D = (double ***) malloc(sizeof(double **)*xSize);

  for (ii = 0; ii < xSize; ii++) {
    data3D[ii] = (double **) malloc(sizeof(double *)*ySize);
    for (jj = 0; jj < ySize; jj++) {
      data3D[ii][jj] = (double *) malloc(sizeof(double)*zSize);
      for (kk = 0; kk < zSize; kk++) {
	data3D[ii][jj][kk] = data[ii+jj*xSize+kk*xSize*ySize];
      }
    }
  }

  free(data);
  free(dataX);
  free(dataY);
  free(dataZ);

  *xVecIn = xVec;
  *yVecIn = yVec;
  *zVecIn = zVec;
  
  return data3D;
  
}

/******************************************************************************
 * Function: getMeshTally3DData
 * Inputs: char *fileName, int, int, int, gsl_vector, int, t
 * Returns: double ***
 * Description: Gets the data in a 3D arrray
 ******************************************************************************/

static double ***getMeshTally3DData(char *fileName) {

  gsl_vector *xVec =
    getMeshTallyXVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *yVec =
    getMeshTallyYVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *zVec =
    getMeshTallyZVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");

  int xSize = xVec->size;
  int ySize = yVec->size;
  int zSize = zVec->size;
  
  double *data = 
    getMeshTallyData("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht",
		     xSize, ySize, zSize);

  double ***data3D = (double ***) malloc(sizeof(double **)*xSize);

  int ii, jj, kk;
  for (ii = 0; ii < xSize; ii++) {
    data3D[ii] = (double **) malloc(sizeof(double *)*ySize);
    for (jj = 0; jj < ySize; jj++) {
      data3D[ii][jj] = (double *) malloc(sizeof(double)*zSize);
      for (kk = 0; kk < zSize; kk++) {
	data3D[ii][jj][kk] = data[ii+jj*xSize+kk*xSize*ySize];
      }
    }
  }

  free(data);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);

  return data3D;
  
}

/******************************************************************************
 * Function: getMeshTallyData
 * Inputs: char *fileName, int, int, int, gsl_vector, int, int
 * Returns: gsl_vector *
 * Description: Gets the data. ii = x iindex, jj = y-index, kk = z-index
 * data(ii + jj*xSize + kk*xSize*ySize)
 ******************************************************************************/

static double *getMeshTallyData(char *fileName, int xSize, int ySize, int zSize) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  /* data(ii + jj*xSize + kk*xSize*ySize) */
  double *data = malloc(sizeof(double)*xSize*ySize*zSize);
  double zMin, zMax, xValue, yValue, value;
  int ii, jj, kk;
  int resTest = 0;

  /* Iterating through Z-values */
  for (kk = 0; kk < (zSize-1); kk++) {
    
    resTest = 0;
    while ( resTest != 2 ) {
      fscanf(fp, "%[^\n]\n", charBuf);
      resTest = sscanf(charBuf, "Z bin: %lf - %lf", &zMin, &zMax);
    }

    resTest = 0;
    fscanf(fp, "%[^\n]\n", charBuf);
    while ( strcmp(charBuf, "Tally Results:  X (across) by Y (down)") != 0 ) {
      fscanf(fp, "%[^\n]\n", charBuf);
    }

    /* Geting x values */
    for (ii = 0; ii < (xSize-1); ii++) {
      fscanf(fp, "%lf", &xValue);
    }

    /* Iterating through y-values */
    for (jj = 0; jj < (ySize-1); jj++) {

      /* Getting y value */
      fscanf(fp, "%lf", &yValue);

      /* Getting data value */
      for (ii = 0; ii < (xSize-1); ii++) {
	fscanf(fp, "%lf", &value);
	data[ii + jj*xSize + kk*xSize*ySize] = value;
      }
    }

    /* Dumping the Relative Errors */
    fgets(charBuf, BUF_SIZE, fp);
    fgets(charBuf, BUF_SIZE, fp);
    /* If you want this data, just set it here instead of throwing it away */
    for (jj = 0; jj < (ySize - 1); jj++) {
      fgets(charBuf, BUF_SIZE, fp);
    }
  }
  
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
      
  char *fileName = "/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";
  int ii, jj, kk;
  ii = 2;
  jj = 1;
  kk = 1;

  gsl_vector *xVec = getMeshTallyXVector(fileName);
  gsl_vector *yVec = getMeshTallyYVector(fileName);
  gsl_vector *zVec = getMeshTallyZVector(fileName);

  double ***dataTest = getMeshTally3DData(fileName);
  save3DData(dataTest, xVec, yVec, zVec, "/home/webertr/Github/fuzeDataAnalysis/data/temp.dat");
  
  printf("data(1.30282E-06): %g\n", dataTest[ii][jj][kk]);
  printf("X-Vector(30): %g\n", gsl_vector_get(xVec, 10));
  printf("X-Vector Size (265): %d\n", (int) xVec->size);
  printf("Y-Vector(30): %g\n", gsl_vector_get(yVec, 10));
  printf("Y-Vector Size (256): %d\n", (int) yVec->size);
  printf("Z-Vector(30): %g\n", gsl_vector_get(zVec, 10));
  printf("Z-Vector Size (149): %d\n", (int) zVec->size);

  gsl_vector *xVecRead;
  gsl_vector *yVecRead;
  gsl_vector *zVecRead;

  double ***dataTestRead =
    read3DData("/home/webertr/Github/fuzeDataAnalysis/data/temp.dat",&xVecRead,&yVecRead,&zVecRead);

  printf("data[0](1.30282E-06): %g\n", dataTestRead[ii][jj][kk]);
  printf("X-Vector(30): %g\n", gsl_vector_get(xVecRead, 10));
  printf("X-Vector Size (265): %d\n", (int) xVecRead->size);
  printf("Y-Vector(30): %g\n", gsl_vector_get(yVecRead, 10));
  printf("Y-Vector Size (256): %d\n", (int) yVecRead->size);
  printf("Z-Vector(30): %g\n", gsl_vector_get(zVecRead, 10));
  printf("Z-Vector Size (149): %d\n", (int) zVecRead->size);  

  free(dataTestRead);
  free(dataTest);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);
  gsl_vector_free(xVecRead);
  gsl_vector_free(yVecRead);
  gsl_vector_free(zVecRead);

  char *fileNameFull = "/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";
  char *fileNameVoid = "/home/webertr/webertrNAS/mcnpOutputFiles/9_16_Void/inpFullMeshVoidmsht";
  
  double ***dataFull = getMeshTally3DData(fileNameFull);
  double ***dataVoid = getMeshTally3DData(fileNameVoid);

  ii = 200;
  jj = 150;
  kk = 75;
  printf("Full: %g\n", dataFull[ii][jj][kk]);
  printf("Void: %g\n", dataVoid[ii][jj][kk]);
  double hfactor = dataFull[ii][jj][kk]/dataVoid[ii][jj][kk];

  printf("H-factor: %g\n", hfactor);
  
  free(dataFull);
  free(dataVoid);
  
  return 0;

}
