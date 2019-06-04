#include "cpp/LightField.h"


/******************************************************************************
 * Function: LightField
 * Inputs: 
 * Returns: 
 * Description: Default constructor
 ******************************************************************************/

LightField::LightField(std::string fileNameParam):
  pixelType(0),
  chordsOK(false),
  xdim(0),
  ydim(0),
  frameNum(0),
  imageSize(0),
  waveLength(0),
  image(0),
  imageUShort(0) {
   
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

  /* Checking if file exists */
  if (!fp) {
    throw std::invalid_argument("File " + fileNameParam + " does not exists\n");
    //throw std::invalid_argument("File does not exists\n");
    return;
  }
    
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
  imageUShort = gsl_matrix_ushort_alloc(ydim, xdim);

  /*
   * Moving data over to matrix
   */
  int ii, jj;
  for (ii = 0; ii < xdim; ii ++) {
    for (jj = 0; jj < ydim; jj++) {

      gsl_matrix_set(image, jj, ii, (double) data[xdim*jj+ii]);
      
    }
  }

  /* Converting to a ushort matrix */
  double minVal = gsl_matrix_min(image);
  if (minVal > 0) {
    minVal = 0;
  }
  for (ii = 0; ii < xdim; ii++) {
    for (jj = 0; jj < ydim; jj++) {
      gsl_matrix_ushort_set(imageUShort, ii, jj,
			    gsl_matrix_get(image, ii, jj) - minVal);
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

  /* Initiliazing the row vector 0 -> num of Rows */
  rows = gsl_vector_alloc(ydim);
  for (ii = 0; ii < ydim; ii++) {
    gsl_vector_set(rows, ii, ii);
  }

  /* Setting the index of the brightess line */
  maxLineIndex = getColMax();

  /* Getting a binned vector to find the fiber locations */
  //binnedLine = getColWithRowSum();
  binnedLine = getBinnedCol(maxLineIndex, 5);

  /* Smooth line by applying an FFT */
  smoothedLine = smoothVector(binnedLine, 4);

  /* Getting the peaks of the smoothed line */
  fiberCenters = getMaxima(smoothedLine);

  /* Setting total number of fibers */
  numFibers = fiberCenters->size;

  /* Finding fiber boundaries */
  fiberEdges = getMinima(smoothedLine);

  /* Setting total number of fiber boundaries */
  numEdges = fiberEdges->size;

  /* Allocating all 20 chords */
  chord1 = gsl_vector_alloc(xdim);
  chord2 = gsl_vector_alloc(xdim);
  chord3 = gsl_vector_alloc(xdim);
  chord4 = gsl_vector_alloc(xdim);
  chord5 = gsl_vector_alloc(xdim);
  chord6 = gsl_vector_alloc(xdim);
  chord7 = gsl_vector_alloc(xdim);
  chord8 = gsl_vector_alloc(xdim);
  chord9 = gsl_vector_alloc(xdim);
  chord10 = gsl_vector_alloc(xdim);
  chord11 = gsl_vector_alloc(xdim);
  chord12 = gsl_vector_alloc(xdim);
  chord13 = gsl_vector_alloc(xdim);
  chord14 = gsl_vector_alloc(xdim);
  chord15 = gsl_vector_alloc(xdim);
  chord16 = gsl_vector_alloc(xdim);
  chord17 = gsl_vector_alloc(xdim);
  chord18 = gsl_vector_alloc(xdim);
  chord19 = gsl_vector_alloc(xdim);
  chord20 = gsl_vector_alloc(xdim);

  /* If all the fibers were found, write a true to the appropriate boolean */
  if ( (numEdges == NUM_FIBERS + 1) &&
       (numFibers == NUM_FIBERS) ) {
    chordsOK = true;
  } else {
    std::cout << "Number of fibers found not equal to pre-determined number.\n"
	      << "Number of fibers: " << NUM_FIBERS << "\n"
	      << "Number of fibers found: " << numFibers << "\n"
	      << "Number of edges: " << NUM_FIBERS + 1 << "\n"
	      << "Number of edges found: " << numEdges << "\n";
    chordsOK = false;
  }

  /* If all the fibers were found in the image, then populate the chords */
  if (chordsOK) {
    populateChords();
  }

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
  gsl_vector_free(rows);
  gsl_vector_free(fiberCenters);
  gsl_vector_free(fiberEdges);
  gsl_vector_free(binnedLine);
  gsl_vector_free(smoothedLine);
  gsl_vector_free(chord1);
  gsl_vector_free(chord2);
  gsl_vector_free(chord3);
  gsl_vector_free(chord4);
  gsl_vector_free(chord5);
  gsl_vector_free(chord6);
  gsl_vector_free(chord7);
  gsl_vector_free(chord8);
  gsl_vector_free(chord9);
  gsl_vector_free(chord10);
  gsl_vector_free(chord11);
  gsl_vector_free(chord12);
  gsl_vector_free(chord13);
  gsl_vector_free(chord14);
  gsl_vector_free(chord15);
  gsl_vector_free(chord16);
  gsl_vector_free(chord17);
  gsl_vector_free(chord18);
  gsl_vector_free(chord19);
  gsl_vector_free(chord20);

  gsl_matrix_free(image);
  gsl_matrix_ushort_free(imageUShort);

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
 * Function: setFiberCenters
 * Inputs: gsl_vector *
 * Returns: bool
 * Description: Sets the fiber centers fector
 ******************************************************************************/

bool LightField::setFiberCenters(gsl_vector *vecIn) {

  int ii, vecSize = vecIn->size;

  if (vecSize != numFibers) {
    std::cout << "Set Fibers Error. Vectors not the same length\n";
    return false;
  }

  for (ii = 0; ii < numFibers; ii++) {
    gsl_vector_set(fiberCenters, ii, gsl_vector_get(vecIn, ii));
  }

  return true;

}


/******************************************************************************
 * Function: setFiberEdges
 * Inputs: gsl_vector *
 * Returns: bool
 * Description: Sets the fiber edges vector
 ******************************************************************************/

bool LightField::setFiberEdges(gsl_vector *vecIn) {

  int ii, vecSize = vecIn->size;

  if (vecSize != numEdges) {
    std::cout << "Set Fiber Edges. Vectors not the same length\n";
    return false;
  }

  for (ii = 0; ii < numEdges; ii++) {
    gsl_vector_set(fiberEdges, ii, gsl_vector_get(vecIn, ii));
  }

  return true;

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
 * Function: getColWithRowSum
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will go through the image and sum up all rows, forming
 * a single column
 ******************************************************************************/

gsl_vector *LightField::getColWithRowSum() {

  int ii, jj;
  double sum;

  gsl_vector *retVec = gsl_vector_alloc(ydim);
  
  for (ii = 0; ii < ydim; ii++) {
    sum = 0;
    for (jj = 0; jj < xdim; jj++) {
      sum = sum + gsl_matrix_get(image, ii, jj);
    }
    gsl_vector_set(retVec, ii, sum);
  }

  return retVec;

}


/******************************************************************************
 * Function: smoothVector
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will smooth the passed vector by chopping of all FFT
 * components that are not the nth largest.
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
 * Function: getMaxima
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will find the local maxima of this smoothed vector
 ******************************************************************************/

gsl_vector *LightField::getMaxima(gsl_vector *vecIn) {

  int vecSize = vecIn->size;
  int ii, jj;
  int numMaxima = 0;

  gsl_vector *tempDeriv = gsl_vector_alloc(vecSize);
  gsl_vector *tempMaxima = gsl_vector_alloc(vecSize);
  gsl_vector_set_zero(tempMaxima);

  /* taking the derivative */
  for (ii = 0; ii < (vecSize-1); ii++) {
    gsl_vector_set(tempDeriv, ii,
		   gsl_vector_get(vecIn, ii+1)-gsl_vector_get(vecIn, ii));
  }
  gsl_vector_set(tempDeriv, vecSize-1, 0);

  /* find the zero crossing positive to negative zero crossings */
  for (ii = 5; ii < (vecSize-5); ii++) {

    /* Assuming we have a maxima */
    gsl_vector_set(tempMaxima, ii, 1);
    numMaxima += 1;
    for (jj = 0; jj < 5; jj++) {
      
      /* If yes, we found that it isn't a maxima, and zero and break */
      if ( (GSL_SIGN(tempDeriv->data[ii-(1+jj)])!=1) || 
	   (GSL_SIGN(tempDeriv->data[ii+1+jj])!=-1)  ||
	   (gsl_vector_get(tempMaxima, ii-1) == 1) ) {
	gsl_vector_set(tempMaxima, ii, 0);
	numMaxima -= 1;
	break;
      }

    }
  }

  gsl_vector *retVec = gsl_vector_alloc(numMaxima);

  jj = 0;
  double val;
  for (ii = 0; ii < vecSize; ii++) {
    val = gsl_vector_get(tempMaxima, ii); 
    if ( val == 1) {
      gsl_vector_set(retVec, jj, ii);
      jj++;
    }
  }

  gsl_vector_free(tempDeriv);
  gsl_vector_free(tempMaxima);

  return retVec;

}


/******************************************************************************
 * Function: getMinima
 * Inputs: 
 * Returns: gsl_vector *
 * Description: This will find the local minima of this smoothed vector
 ******************************************************************************/

gsl_vector *LightField::getMinima(gsl_vector *vecIn) {

  int vecSize = vecIn->size;
  int ii, jj;
  int numMaxima = 0;

  gsl_vector *tempDeriv = gsl_vector_alloc(vecSize);
  gsl_vector *tempMinima = gsl_vector_alloc(vecSize);
  gsl_vector_set_zero(tempMinima);

  /* taking the derivative */
  for (ii = 0; ii < (vecSize-1); ii++) {
    gsl_vector_set(tempDeriv, ii,
		   gsl_vector_get(vecIn, ii+1)-gsl_vector_get(vecIn, ii));
  }
  gsl_vector_set(tempDeriv, vecSize-1, 0);

  /* find the zero crossing positive to negative zero crossings */
  for (ii = 5; ii < (vecSize-5); ii++) {

    /* Assuming we have a maxima */
    gsl_vector_set(tempMinima, ii, 1);
    numMaxima += 1;
    for (jj = 0; jj < 5; jj++) {
      
      /* If yes, we found that it isn't a maxima, and zero and break */
      if ( (GSL_SIGN(tempDeriv->data[ii-(1+jj)])!=-1) || 
	   (GSL_SIGN(tempDeriv->data[ii+1+jj])!=1)  ||
	   (gsl_vector_get(tempMinima, ii-1) == 1) ) {
	gsl_vector_set(tempMinima, ii, 0);
	numMaxima -= 1;
	break;
      }

    }
  }

  gsl_vector *retVec = gsl_vector_alloc(numMaxima);

  jj = 0;
  double val;
  for (ii = 0; ii < vecSize; ii++) {
    val = gsl_vector_get(tempMinima, ii); 
    if ( val == 1) {
      gsl_vector_set(retVec, jj, ii);
      jj++;
    }
  }

  gsl_vector_free(tempDeriv);
  gsl_vector_free(tempMinima);

  return retVec;

}


/******************************************************************************
 * Function: populateChords
 * Inputs: 
 * Returns: gsl_vector *
 * Description: 
 ******************************************************************************/

int LightField::populateChords() {

  

  gsl_vector *chordMatrix[NUM_FIBERS] = {chord1, chord2, chord3, chord4, chord5, chord6,
					 chord7, chord8, chord9, chord10, chord11, chord12,
					 chord13, chord14, chord14, chord16, chord17, chord18,
					 chord19, chord20};

  int ii, jj, kk, ki, kf;
  double val;

  for (ii = 0; ii < NUM_FIBERS; ii++) {
    ki = (int) gsl_vector_get(fiberEdges, ii);
    kf = (int) gsl_vector_get(fiberEdges, ii+1);
    for (jj = 0; jj < xdim; jj++) {
      for (kk = ki + 1; kk < kf; kk++) {
	val = gsl_matrix_get(image, kk, jj) + gsl_vector_get(chordMatrix[ii], jj);
	gsl_vector_set(chordMatrix[ii], jj, val);
      }
    }
  }

  return 0;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testClassCreation();
static bool testFindColMax();
static bool testImageUShort();

bool testLightField() {

  if( !testImageUShort() ) {
    std::cout << "Failed ushort image test\n";
    return false;
  }
  
  if( !testClassCreation() ) {
    std::cout << "Failed Class Creation\n";
    return false;
  }

  if( !testFindColMax() ) {
    std::cout << "Failed Find Column Max\n";
    return false;
  }

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  int ii, jj;

  /* Populating fiberBoundCol */
  gsl_vector *fiberBoundCol = gsl_vector_alloc(test.smoothedLine->size);
  gsl_vector_set_zero(fiberBoundCol);
  for (ii = 0; ii < test.numEdges; ii++) {
    gsl_vector_set(fiberBoundCol, (int) gsl_vector_get(test.fiberEdges, ii), 1);
  }

  /* Populating fiberCenterCol */
  gsl_vector *fiberCenterCol = gsl_vector_alloc(test.smoothedLine->size);
  gsl_vector_set_zero(fiberCenterCol);
  for (ii = 0; ii < test.numFibers; ii++) {
    gsl_vector_set(fiberCenterCol, (int) gsl_vector_get(test.fiberCenters, ii), 1);
  }

  double maxVal = gsl_matrix_max(test.image);
  /* Putting into boundary lines to image */
  for (ii = 0; ii < test.numEdges; ii++) {
    for (jj = 0; jj < test.xdim; jj++) {
      gsl_matrix_set(test.image, (int) gsl_vector_get(test.fiberEdges, ii), jj, maxVal);
    }
  }

  test.plotImage();

  plotVectorData(test.rows, test.binnedLine, 
  		 "", "", "data/splTest.dat", "data/splTest.sh");

  plot2VectorData(test.rows, test.binnedLine, "title 'data'", 
  		  test.smoothedLine, "with line lw 3 title 'model'", 
  		  "", "data/temp1.txt", "data/temp1.sh");

  gsl_vector_scale(fiberCenterCol, maxVal);
  plot2VectorData(test.rows, fiberCenterCol, "with line title 'centers'", 
  		  test.smoothedLine, "with line lw 3 title 'model'", 
  		  "", "data/temp2.txt", "data/temp2.sh");

  gsl_vector_scale(fiberBoundCol, maxVal);
  plot2VectorData(test.rows, fiberBoundCol, "with line title 'bound'", 
  		  test.smoothedLine, "with line lw 3 title 'model'", 
  		  "", "data/temp3.txt", "data/temp3.sh");

  gsl_vector_scale(test.rows, maxVal);
  plot2VectorData(test.rows, fiberBoundCol, "with line title 'bound'", 
  		  test.binnedLine, "title 'data'", 
  		  "", "data/temp4.txt", "data/temp4.sh");

  plot2VectorData(test.waveLength, test.chord1, "with line title 'Chord 1'", 
		  test.chord2, "with line title 'Chord 2'", 
		  "", "data/temp5.txt", "data/temp5.sh");
  
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


static bool testImageUShort() {

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  int ii = 100,
    jj = 50;

  double minVal = gsl_matrix_min(test.image);
  ushort val = (ushort) (gsl_matrix_get(test.image, ii, jj) - minVal);

  if ( gsl_matrix_ushort_get(test.imageUShort, ii, jj) != val) {
    return false;
  }
  
  return true;

}
