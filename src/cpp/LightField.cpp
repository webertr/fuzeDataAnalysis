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

  /* Smooth line by applying an FFT */
  smoothedLine = smoothVector(binnedLine, 4);

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

gsl_vector *LightField::smoothVector(gsl_vector *vecIn, int maxFFTCutoff) {

  int vecSize = vecIn->size;
  int ii;

  gsl_vector *retVec = gsl_vector_alloc(vecSize);
  size_t *sortIndex = (size_t *)malloc(sizeof(size_t) * vecSize);

  gsl_fft_real_wavetable * real;
  gsl_fft_halfcomplex_wavetable * hc;
  gsl_fft_real_workspace * work;

  work = gsl_fft_real_workspace_alloc(vecSize);
  real = gsl_fft_real_wavetable_alloc(vecSize);

  for (ii = 0; ii < vecSize; ii++) {
    gsl_vector_set(retVec, ii, gsl_vector_get(vecIn, ii));
  }

  gsl_fft_real_transform (retVec->data, 1, vecSize, real, work);

  /* Sorting elements from smallest to largest */
  gsl_sort_index(sortIndex, retVec->data, 1, vecSize);

  /* Zeroing all elements not in the maxFFTCutoff largest */
  for (ii = 0; (ii + maxFFTCutoff) < vecSize; ii++) {
    std::cout << "ii: " << ii << "\n";
    gsl_vector_set(retVec, sortIndex[ii], 0);
  }

  gsl_fft_real_wavetable_free(real);

  hc = gsl_fft_halfcomplex_wavetable_alloc(vecSize);

  gsl_fft_halfcomplex_inverse(retVec->data, 1, vecSize, hc, work);
  gsl_fft_halfcomplex_wavetable_free(hc);

  gsl_fft_real_workspace_free(work);

  free(sortIndex);

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
  //test.plotImage();

  //plotVectorData(test.waveLength, test.binnedLine, 
  //		 "", "", "data/splTest.dat", "data/splTest.sh");

  std::cout << "Wavlength size: " << test.waveLength->size << "\n";
  std::cout << "Binned line size: " << test.binnedLine->size << "\n";
  std::cout << "Smoothed line size: " << test.smoothedLine->size << "\n";
  plot2VectorData(test.waveLength, test.binnedLine, "title 'data'", 
		  test.smoothedLine, "with line lw 3 title 'model'", 
		  "", "data/temp.txt", "data/temp.sh");

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
