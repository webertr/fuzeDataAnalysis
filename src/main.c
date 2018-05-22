#include "main.h"

/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: 
 ******************************************************************************/

int main(int argc, char *argv[]) {

  /*
   * Running the DHI analysis
   */
  hologramAnalysis();

  /* 
   * Plotting post shot data
   */
  //plotPostAnalysis();

  /*
   * Testing DHI abel inverison code 
   */
  //testAbelInversionDHI();
  
  /*
   * Plotting plasma parameters
   */
  //plasmaParameters();

  /*
   * Simulating particle in acceleration region of marshall gun
   */
  //simluateAccel();

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
  //plotSpecCIIIImageApril2018Talk();


  /*
   * CII emission line
   */
  //plotCIIILineApril2018Talk();

  
  /*
   * Testing real FFT
   */
  //testMagnetic();

  /*
   * test mode data with mdsplus
   */
  //plotModeCompApril2018Talk();

  /* 
   * Inverting test flat top data
   */
  //invertFlatTopProfile();
  //flatTopRadialForceBalance();

  return 0;

}
