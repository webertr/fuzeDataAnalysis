#include "main.h"

/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: 
 ******************************************************************************/

int main(int argc, char *argv[]) {

  /*
   * Testing mdsplus read 
   */
  //mdsplusReadTest();
  
  /*
   * Running the DHI analysis
   */
  //hologramAnalysis();

  /*
   * Running the pinch formation analysis
   */
  //pinchAnalysis();

  /* 
   * Plotting post shot data
   */
  plotPostAnalysis();

  /*
   * Modeling magnetic field from random currents
   */
  //magneticModel();
  //testMagneticModel();
  
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
  //testForceBalance();

  /*
   * Regex expressions
   */
  //testRegExpSub();

  /*
   * Checking bank diagnostics
   */
  //bankCheck();

  return 0;

}
