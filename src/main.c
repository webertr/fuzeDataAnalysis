#include "magnetic.h"
#include "getLFData.h"
#include "getSVFData.h"
#include "imageAnalysisDHI.h"
#include "testing.h"
#include "analysis.h"
#include "holoParam.h"


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
  //testPlotImageData();

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
  //hologramAnalysis();

  /*
   * Plotting plasma parameters
   */
  //testPlasmaParam();

  /* 
   * Plotting post shot data
   */
  //plotPostShotData(170817005);

  /*
   * Plotting IP for talk
   */
  //plotAccelApril2018Talk();

  /*
   * Plotting mode data for talk
   */
  plotModeApril2018Talk();

  /*
   * Plotting DHI data for talk
   */
  //plotDHIApril2018Talk();

  /*
   * Testing real FFT
   */
  //testAziMode();
  
  return 0;

}
