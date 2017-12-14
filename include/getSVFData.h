#ifndef GETSVFDATA_H
#define GETSVFDATA_H

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
#include <byteswap.h>

/* 
 * A structure to pass around the SVF file
 */
struct kiranasvfdata { 
  gsl_matrix *image;       // A gsl matrix of the image
  int width;               // The width of the frame in pixels
  int height;              // The height of the frame in pixels
  int frameNum;            // The Number of frames.
  short *data;             // Pointer to all frames.
  int frameSize;           // Size of an image in...
};

typedef struct kiranasvfdata kiranaSVFData;

int readSVFFile(kiranaSVFData *kiranaStruct, char *fileName);
int saveKiranaImage(gsl_matrix *mInput, char *fileName);
int kiranaSetImage(kiranaSVFData *kiranaStruct, int frameNumber);
int plotKiranaImage(char *fileName, int frameNumber);

#endif
