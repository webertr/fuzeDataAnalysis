#include "mcnp/readMeshTally.h"

/******************************************************************************
 *
 * This has some functions to read a mesh tally file outpued from MCNP
 *
 ******************************************************************************/

static gsl_vector *getMeshTallyXVector(char *fileName);
static gsl_vector *getMeshTallyYVector(char *fileName);
static gsl_vector *getMeshTallyZVector(char *fileName);
static double ****getMeshTallyData(char *fileName, gsl_vector **xVec, gsl_vector **yVec,
				    gsl_vector **zVec, gsl_vector **eVec);
static int save4DData(double ****data4D, gsl_vector *xVec, gsl_vector *yVec, gsl_vector *zVec,
		      gsl_vector *eVec, char *fileName);
static double ****read4DData(char *fileName, gsl_vector **xVecIn, gsl_vector **yVecIn,
			     gsl_vector **zVecIn, gsl_vector **eVecIn);

/******************************************************************************
 * Function: save4DData
 * Inputs: double ***, char *
 * Returns: int
 * Description: Gets the data in a 4D arrray
 ******************************************************************************/

static int save4DData(double ****data4D, gsl_vector *xVec, gsl_vector *yVec, gsl_vector *zVec,
		      gsl_vector *eVec, char *fileName) {

  int xSize = xVec->size,
    ySize = yVec->size,
    zSize = zVec->size,
    eSize = eVec->size;

  FILE *fp;

  double *data = malloc(sizeof(double)*xSize*ySize*zSize*eSize);
  
  int ii, jj, kk, ll;
  for (ll = 0; ll < eSize; ll++) {
    for (kk = 0; kk < zSize; kk++) {
      for (jj = 0; jj < ySize; jj++) {
	for (ii = 0; ii < xSize; ii++) {
	  data[ii+jj*xSize+kk*xSize*ySize+ll*xSize*ySize*zSize] = data4D[ll][ii][jj][kk];
	}
      }
    }
  }

  fp = fopen(fileName, "wb");

  fwrite(&xSize, sizeof(int), 1, fp);
  fwrite(&ySize, sizeof(int), 1, fp);
  fwrite(&zSize, sizeof(int), 1, fp);
  fwrite(&eSize, sizeof(int), 1, fp);

  fwrite(xVec->data, sizeof(double), xSize, fp);
  fwrite(yVec->data, sizeof(double), ySize, fp);
  fwrite(zVec->data, sizeof(double), zSize, fp);
  fwrite(eVec->data, sizeof(double), eSize, fp);
  
  fwrite(data, sizeof(double), xSize*ySize*zSize*eSize, fp);

  fclose(fp);

  free(data);
  
  return 0;
  
}

/******************************************************************************
 * Function: read4DData
 * Inputs: int, int, int char *
 * Returns: double ***
 * Description: Gets the data in a 4D arrray
 ******************************************************************************/

