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
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_sort.h>

#include "plot.h"

/******************************************************************************
 * Class: LightField
 * Inputs: 
 * Description: 
 ******************************************************************************/

class LightField {
  
 private:

  std::string fileName;                          // The file name
  static const int MAX_BUFFER = 1000;            // Used instead of #define directive
  int xdim;                                      // The width of the frame in pixels
  int ydim;                                      // The height of the frame in pixels
  int frameNum;                                  // The Number of frames.
  int imageSize;                                 // The total number of pixels
  int numFibers;                                 // The total number of fibers found
  int numBoundaries;                             // The total number of fiber boundaries found
  signed int pixelType;                          // The pixel type 32f = 32 bit floating type
  char speFile[100];                             // The SPE fiel anem
  bool xmlParserSPE(std::string fileName,        // Internal function to deal with .xml file
		    float *dataArray, int dim); 
  int getColMax();                               // Find the max summed column in the image
  gsl_vector *getBinnedCol(int colIndex,         // For the col line index this bins the col 
			   int binNum);          // +/- 5 pixels around this column
  gsl_vector *smoothVector(gsl_vector *vecIn,    // Smooths passed vector by chopping off
			   int maxFFTCutoff);    // FFT components below cutoff
  gsl_vector *getMaxima(gsl_vector *vecIn);      // Getting the peaks of the smoothed line
  gsl_vector *getMinima(gsl_vector *vecIn);      // Getting the valleys of the smoothed line

 public:

  int fiberSpacing;                          // The average spacing between fiber centers
  int maxLineIndex;                          // The index of the brighted sumed col. This is what
                                             // We will say is the location of the chord we will
                                             // Use to find the fiber centers/boundaries
  gsl_vector *waveLength;                    // A vector of the wavelengths of the spectrometer
  gsl_vector *fiberCenters;                  // A vector of the center values of each fiber
  gsl_vector *fiberBoundaries;               // The boundaries of all fibers
  gsl_vector *binnedLine;                    // A the primary line summed across +/- 5 pixels
                                             // to account for any doppler shifts in wavelength
  gsl_vector *smoothedLine;                  // Binned line but smoothed with FFT
  gsl_vector *fiberCenterCol;                // A 1 if a fiber center, zero otherwise
  gsl_vector *fiberBoundCol;                 // A 1 if a fiber boundary, zero otherwise
  gsl_matrix *image;                         // The full image of all 20 fibers
  
  LightField(std::string fileNameParam);
  ~LightField();
  int plotImage();                           // Plots the main image

};


/* Testing function */
bool testLightField();

#endif
