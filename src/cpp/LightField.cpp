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
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testClassCreation();

bool testLightField() {

  if( !testClassCreation() ) {
    std::cout << "Failed Class Creation\n";
    return false;
  }

  std::cout << "Passed All Light Field Tests\n";

  return true;

}


static bool testClassCreation() {

  LightField test = LightField("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  return true;

}
