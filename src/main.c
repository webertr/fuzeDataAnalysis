#include "dataAnalysis.h"
#include "getLFData.h"
#include "getSVFData.h"
#include "imageAnalysis.h"
#include "testing.h"


/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: 
 ******************************************************************************/

int main(int argc, char *argv[]) {

  /* 
   * Line to plot SPE file with wavelength displayed at the bottom
   */
  //plotLightFieldImageWithWavelength("/home/fuze/Spectroscopy/Data/171212/171212  020.spe");

  /*
   * Line to plot Kirana SVF image from video
   */
  //plotKiranaImage("/home/fuze/Kirana_Videos/2017/170411/170411019.SVF", 95);

  /* 
   * Demonstrating how to get and plot mdsplus data.
   */
  //dataStructPlotTest(170817005, "\\b_n95_000_sm");

  /* 
   * struct containing all the holography parameters.
   * Setting to default values.
   */
  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  param.res = 3.85E-6;             // CCD Resolution
  param.lambda = 532E-9;           // Wavelength of laser
  param.d = 0.5;                  // Reconstruction distance
  param.deltaN = 1E23;             // Density offset delta for inversion
  param.hyperbolicWin = 8;         // Hyperbolic window parameter
  param.sampleInterval = 1;        // Sampling interval of line-integrated density 
  param.centroidNum = 10;          // number of centroids to vary +/- around maximum (10)
  param.offsetIter = 10;           // Number of offset iterations (15)
  param.boxCarSmoothWidth = 40;    // Width of box car smoothing on phase
  param.unwrapThresh = 1.0*M_PI;   // Threshold to trigger the phase unwrapping
  param.signTwin = 1;              // Sign to density conversion +/-1. Depends on laser setup (-1)
  param.debugPhase = 1;            // 1 means save and plot a col profile of phase 
                                   // and unwrapped phase
  param.debugPhaseColNum = 10;     // Col number to save for the phase and unwrapped phase
  param.debugPhaseRowNum = 61;     // Row number to save for the phase and unwrapped phase
  param.hologramPreview = 0;       // 1 means to preview the hologram before extracting twin image
  param.invertImage = 0;           // 1 means to invert the image.
  param.plotRadialProfile = 1;     // 1 means to plot the inverted radial profile and slice through
                                   // the line integrated image (at plotColNum)
  param.plotColNum = 100;           // Column number to plot for the inverted radial profile and a 
                                   // slice of line integrated data 
  param.plotLineIntegrated = 1;    // 1 means to plot the line integrated data
  param.plotRawHologram = 0;       // 1 means it will plot the raw hologram
  param.plotRawHologramRow = 100;  // 1 means it will plot a row of the raw hologram
  param.plotRawHologramCol = 100;  // 1 means it will plot a column of the raw hologram
  param.plotTwinImage = 0;         // 1 means it will plot a column of the twin image  
  param.rotateImage = 1;           // 1 means to rotate the image by 90 degrees CW

  /******** Holography Analysis *************/

  char *filenameRef = "/home/fuze/DHI_Images/171213/171213018.JPG";
  char *filenamePlasma = "/home/fuze/DHI_Images/171213/171213016.JPG";

  /* Setting bounds of reconstructed image */
  param.xLL = 2739;          // Lower left x pixel value of phase reconstruction
  param.yLL = 2751;          // Lower left y pixel value of phase reconstruction
  param.xUR = 3430;          // Upper right x pixel value of phase reconstruction
  param.yUR = 4382;          // Upper right y pixel value of phase reconstruction


  /* Obtained line integrated data and do an abel inversion */
  hologramAnalysis(filenameRef, filenamePlasma, 
		   &param,
		   "data/leftProfile.dat", "data/rightProfile.dat",
		   "data/centroidLocation.dat");
  
  return 0;

}

/*
 * Holography Parameters:
 *
 * Resolution of Camera CCD Pixels = 3.85E-6,
 * Wavelength of the laser = 532E-9,
 * Reconstruction distance = 0.5;
 * Bounds of reconstructed image: xLL, yLL, xUR, yUR
 * Hyperbolic Window Fit Parameter: 8
 * Sampling interval of line-integrated density: 10
 * Box car average window of phase: 8
 * Sign of the twin image (depends on laser setup): -1
 *
 * Inversion Parameters:
 * Numbers of centroids to vary +/- around the maximum: 10
 * Density offset delta = 5E18
 * Number of offset iterations: 15
 *
 * 1 radian = 6.7E20 number/m^2
 */
