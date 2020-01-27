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
  float *wvData;
  void *data = NULL;

  /* Offset for xml file (64 bit)*/
  unsigned int xmlOffset;

  char xmlFileBuffer[MAX_BUFFER];

  /* 
   * Opening the file
   */
  std::cout << "Opening file for LF object creation\n";
  fp = fopen(fileName.c_str(), "r");

  /* Checking if file exists */
  if (!fp) {
    throw std::invalid_argument("File " + fileNameParam + " does not exists\n");
    return;
  } else {
    std::cout << "LF object filed opened successfully\n";
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
   * 3 = uint16, 1 = int32, 2 = int16
   */
  fseek(fp, 108, SEEK_SET);
  fb = fread((void *) &pixelType, (size_t) 2, (size_t) 1, fp);

  int dataSize;
  if (pixelType == 0) {
    dataSize = 4;
    data = malloc(xdim * ydim * sizeof(float));
  } else if (pixelType == 1) {
    dataSize = 4;
    data = malloc(xdim * ydim * sizeof(float));
  } else if (pixelType == 2) {
    dataSize = 4;
    data = malloc(xdim * ydim * sizeof(int));
  } else if (pixelType == 3) {
    dataSize = 2;
    data = malloc(xdim * ydim * sizeof(uint));
  } else if (pixelType == 4) {
    dataSize =4;
    data = malloc(xdim * ydim * sizeof(float));
  } else {
    dataSize = 4;
    data = malloc(xdim * ydim * sizeof(float));
  }

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
   * Pulling the 4100th byte to get the binary image.
   */
  
  
  fseek(fp, 4100, SEEK_SET);
  fb = fread((void *) data, (size_t) dataSize, (size_t) (imageSize), fp);

  printf("Image size: %d\n", imageSize);
  printf("Read Back: %d\n", (int) fb);
  printf("X-dimension: %d\n", xdim);
  printf("Y-dimension: %d\n", ydim);
  printf("Pixel Type: %d\n", pixelType);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != (size_t) (imageSize) ) {

    free(data);
    printf("Error reading SPE file for image:\n");
    printf("Image size: %d\n", imageSize);
    printf("Read Back: %d\n", (int) fb);
    printf("X-dimension: %d\n", xdim);
    printf("Y-dimension: %d\n", ydim);
    printf("Pixel Type: %d\n", pixelType);
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

  if (pixelType == 0) {
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((float *) data)[xdim*jj+ii]);
      }
    }
  } else if (pixelType == 1) { 
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((float *) data)[xdim*jj+ii]);
      }
    }
 } else if (pixelType == 2) {
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((int *) data)[xdim*jj+ii]);
      }
    }
  } else if (pixelType == 3) {
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((uint *) data)[xdim*jj+ii]);
      }
    }
  } else if (pixelType == 4) {
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((float *) data)[xdim*jj+ii]);
      }
    }
  } else {
    for (ii = 0; ii < xdim; ii ++) {
      for (jj = 0; jj < ydim; jj++) {
	gsl_matrix_set(image, jj, ii, (double) ((float *) data)[xdim*jj+ii]);
      }
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
  
  std::string xmlTempFile = "xmlTemp.xml";

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
  binnedLine = getColWithRowSum();
  //binnedLine = getBinnedCol(maxLineIndex, 10);

  /* Smooth line by binning */
  binVector(binnedLine, 5);

  /* Getting the peaks of the smoothed line */
  fiberCenters = getMaxima(binnedLine);
  
  /* Setting total number of fibers */
  numFibers = fiberCenters->size;

  /* Finding fiber boundaries */
  fiberEdges = getFiberEdges();

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
    std::cout << "Found correct number of edges (" << NUM_FIBERS+1
	      << ") and fibers (" << NUM_FIBERS << ")\n";
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
  xmlNode *root, *first_child, *node, *calibration, *wavelengthMapping, *wavelength = 0;
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
 * Function: getFiberEdges
 * Inputs: void
 * Returns: gsl_vector *
 * Description: Figures out the fiber edges from the fiber centers
 ******************************************************************************/

gsl_vector *LightField::getFiberEdges() {

  int ii;
  
  gsl_vector *fiberEdgesRet = gsl_vector_alloc(numFibers + 1);
  gsl_vector_set(fiberEdgesRet, 0, 0);
  gsl_vector_set(fiberEdgesRet, numFibers, ydim-1);

  int halfDist,
    fibE;
  for (ii = 1; ii < numFibers; ii++) {
    halfDist = (int) (gsl_vector_get(fiberCenters, ii) - gsl_vector_get(fiberCenters, ii-1)) / 2;
    fibE = (int) gsl_vector_get(fiberCenters, ii) - halfDist;
    gsl_vector_set(fiberEdgesRet, ii, fibE);
  }

  return fiberEdgesRet;

}


/******************************************************************************
 * Function: setNumFibers
 * Inputs: gsl_vector *
 * Returns: bool
 * Description: Sets the fiber centers fector
 ******************************************************************************/

bool LightField::setNumFibers(int numFibersIn) {

  numFibers = numFibersIn;

  return true;

}

/******************************************************************************
 * Function: setNumEdges
 * Inputs: gsl_vector *
 * Returns: bool
 * Description: Sets the fiber centers fector
 ******************************************************************************/

bool LightField::setNumEdges(int numEdgesIn) {

  numEdges = numEdgesIn;

  return true;

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
    std::cout << "Set Fiber Centers Error. Vectors not the same length\n";
    return false;
  }

  /* Freeing old fiber centers and allocating new one */
  gsl_vector_free(fiberCenters);
  fiberCenters = gsl_vector_alloc(numFibers);

  for (ii = 0; ii < numFibers; ii++) {
    gsl_vector_set(fiberCenters, ii, gsl_vector_get(vecIn, ii));
  }

  return true;

}


/******************************************************************************
 * Function: binVector
 * Inputs: gsl_vector *, int
 * Returns: void
 * Description: Bins the passed vector
 ******************************************************************************/

void LightField::binVector(gsl_vector *vecIn, int binSize) {

  int ii, jj, vecSize = vecIn->size;
  double sum;

  gsl_vector *temp = gsl_vector_alloc(vecSize);

  for (ii = binSize; ii < (vecSize - binSize); ii++) {
    sum = 0;
    for (jj = -binSize; jj < binSize; jj++) {
      sum = sum + gsl_vector_get(vecIn, ii+jj);
    }
    gsl_vector_set(temp, ii, sum); 
  }

  for (ii = 0; ii < binSize; ii++) {
    gsl_vector_set(temp, ii, gsl_vector_get(vecIn, ii));
  }

  for (ii = vecSize-binSize; ii < vecSize; ii++) {
    gsl_vector_set(temp, ii, gsl_vector_get(vecIn, ii));
  }

  /* Copying temporary vector over */
  gsl_vector_memcpy(vecIn, temp);

  gsl_vector_free(temp);

  return;

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
    std::cout << "Set Fiber Edges Error. Vectors not the same length\n";
    return false;
  }

  /* Freeing old fiber centers and allocating new one */
  gsl_vector_free(fiberEdges);
  fiberEdges = gsl_vector_alloc(numEdges);

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
 * Function: uploadToMDSplus
 * Inputs: 
 * Returns: void
 * Description: 
 ******************************************************************************/

void LightField::uploadToMDSplus(int shotNumber) {


  if (!chordsOK) {
    std::cout << "Fiber centers not found. Not uploading to mdsplus\n";
    return;
  }

  std::cout << "Uploading data to mdsplus\n";

  /* Uploading LF data to mdsplus */
  writeMDSplusMatrix(imageUShort, shotNumber, "\\ICCD:RAW", "fuze");
  writeMDSplusVector(waveLength, shotNumber, "\\ICCD:LAMBDA", "fuze");
  writeMDSplusVector(rows, shotNumber, "\\ICCD:ROWS", "fuze");
  writeMDSplusVector(fiberCenters, shotNumber, "\\ICCD:FIBERCENTERS", "fuze");
  writeMDSplusVector(fiberEdges, shotNumber, "\\ICCD:FIBEREDGES", "fuze");

  /* Uploading chords to mdsplus */
  writeMDSplusVector(chord1, shotNumber, "\\ICCD_01:RAW", "fuze");
  writeMDSplusVector(chord2, shotNumber, "\\ICCD_02:RAW", "fuze");
  writeMDSplusVector(chord3, shotNumber, "\\ICCD_03:RAW", "fuze");
  writeMDSplusVector(chord4, shotNumber, "\\ICCD_04:RAW", "fuze");
  writeMDSplusVector(chord5, shotNumber, "\\ICCD_05:RAW", "fuze");
  writeMDSplusVector(chord6, shotNumber, "\\ICCD_06:RAW", "fuze");
  writeMDSplusVector(chord7, shotNumber, "\\ICCD_07:RAW", "fuze");
  writeMDSplusVector(chord8, shotNumber, "\\ICCD_08:RAW", "fuze");
  writeMDSplusVector(chord9, shotNumber, "\\ICCD_09:RAW", "fuze");
  writeMDSplusVector(chord10, shotNumber, "\\ICCD_10:RAW", "fuze");
  writeMDSplusVector(chord11, shotNumber, "\\ICCD_11:RAW", "fuze");
  writeMDSplusVector(chord12, shotNumber, "\\ICCD_12:RAW", "fuze");
  writeMDSplusVector(chord13, shotNumber, "\\ICCD_13:RAW", "fuze");
  writeMDSplusVector(chord14, shotNumber, "\\ICCD_14:RAW", "fuze");
  writeMDSplusVector(chord15, shotNumber, "\\ICCD_15:RAW", "fuze");
  writeMDSplusVector(chord16, shotNumber, "\\ICCD_16:RAW", "fuze");
  writeMDSplusVector(chord17, shotNumber, "\\ICCD_17:RAW", "fuze");
  writeMDSplusVector(chord18, shotNumber, "\\ICCD_18:RAW", "fuze");
  writeMDSplusVector(chord19, shotNumber, "\\ICCD_19:RAW", "fuze");
  writeMDSplusVector(chord20, shotNumber, "\\ICCD_20:RAW", "fuze");

  return;

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
					 chord13, chord14, chord15, chord16, chord17, chord18,
					 chord19, chord20};

  int ii, jj, kk, ki, kf;
  double val;

  for (ii = 0; ii < NUM_FIBERS; ii++) {
    ki = (int) gsl_vector_get(fiberEdges, ii);
    kf = (int) gsl_vector_get(fiberEdges, ii+1);
    for (jj = 0; jj < xdim; jj++) {
      val = 0;
      for (kk = ki; kk < kf; kk++) {
	val = gsl_matrix_get(image, kk, jj) + val;
      }
      gsl_vector_set(chordMatrix[ii], jj, val);
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
static bool testFibersFind();
static bool testImageRead();

bool testLightField() {

  LightField test = LightField("/home/fuze/SpectroscopyData/200122/200122008.spe");
  test.plotImage();
  
  if (!testImageRead()) {
    std::cout << "Failed image read test\n";
  }

  if (!testFibersFind()) {
    std::cout << "Failed fibers find test\n";
  }

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

  std::cout << "Passed All Light Field Tests\n";

  return true;

}


static bool testClassCreation() {

  LightField test = LightField("/home/fuze/SpectroscopyData/171212/171212  020.spe");

  return true;

}

static bool testImageRead() {

  //LightField test = LightField("/home/fuze/SpectroscopyData/171212/171212  020.spe");
  LightField test = LightField("/home/fuze/SpectroscopyData/190821/190821012.spe");

  return true;

}


static bool testFindColMax() {

  LightField test = LightField("/home/fuze/SpectroscopyData/171212/171212  020.spe");

  int colMax = test.maxLineIndex;

  if (colMax != 462) {
    return false;
  }

  return true;

}


static bool testImageUShort() {

  LightField test = LightField("/home/fuze/SpectroscopyData/171212/171212  020.spe");

  int ii = 100,
    jj = 50;

  double minVal = gsl_matrix_min(test.image);
  ushort val = (ushort) (gsl_matrix_get(test.image, ii, jj) - minVal);

  if ( gsl_matrix_ushort_get(test.imageUShort, ii, jj) != val) {
    return false;
  }
  
  return true;

}


static bool testFibersFind() {

  int shotNumber = 190604020;
  std::string shotNumberFileName = "/home/fuze/SpectroscopyData/190604/190604020.spe";

  LightField lfObject = LightField(shotNumberFileName);

  lfObject.uploadToMDSplus(shotNumber);

  plotVectorData(lfObject.waveLength, lfObject.binnedLine, 
  		 "", "", "data/splTest.dat", "data/splTest.sh");

  double maxVal = gsl_matrix_max(lfObject.image);
  
  /* Putting into boundary lines to image */
  bool center = false;
  bool edge = true;
  int ii, jj;
  if (center) {
    for (ii = 0; ii < lfObject.numFibers; ii++) {
      for (jj = 0; jj < lfObject.xdim; jj++) {
	gsl_matrix_set(lfObject.image, 
		       (int) gsl_vector_get(lfObject.fiberCenters, ii)+1, jj, maxVal);
	gsl_matrix_set(lfObject.image, 
		       (int) gsl_vector_get(lfObject.fiberCenters, ii), jj, maxVal);
	gsl_matrix_set(lfObject.image, 
		       (int) gsl_vector_get(lfObject.fiberCenters, ii)-1, jj, maxVal);
      }
    }
  } else if (edge) {
    for (ii = 1; ii < (lfObject.numEdges-1); ii++) {
      for (jj = 0; jj < lfObject.xdim; jj++) {
	gsl_matrix_set(lfObject.image,
		       (int) gsl_vector_get(lfObject.fiberEdges, ii) + 1, jj, maxVal);
	gsl_matrix_set(lfObject.image, 
		       (int) gsl_vector_get(lfObject.fiberEdges, ii), jj, maxVal);
	gsl_matrix_set(lfObject.image,
		       (int) gsl_vector_get(lfObject.fiberEdges, ii) - 1, jj, maxVal);
      }
    }

    for (jj = 0; jj < lfObject.xdim; jj++) {
      	gsl_matrix_set(lfObject.image, (int) gsl_vector_get(lfObject.fiberEdges, 0), jj, maxVal);
    }

    for (jj = 0; jj < lfObject.xdim; jj++) {
      	gsl_matrix_set(lfObject.image,
		       (int) gsl_vector_get(lfObject.fiberEdges, lfObject.numFibers-1), 
		       jj, maxVal);
    }
    
  }
  
  lfObject.plotImage();

  if (lfObject.chordsOK) {
    return true;
  }

  return false;

}
