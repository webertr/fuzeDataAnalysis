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
   * Demonstrating how to get and plot mdsplus data.
   */
  //testMagneticData();

  /*
   * Doing testing of synthetic data 
   */
  //getTestData();

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
  //plotModeApril2018Talk();

  /*
   * Plotting DHI data for talk
   */
  //plotDHIApril2018Talk();

  /*
   * Plotting spectrscopy data for talk
   */
  plotSpecCIIIImageApril2018Talk();


  /*
   * CII emission line
   */
  //plotCIIILineApril2018Talk();

  
  /*
   * Testing real FFT
   */
  //testAziMode();
  
  return 0;

}
