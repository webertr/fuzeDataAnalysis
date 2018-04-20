#ifndef LF_DATA_H
#define LF_DATA_H

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

/* 
 * A structure to pass around the SPE file data
 */
struct lightfieldparameters { 
  int xdim;                      // The width of the frame in pixels
  int ydim;                      // The height of the frame in pixels
  int frameNum;                  // The Number of frames.
  int imageSize;                 // The total number of pixels
  signed int pixelType;          // The pixel type 32f = 32 bit floating type
  char speFile[100];             // The SPE fiel anem
};

typedef struct lightfieldparameters lightFieldParameters;

/*
 * This is the struct that will hold the default values,
 * static means it should only be visible to the source file
 * that includes it, so the compiler won't complain when linking, I think?
 */
static const lightFieldParameters LIGHT_FIELD_PARAMETERS_DEFAULT = {
  .xdim = 0, 
  .ydim = 0,
  .imageSize = 0
};

int getLightFieldData(gsl_matrix **image, gsl_vector **waveLength, lightFieldParameters *param);
int xmlParserSPE(char *fileName, float *dataArray, int dim);
int saveImageWithWavelength(gsl_matrix *mInput, gsl_vector* wavVec, char *fileName);
int saveLightFieldImageWithWavelength(char *speFile, char *saveFile);

#endif
