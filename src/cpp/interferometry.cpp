#include "cpp/interferometry.h"

/******************************************************************************
 *
 * These are functions to break down inteferometry data from the FuZE experimemt
 *
 ******************************************************************************/

/*
 */

/******************************************************************************
 * Function: getIFData
 * Inputs: 
 * Returns: boolean
 * Description: Gets inteferometry data for passed shot number
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:COS# = The cos of the phase shift of the scene beam
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:SIN# = The sin of the phase shift of the scene beam
 ******************************************************************************/

static std::string getIFData(long shotNumber) {

  gsl_vector *cos1 = readMDSplusVector(shotNumber, "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_1:COS1", "fuze");
  gsl_vector *sin1 = readMDSplusVector(shotNumber, "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_1:SIN1", "fuze");

  return "";
  
}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testGetIFData();

bool testInterferometry() {

  if (!testGetIFData()) {
    std::cout << "Test interferometry.cpp get data FAILED\n";
    return false;
  }

  std::cout << "All interferometry.cpp tests passed\n";

  return true;

}


bool testGetIFData() { 

  getIFData(190416004);
  
  return true;

}
