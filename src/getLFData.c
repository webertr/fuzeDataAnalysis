#include "getLFData.h"

/* Char buffer to write xml */
#define MAX_BUFFER 1000
 

/******************************************************************************
 * Function: xmlParserSPE
 * Inputs: char *, float *, in
 * Returns: int
 * Description: Parses xml file
 ******************************************************************************/

int xmlParserSPE(char *fileName, float *dataArray, int dim) {
  
    xmlDoc *document;
    xmlNode *root, *first_child, *node, *calibration, *wavelengthMapping, *wavelength;
    xmlChar *data;
    
    document = xmlReadFile(fileName, NULL, 0);
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

    char *end = ",";
    char *location;
    dataArray[0] = strtof( (char *) data, &end);
    location = strchr((const char *) data, ',');
    for ( ii = 1; ii < dim; ii++) {
      
      dataArray[ii] = strtof( &location[1], &end);
      location = strchr(&location[1], ',');
      
    }

    /* Free xml document */
    xmlFreeDoc(document);


    return 0;
}


/******************************************************************************
 * Function: getLightFieldData
 * Inputs: lightFieldData, char *
 * Returns: int
 * Description: Pass a .spe file, and it will return a data structure contain
 * all the light field data.
 ******************************************************************************/

int getLightFieldData(lightFieldData *dataStruct, char *fileName) {

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
  fp = fopen(fileName, "r");

  /*
   * Pulling the 42th byte to get the width of the frame in pixels
   */
  fseek(fp, 42, SEEK_SET);
  fb = fread((void *) &(dataStruct->xdim), (size_t) 2 , (size_t) 1, fp);

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
  fb = fread((void *) &(dataStruct->ydim), (size_t) 2, (size_t) 1, fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != 1) {

    printf("Error reading SPE file for frame height\n");
    fclose(fp);
    exit(1);
    
  }

  dataStruct->imageSize = (dataStruct->xdim)*(dataStruct->ydim);

  /*
   * Returning file to original position
   */
  fseek(fp, 0, SEEK_SET);

  /*
   * Pulling the 1446th byte to get the number of the frames
   */
  fseek(fp, 1446, SEEK_SET);
  fb = fread((void *) &(dataStruct->frameNum), (size_t) 4, (size_t) 1, fp);

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
  fb = fread((void *) &(dataStruct->pixelType), (size_t) 2, (size_t) 1, fp);

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
  data = (float *)malloc((dataStruct->xdim) * (dataStruct->ydim) * sizeof(float));
  fseek(fp, 4100, SEEK_SET);
  fb = fread((void *) data, (size_t) 4, (size_t) (dataStruct->imageSize), fp);

  /*
   * Verifying the read occurred correctly
   */
  if (fb != (dataStruct->imageSize)) {

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
  dataStruct->image = gsl_matrix_alloc(dataStruct->ydim, dataStruct->xdim);

  /*
   * Moving data over to matrix
   */
  int ii, jj;
  for (ii = 0; ii < (dataStruct->xdim); ii ++) {

    for (jj = 0; jj < (dataStruct->ydim); jj++) {

      gsl_matrix_set(dataStruct->image, jj, ii, (double) data[(dataStruct->xdim)*jj+ii]);
      
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
  
  /*
   * Set fp to start of the xml file and read xml file
   */
  fXML = fopen("data/xmlTemp.xml", "w");

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
  if (fbXML != (sz-xmlOffsetTemp) ) {

    printf("Error writing xml file:\n");
    fclose(fXML);
    fclose(fp);
    exit(1);
    
  }

  fclose(fXML);
  
  /* Parsing xml file */
  wvData = (float *) malloc((dataStruct->xdim)*sizeof(float));
  xmlParserSPE("data/xmlTemp.xml", wvData, dataStruct->xdim);

  /*
   * Vector contain wavelengths
   */
  dataStruct->waveLengthVector = gsl_vector_alloc(dataStruct->xdim);

  for (ii = 0; ii < (dataStruct->xdim); ii++) {

    gsl_vector_set(dataStruct->waveLengthVector, ii, (double) wvData[ii]);
      
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
  return 1;
  
}


/******************************************************************************
 * Function: saveImageWithWavelength
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

int saveImageWithWavelength(gsl_matrix *mInput, gsl_vector* wavVec, char *fileName) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Creates matrix to get written */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }
  /* Setting x vector values */
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) gsl_vector_get(wavVec, ii-1));
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
 * Function: plotLightFieldImageWithWavelength
 * Inputs: char *fileName
 * Returns: int
 * Description: This will plot the image in an SPE file with the wavelength
 ******************************************************************************/

int plotLightFieldImageWithWavelength(char *fileName) {

  lightFieldData dataStruct = LIGHT_FIELD_DATA_DEFAULT;

  getLightFieldData(&dataStruct, fileName);

  saveImageWithWavelength(dataStruct.image, dataStruct.waveLengthVector, "data/imageWV.dat");

  gsl_matrix_free(dataStruct.image);
  gsl_vector_free(dataStruct.waveLengthVector);

  system("script/plot_image_wavelength.sh");

  return 0;

}
