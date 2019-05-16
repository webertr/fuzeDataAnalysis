#ifndef GETLIGHTFIELD_H
#define GETLIGHTFIELD_H

#include <iostream>
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <string.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


/******************************************************************************
 * Class: LightField
 * Inputs: 
 * Description: 
 ******************************************************************************/

class LightField {
  
 private:

  std::string fileName;               // The file name
  static const int MAX_BUFFER = 1000;
  int xdim;                           // The width of the frame in pixels
  int ydim;                           // The height of the frame in pixels
  int frameNum;                       // The Number of frames.
  int imageSize;                      // The total number of pixels
  signed int pixelType;               // The pixel type 32f = 32 bit floating type
  char speFile[100];                  // The SPE fiel anem
  bool xmlParserSPE(std::string fileName, float *dataArray, int dim);

 public:

  gsl_vector *waveLength;
  gsl_matrix *image;
  
  LightField(std::string fileNameParam);
  ~LightField();

};


/* Testing function */
bool testLightField();

#endif
