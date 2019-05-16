#include "cpp/LightField.h"


/******************************************************************************
 * Function: LightField
 * Inputs: 
 * Returns: 
 * Description: Default constructor
 ******************************************************************************/

LightField::LightField(std::string fileNameParam):
  xdim(0),
  ydim(0),
  frameNum(0),
  imageSize(0),
  pixelType(0),
  waveLength(0),
  image(0)
 {

  fileName = fileNameParam;

  FILE *fp;
  FILE *fXML;
  size_t fb;
  size_t fbXML;

  /* Image and wavelength data */
  float *data,
    *wvData;

  /* Offset for xml file (64 bit)*/
  unsigned int xmlOffset;

  char xmlFileBuffer[MAX_BUFFER];

  /* 
   * Opening the file
   */
  fp = fopen(fileName.c_str(), "r");

  /*
   * Pulling the 42th byte to get the width of the frame in pixels
   */
  fseek(fp, 42, SEEK_SET);
  fb = fread((void *) &xdim, (size_t) 2 , (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for frame width\n");
    fclose(fp);
    exit(1);
    
  }

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);
  
  /*
   * Pulling the 656th byte to get the height of the frame in pixels
   */
  fseek(fp, 656, SEEK_SET);
  fb = fread((void *) &ydim, (size_t) 2, (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for frame height\n");
    fclose(fp);
    exit(1);
    
  }

  imageSize = xdim*ydim;

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /*
   * Pulling the 1446th byte to get the number of the frames
   */
  fseek(fp, 1446, SEEK_SET);
  fb = fread((void *) &frameNum, (size_t) 4, (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for frame number\n");
    fclose(fp);
    exit(1);

  }

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /*
   * Pulling the 108th byte to get the pixel type. 0 means 32f = 32 bit floating type
   */
  fseek(fp, 108, SEEK_SET);
  fb = fread((void *) &pixelType, (size_t) 2, (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for pixel type\n");
    fclose(fp);
    exit(1);
    
  }

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);
  
  /*
   * Pulling the 410th byte to get the binary image.
   */
  data = (float *)malloc(xdim * ydim * sizeof(float));
  fseek(fp, 4100, SEEK_SET);
  fb = fread((void *) data, (size_t) 4, (size_t) (imageSize), fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != (size_t) (imageSize) ) {

    free(data);
    printf("Error reading SPE file for image:\n");
    fclose(fp);
    exit(1);

  }

  /*
   * Matrix to return
   * xdim = number of columns
   * ydim = number of rows
   */
  image = gsl_matrix_alloc(ydim, xdim);

  /*
   * Moving data over to matrix
   */
  int ii, jj;
  for (ii = 0; ii < xdim; ii ++) {

    for (jj = 0; jj < ydim; jj++) {

      gsl_matrix_set(image, jj, ii, (double) data[xdim*jj+ii]);
      
    }

  }

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /*
   * Pulling the 678th byte to get the
   * xml footer offset
   */
  fseek(fp, 678, SEEK_SET);
  fb = fread((void *) &xmlOffset, (size_t) 4, (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for XML Offset:\n");
    fclose(fp);
    exit(1);
    
  }

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /* Get end of file */
  fseek(fp, 0L, SEEK_END);
  int sz = ftell(fp);
  
  std::string xmlTempFile = "data/xmlTemp.xml";
  /* Removing file in case it exists */
  remove(xmlTempFile.c_str());

  /*
   * Set fp to start of the xml file and read xml file
   */
  fXML = fopen(xmlTempFile.c_str(), "w");

  fseek(fp, xmlOffset, SEEK_SET);

  int xmlOffsetTemp = xmlOffset;

  while ( ( sz - xmlOffsetTemp ) > MAX_BUFFER ) {

    fb = fread((void *) &xmlFileBuffer, (size_t) 1, (size_t) MAX_BUFFER, fp);
    fbXML = fwrite((const void *) xmlFileBuffer, (size_t) 1, MAX_BUFFER, fXML);
    xmlOffsetTemp = xmlOffsetTemp + MAX_BUFFER;
    
  }
  
  fb = fread((void *) &xmlFileBuffer, (size_t) 1, (size_t) (sz - xmlOffsetTemp), fp);
  fbXML = fwrite((const void *) xmlFileBuffer, (size_t) 1, (sz - xmlOffsetTemp), fXML);

  /*
   * Verifying the write occurred correctly
   */
  if (fbXML != (size_t) (sz-xmlOffsetTemp) ) {

    printf("Error writing xml file:\n");
    fclose(fXML);
    fclose(fp);
    exit(1);
    
  }

  fclose(fXML);
  
  /* Parsing xml file */
  wvData = (float *) malloc(xdim*sizeof(float));
  xmlParserSPE(xmlTempFile, wvData, xdim);

  remove(xmlTempFile.c_str());

  /*
   * Vector contain wavelengths
   */
  waveLength = gsl_vector_alloc(xdim);

  for (ii = 0; ii < xdim; ii++) {

    gsl_vector_set(waveLength, ii, (double) wvData[ii]);
      
  }
  
  free(wvData);

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /* Closing file */
  fclose(fp);

  /* free data */
  free(data);

  /* Setting the index of the brightess line */
  maxLineIndex = getColMax();

  /* Setting the index of the brightess line */
  binnedLine = getBinnedCol(maxLineIndex, 5);

  /* Getting the fiber centers */
  //fiberCenters = smoothVector(binnedLine);

  /*
   * Returning Success
   */
  return;

}


/******************************************************************************
 * Function: LightField
 * Inputs: 
 * Returns: 
 * Description: Default destructor
 ******************************************************************************/

LightField::~LightField() {

  gsl_vector_free(waveLength);
  gsl_matrix_free(image);

  return;

}


/******************************************************************************
 * Function: xmlParserSPE
 * Inputs: char *, float *, in
 * Returns: int
 * Description: Parses xml file
 ******************************************************************************/

bool LightField::xmlParserSPE(std::string fileName, float *dataArray, int dim) {
  
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *calibration, *wavelengthMapping, *wavelength;
  xmlChar *data;
    
  document = xmlReadFile(fileName.c_str(), NULL, 0);
  root = xmlDocGetRootElement(document);
  first_child = root->children;

  int ii = 0;
  for (node = first_child; node; node = node->next) {
      
    if ( ii == 1) {
      
      calibration = node;
      wavelengthMapping = calibration->children;
      wavelength = wavelengthMapping->children;
      
    }
    ii = ii + 1;
    
  }
  
  data = xmlNodeListGetString(document, wavelength->xmlChildrenNode, 1);
  
  char endC = ',';
  char *endCP = &endC;
  char *location;
  dataArray[0] = strtof( (char *) data, &endCP);
  location = strchr((char *) data, ',');
  for ( ii = 1; ii < dim; ii++) {
      
    dataArray[ii] = strtof( &location[1], &endCP);
    location = strchr(&location[1], ',');
      
  }
  
  /* Free xml document */
  xmlFreeDoc(document);
  
  return true;

}


/******************************************************************************
 * Function: plotImage
 * Inputs: 
 * Returns: 
 * Description: Plots image
 ******************************************************************************/

int LightField::plotImage() {

  plotImageData(image, 1, 1, "", "data/plotImageLF.dat", "data/plotImageLF.sh");

  return 0;

}


/******************************************************************************
 * Function: getColMax
 * Inputs: 
 * Returns: 
 * Description: This will go through the image and find the maximum value
 * for the sum of each column. This will be identified as the primary
 * line to use
 ******************************************************************************/

int LightField::getColMax() {

  int ii, jj;

  gsl_vector *sumCol = gsl_vector_alloc(xdim);
  double sum;

  for (jj = 0; jj < xdim; jj++) {
    sum = 0;
    for (ii = 0; ii < ydim; ii++) {
      sum = sum + gsl_matrix_get(image, ii, jj);
    }
    gsl_vector_set(sumCol, jj, sum);
  }

  int maxIndex = gsl_vector_max_index(sumCol);

  gsl_vector_free(sumCol);

  return maxIndex;

}


/******************************************************************************
 * Function: getBinnedCol
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will go through the image at the specified column
 * and bin up by +/- the specified pixels
 ******************************************************************************/

gsl_vector *LightField::getBinnedCol(int colIndex, int binNum) {

  int ii, jj;
  double sum;

  gsl_vector *retVec = gsl_vector_alloc(ydim);
  
  for (ii = 0; ii < ydim; ii++) {
    sum = 0;
    for (jj = -binNum; jj <= binNum; jj++) {
      sum = sum + gsl_matrix_get(image, ii, colIndex+jj);
    }
    gsl_vector_set(retVec, ii, sum);
  }

  return retVec;

}


/******************************************************************************
 * Function: smoothVector
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will determine the local maxima of the passed vectors
 ******************************************************************************/

gsl_vector *LightField::smoothVector(gsl_vector *vecIn) {

  const int vecSize = vecIn->size;
  const int N_COEFFS = 12;
  const int K = 4;                        // k = 4 for cubic spline
  const int N_BREAK = N_COEFFS + 2 - K;   // nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4
  int ii, jj;
  gsl_bspline_workspace *bw;
  gsl_vector *B;
  gsl_rng *r;
  gsl_vector *c, *w;
  gsl_vector *x, *y;
  gsl_matrix *X, *cov;
  gsl_multifit_linear_workspace *mw;
  double chisq, Rsq, dof, tss;

  gsl_vector *retVec = gsl_vector_alloc(vecSize);
  gsl_vector *xVec = gsl_vector_alloc(vecSize);

  for (ii = 0; ii < vecSize; ii++) {
    gsl_vector_set(xVec, ii, ii);
  }

  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);

  /* allocate a cubic bspline workspace (k = 4) */
  bw = gsl_bspline_alloc(K, N_BREAK);
  B = gsl_vector_alloc(N_COEFFS);

  x = gsl_vector_alloc(vecSize);
  y = gsl_vector_alloc(vecSize);
  X = gsl_matrix_alloc(vecSize, N_COEFFS);
  c = gsl_vector_alloc(N_COEFFS);
  w = gsl_vector_alloc(vecSize);
  cov = gsl_matrix_alloc(N_COEFFS, N_COEFFS);
  mw = gsl_multifit_linear_alloc(vecSize, N_COEFFS);

  /* this is the data to be fitted */
  for (ii = 0; ii < vecSize; ii++) {
    gsl_vector_set(x, ii, gsl_vector_get(xVec, ii));
    gsl_vector_set(y, ii, gsl_vector_get(vecIn, ii));
    gsl_vector_set(w, ii, 1.0);
  }

  /* use uniform breakpoints on [0, vecSize] */
  gsl_bspline_knots_uniform(0.0, vecSize, bw);

  /* construct the fit matrix X */
  for (ii = 0; ii < vecSize; ++ii) {
   
    double xi = gsl_vector_get(x, ii);

    /* compute B_j(xi) for all j */
    gsl_bspline_eval(xi, B, bw);

    /* fill in row i of X */
    for (jj = 0; jj < N_COEFFS; ++jj) {
      double Bj = gsl_vector_get(B, jj);
      gsl_matrix_set(X, ii, jj, Bj);
    }
  }

  /* do the fit */
  gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);

  dof = vecSize - N_COEFFS;
  tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
  Rsq = 1.0 - chisq / tss;

  fprintf(stderr, "chisq/dof = %e, Rsq = %f\n",
                   chisq / dof, Rsq);

  printf("\n\n");

  /* output the smoothed curve */
  double yi, yerr;

  for (ii =0; ii < vecSize; ++ii) {
    gsl_bspline_eval(ii, B, bw);
    gsl_multifit_linear_est(B, c, cov, &yi, &yerr);
    gsl_vector_set(retVec, ii, yi);
  }

  gsl_rng_free(r);
  gsl_bspline_free(bw);
  gsl_vector_free(B);
  gsl_vector_free(x);
  gsl_vector_free(y);
  gsl_matrix_free(X);
  gsl_vector_free(c);
  gsl_vector_free(w);
  gsl_matrix_free(cov);
  gsl_multifit_linear_free(mw);


  plot2VectorData(xVec, vecIn, "title 'data'", retVec, "title 'model'", 
		  "", "data/temp.txt", "data/temp.sh");

  gsl_vector_free(xVec);
 
  return retVec;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testClassCreation();
static bool testFindColMax();

bool testLightField() {

  if( !testClassCreation() ) {
    std::cout << "Failed Class Creation\n";
    return false;
  }

  if( !testFindColMax() ) {
    std::cout << "Failed Find Column Max\n";
    return false;
  }

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");
  test.plotImage();

  plotVectorData(test.waveLength, test.binnedLine, 
		 "", "", "data/splTest.dat", "data/splTest.sh");

  std::cout << "Passed All Light Field Tests\n";

  return true;

}


static bool testClassCreation() {

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  return true;

}


static bool testFindColMax() {

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  int colMax = test.maxLineIndex;

  if (colMax != 462) {
    return false;
  }

  return true;

}
