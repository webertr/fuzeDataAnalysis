#include "magneticData.h"
#include "getLFData.h"
#include "getSVFData.h"
#include "imageAnalysisDHI.h"
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
  //testMagneticData();

  /* Testing image plotting */
  testPlotImageData();

  /*
   * Doing testing of synthetic data 
   */
  //getTestData();

  /*
   * Trying to do an FFT smoothing procedure
   */
  //testSmoothData();
  
  /*
   * Running the DHI analysis
   */
  //hologramMain();

  return 0;

}