static double ****read4DData(char *fileName, gsl_vector **xVecIn, gsl_vector **yVecIn,
			     gsl_vector **zVecIn, gsl_vector **eVecIn) {
  
  int ii, jj, kk, ll;
  
  FILE *fp;

  fp = fopen(fileName, "rb");

  int *xSizeRead = (int *) malloc(sizeof(int));
  int *ySizeRead = (int *) malloc(sizeof(int));
  int *zSizeRead = (int *) malloc(sizeof(int));
  int *eSizeRead = (int *) malloc(sizeof(int));

  fread(xSizeRead, sizeof(int), 1, fp);
  fread(ySizeRead, sizeof(int), 1, fp);
  fread(zSizeRead, sizeof(int), 1, fp);
  fread(eSizeRead, sizeof(int), 1, fp);

  int xSize = *xSizeRead;
  int ySize = *ySizeRead;
  int zSize = *zSizeRead;
  int eSize = *eSizeRead;

  double *dataX = (double *) malloc(sizeof(double)*xSize);
  fread(dataX, sizeof(double), xSize, fp);
  double *dataY = (double *) malloc(sizeof(double)*ySize);
  fread(dataY, sizeof(double), ySize, fp);
  double *dataZ = (double *) malloc(sizeof(double)*zSize);
  fread(dataZ, sizeof(double), zSize, fp);
  double *dataE = (double *) malloc(sizeof(double)*eSize);
  fread(dataE, sizeof(double), eSize, fp);

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
  gsl_vector *eVec = gsl_vector_alloc(eSize);
  for (ii = 0; ii < eSize; ii++) {
    gsl_vector_set(eVec, ii, dataE[ii]);
  }
  
  double *data = (double *) malloc(sizeof(double)*xSize*ySize*zSize*eSize);
  
  fread(data, sizeof(double), xSize*ySize*zSize*eSize, fp);

  fclose(fp);

  double ****data4D = (double ****) malloc(sizeof(double ***)*eSize);

  for (ll = 0; ll < eSize; ll++) {
    data4D[ll] = (double ***) malloc(sizeof(double **)*xSize);
    for (ii = 0; ii < xSize; ii++) {
      data4D[ll][ii] = (double **) malloc(sizeof(double *)*ySize);
      for (jj = 0; jj < ySize; jj++) {
	data4D[ll][ii][jj] = (double *) malloc(sizeof(double)*ySize);
	for (kk = 0; kk < zSize; kk++) {
	  data4D[ll][ii][jj][kk] = data[ii+jj*xSize+kk*xSize*ySize+ll*xSize*ySize*zSize];
	}
      }
    }
  }

  free(data);
  free(dataX);
  free(dataY);
  free(dataZ);
  free(xSizeRead);
  free(ySizeRead);
  free(zSizeRead);
  free(eSizeRead);
  
  *xVecIn = xVec;
  *yVecIn = yVec;
  *zVecIn = zVec;
  *eVecIn = eVec;
  
  return data4D;
  
}


/******************************************************************************
 * Function: getMeshTallyData
 * Inputs: char *fileName, int, int, int, gsl_vector, int, int
 * Returns: gsl_vector *
 * Description: Gets the data. ii = x iindex, jj = y-index, kk = z-index,
 * ll = Ebin index
 * data[ll][ii][jj][kk]
 ******************************************************************************/

