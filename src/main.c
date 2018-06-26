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
  //hologramAnalysis();

  /* 
   * Plotting post shot data
   */
  plotPostAnalysis();
  //plotPostShotModeData(180215012, 0, 50, "\\b_p15_000_sm");

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
  
  return 0;

}