static double ****getMeshTallyData(char *fileName, gsl_vector **xVec, gsl_vector **yVec,
				    gsl_vector **zVec, gsl_vector **eVec) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];
  double xValue, yValue, zValue, eValue, value;
  int ii, jj, kk, ll;
  int xSize, ySize, zSize, eSize;
  int charLoc = 0,
    loc;
  
  int resTest = 0;

  /* Getting x size and data*/
  while ( resTest != 1 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
    resTest = sscanf(charBuf, "X direction:\t%lf%n", &xValue, &loc);
  }
  charLoc = charLoc + loc;
  xSize = 0;
  while (resTest == 1) {
    xSize = xSize + 1;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &xValue, &loc);
    charLoc = charLoc + loc;
  }
  double *dataXTemp = (double *) malloc(sizeof(double)*xSize);
  charLoc = 0;
  resTest = sscanf(charBuf, "X direction:\t%lf%n", &xValue, &loc);
  ii = 0;
  charLoc = charLoc + loc;
  while (resTest == 1) {
    dataXTemp[ii] = xValue;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &xValue, &loc);
    charLoc = charLoc + loc;
    ii++;
  }
  xSize = xSize - 1;
  gsl_vector *xVecNew = gsl_vector_alloc(xSize);
  double *dataX = (double *) malloc(sizeof(double)*xSize);
  for (ii = 0; ii < xSize; ii++) {
    dataX[ii] = (dataXTemp[ii]+dataXTemp[ii+1])/2.0;
    gsl_vector_set(xVecNew, ii, dataX[ii]);
  }
  *xVec = xVecNew;
  free(dataXTemp);  
  
  /* Getting y size and data*/
  resTest = 0;
  charLoc = 0;
  while ( resTest != 1 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
    resTest = sscanf(charBuf, "Y direction:\t%lf%n", &yValue, &loc);
  }
  charLoc = charLoc + loc;
  ySize = 0;
  while (resTest == 1) {
    ySize = ySize + 1;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &yValue, &loc);
    charLoc = charLoc + loc;
  }
  double *dataYTemp = (double *) malloc(sizeof(double)*ySize);
  charLoc = 0;
  resTest = sscanf(charBuf, "Y direction:\t%lf%n", &yValue, &loc);
  ii = 0;
  charLoc = charLoc + loc;
  while (resTest == 1) {
    dataYTemp[ii] = yValue;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &yValue, &loc);
    charLoc = charLoc + loc;
    ii++;
  }
  ySize = ySize - 1;
  double *dataY = (double *) malloc(sizeof(double)*ySize);
  gsl_vector *yVecNew = gsl_vector_alloc(ySize);
  for (ii = 0; ii < ySize; ii++) {
    dataY[ii] = (dataYTemp[ii]+dataYTemp[ii+1])/2.0;
    gsl_vector_set(yVecNew, ii, dataY[ii]);
  }
  *yVec = yVecNew;  
  free(dataYTemp);
  
  /* Getting z size and data */
  resTest = 0;
  charLoc = 0;
  while ( resTest != 1 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
    resTest = sscanf(charBuf, "Z direction:\t%lf%n", &zValue, &loc);
  }
  charLoc = charLoc + loc;
  zSize = 0;
  while (resTest == 1) {
    zSize = zSize + 1;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &zValue, &loc);
    charLoc = charLoc + loc;
  }
  double *dataZTemp = (double *) malloc(sizeof(double)*zSize);
  charLoc = 0;
  resTest = sscanf(charBuf, "Z direction:\t%lf%n", &yValue, &loc);
  ii = 0;
  charLoc = charLoc + loc;
  while (resTest == 1) {
    dataZTemp[ii] = zValue;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &zValue, &loc);
    charLoc = charLoc + loc;
    ii++;
  }
  zSize = zSize - 1;
  double *dataZ = (double *) malloc(sizeof(double)*zSize);
  gsl_vector *zVecNew = gsl_vector_alloc(zSize);
  for (ii = 0; ii < zSize; ii++) {
    dataZ[ii] = (dataZTemp[ii]+dataZTemp[ii+1])/2.0;
    gsl_vector_set(zVecNew, ii, dataZ[ii]);
  }
  *zVec = zVecNew;
  free(dataZTemp);

  /* Getting e size and values  */
  resTest = 0;
  charLoc = 0;
  while ( resTest != 1 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
    resTest = sscanf(charBuf, "Energy bin boundaries:\t%lf%n", &eValue, &loc);
  }
  charLoc = charLoc + loc;
  eSize = 0;
  while (resTest == 1) {
    eSize = eSize + 1;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &eValue, &loc);
    charLoc = charLoc + loc;
  }
  double *dataETemp = (double *) malloc(sizeof(double)*eSize);
  charLoc = 0;
  resTest = sscanf(charBuf, "Energy bin boundaries:\t%lf%n", &eValue, &loc);
  ii = 0;
  charLoc = charLoc + loc;
  while (resTest == 1) {
    dataETemp[ii] = eValue;
    resTest = sscanf(&charBuf[charLoc], "%lf%n", &eValue, &loc);
    charLoc = charLoc + loc;
    ii++;
  }
  eSize = eSize - 1;
  double *dataE = (double *) malloc(sizeof(double)*eSize);
  gsl_vector *eVecNew = gsl_vector_alloc(eSize);
  for (ii = 0; ii < eSize; ii++) {
    dataE[ii] = (dataETemp[ii]+dataETemp[ii+1])/2.0;
    gsl_vector_set(eVecNew, ii, dataE[ii]);
  }
  free(dataETemp);
  *eVec = eVecNew;     
  
  /* data4D[ll][ii][jj][kk] ll = energy, ii = x, jj = y, kk = z */
  double ****data4D = (double ****) malloc(sizeof(double ***)*eSize);

  /* Setting up 4D array */
  for (ll = 0; ll < eSize; ll++) {
    data4D[ll] = (double ***) malloc(sizeof(double **)*xSize);
    for (ii = 0; ii < xSize; ii++) {
      data4D[ll][ii] = (double **) malloc(sizeof(double *)*ySize);
      for (jj = 0; jj < ySize; jj++) {
	data4D[ll][ii][jj] = (double *) malloc(sizeof(double)*zSize);
      }
    }
  }
  

  /* Iterating through energy bins */
  for (ll = 0; ll < eSize; ll++) {
    
    /* Iterating through Z-values */
    for (kk = 0; kk < zSize; kk++) {

      while ( strcmp(charBuf, "Tally Results:  X (across) by Y (down)") != 0 ) {
	fscanf(fp, "%[^\n]\n", charBuf);
      }
      fscanf(fp, "%[^\n]\n", charBuf);
      
      /* Iterating through y-values */
      for (jj = 0; jj < ySize; jj++) {

	/* Getting y value */
	fscanf(fp, "%lf", &yValue);
	
	/* Getting data values */
	for (ii = 0; ii < xSize; ii++) {
	  fscanf(fp, "%lf", &value);
	  data4D[ll][ii][jj][kk] = value;
	}
      }

      /* Dumping the Relative Errors */
      fgets(charBuf, BUF_SIZE, fp);
      fgets(charBuf, BUF_SIZE, fp);
      /* If you want this data, just set it here instead of throwing it away */
      for (jj = 0; jj < ySize ; jj++) {
	fgets(charBuf, BUF_SIZE, fp);
      }
    }
  }
  
  return data4D;

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

  int ii, jj, kk, ll;
  gsl_vector *xVec;
  gsl_vector *yVec;
  gsl_vector *zVec;
  gsl_vector *eVec;
  char *fileName = "/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";

  double ****data = getMeshTallyData(fileName, &xVec, &yVec, &zVec, &eVec);

  ii = 2;
  jj = 1;
  kk = 1;
  ll = 0;

  printf("data(1.30282E-06): %g\n", data[ll][ii][jj][kk]);
  printf("data(1.23122E-06): %g\n", data[0][0][0][0]);
  printf("X-Vector(31.5): %g\n", gsl_vector_get(xVec, 10));
  printf("X-Vector Size (264): %d\n", (int) xVec->size);
  printf("Y-Vector(31.5): %g\n", gsl_vector_get(yVec, 10));
  printf("Y-Vector Size (255): %d\n", (int) yVec->size);
  printf("Z-Vector(31.5): %g\n", gsl_vector_get(zVec, 10));
  printf("Z-Vector Size (148): %d\n", (int) zVec->size);
  printf("E-Vector(0.25): %g\n", gsl_vector_get(eVec, 0));
  printf("E-Vector Size (2): %d\n", (int) eVec->size);

  char *fileNameTest = "/home/webertr/Github/fuzeDataAnalysis/data/temp.dat";
  save4DData(data, xVec, yVec, zVec, eVec, fileNameTest);

  printf("data(1.30282E-06): %g\n", data[ll][ii][jj][kk]);
  printf("data(1.23122E-06): %g\n", data[0][0][0][0]);
  printf("X-Vector(31.5): %g\n", gsl_vector_get(xVec, 10));
  printf("X-Vector Size (264): %d\n", (int) xVec->size);
  printf("Y-Vector(31.5): %g\n", gsl_vector_get(yVec, 10));
  printf("Y-Vector Size (255): %d\n", (int) yVec->size);
  printf("Z-Vector(31.5): %g\n", gsl_vector_get(zVec, 10));
  printf("Z-Vector Size (148): %d\n", (int) zVec->size);
  printf("E-Vector(0.25): %g\n", gsl_vector_get(eVec, 0));
  printf("E-Vector Size (2): %d\n", (int) eVec->size);
  
  
  gsl_vector *xVecRead;
  gsl_vector *yVecRead;
  gsl_vector *zVecRead;
  gsl_vector *eVecRead;
  
  double ****dataRead = read4DData(fileNameTest,&xVecRead,&yVecRead,&zVecRead,&eVecRead);

  printf("Read Back data(1.30282E-06): %g\n", dataRead[ll][ii][jj][kk]);
  printf("Read Back data(1.23122E-06): %g\n", dataRead[0][0][0][0]);
  printf("Read Back X-Vec(31.5): %g\n", gsl_vector_get(xVecRead, 10));
  printf("Read Back X-Vec Size (264): %d\n", (int) xVecRead->size);
  printf("Read Back Y-Vec(31.5): %g\n", gsl_vector_get(yVecRead, 10));
  printf("Read Back Y-Vec Size (255): %d\n", (int) yVecRead->size);
  printf("Read Back Z-Vec(31.5): %g\n", gsl_vector_get(zVecRead, 10));
  printf("Read Back Z-Vec Size (148): %d\n", (int) zVecRead->size);
  printf("Read Back E-Vec(0.25): %g\n", gsl_vector_get(eVecRead, 0));
  printf("Read Back E-Vec Size (2): %d\n", (int) eVecRead->size);

  free(dataRead);
  free(data);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);
  gsl_vector_free(eVec);
  gsl_vector_free(xVecRead);
  gsl_vector_free(yVecRead);
  gsl_vector_free(zVecRead);
  gsl_vector_free(eVecRead);
  
  exit(0);

  if (0) {

    char *fileNameTemp = "/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";
    getMeshTallyXVector(fileNameTemp);
    getMeshTallyYVector(fileNameTemp);
    getMeshTallyZVector(fileNameTemp);
    
  }
    
  /*
  if (0) {
    
    char *fileName = "/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";

    ii = 2;
    jj = 1;
    kk = 1;


    char *fileNameTest = "/home/fuze/Github/fuzeDataAnalysis/data/temp.dat";
    double ***dataTest = getMeshTally3DData(fileName, &xVec, &yVec, &zVec);
    save3DData(dataTest, xVec, yVec, zVec, fileNameTest);

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

    double ***dataTestRead = read3DData(fileNameTest,&xVecRead,&yVecRead,&zVecRead);

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

  }
  */
  /*
  //char *fileNameFull = "/home/webertr/webertrNAS/mcnpOutputFiles/brianFull/meshTalFull";
  //char *fileNameVoid = "/home/webertr/webertrNAS/mcnpOutputFiles/brianVoid/meshTalVoid";

  char *fileNameFull = "/home/fuze/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";
  char *fileNameVoid = "/home/fuze/webertrNAS/mcnpOutputFiles/9_16_Void/inpFullMeshVoidmsht";
  
  double ***dataFull = getMeshTally3DData(fileNameFull, &xVec, &yVec, &zVec);
  double ***dataVoid = getMeshTally3DData(fileNameVoid, &xVec, &yVec, &zVec);

  ii = 200;
  jj = 150;
  kk = 75;
  printf("Full: %g\n", dataFull[ii][jj][kk]);
  printf("Void: %g\n", dataVoid[ii][jj][kk]);
  double hfactor = dataFull[ii][jj][kk]/dataVoid[ii][jj][kk];

  printf("H-factor: %g\n", hfactor);
  
  free(dataFull);
  free(dataVoid);
  */
  
  return 0;

}
