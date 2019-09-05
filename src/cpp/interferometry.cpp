#include "cpp/interferometry.h"

/******************************************************************************
 *
 * These are functions to break down inteferometry data from the FuZE experimemt
 *
 ******************************************************************************/

static gsl_vector *getIFData(long shotNumber, long baseLineShotNumber, int chordNum);


/******************************************************************************
 * Function: getIFData
 * Inputs: 
 * Returns: boolean
 * Description: Gets inteferometry data for passed shot number
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:COS# = The cos of the phase shift of the scene beam
 * TOP.SIGNALS.DENSITY.HENE_IF:NE_#:SIN# = The sin of the phase shift of the scene beam
 ******************************************************************************/

static gsl_vector *getIFData(long shotNumber, long baseLineShotNumber, int chordNum) {

  
  std::string baseLineCNode;
  std::string baseLineSNode;
  std::ostringstream oss;

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":COS" << chordNum;
  baseLineCNode = oss.str();
  oss.str("");

  oss << "\\TOP.SIGNALS.DENSITY.HENE_IF:NE_" << chordNum << ":SIN" << chordNum;
  baseLineSNode = oss.str();
  oss.str("");

  gsl_vector *baseLineCos = readMDSplusVector(baseLineShotNumber, baseLineCNode, "fuze");
  gsl_vector *baseLineSin = readMDSplusVector(baseLineShotNumber, baseLineSNode, "fuze");

  double cosOffset = gsl_stats_mean(baseLineCos->data, baseLineCos->size, baseLineCos->stride);
  double sinOffset = gsl_stats_mean(baseLineSin->data, baseLineSin->size, baseLineSin->stride);

  std::cout << "X offset: " << cosOffset << "\n";
  std::cout << "Y offset: " << sinOffset << "\n";

  plotVectorData(baseLineCos, baseLineSin, "", "", "data/inter.txt", "data/inter.sh");

  //gsl_vector_add_constant(cos1, -cos1Offset);
  //gsl_vector_add_constant(sin1, -sin1Offset);

  //plotVectorData(cos1, sin1, "", "", "data/inter.txt", "data/inter.sh");

  gsl_vector_free(baseLineCos);
  gsl_vector_free(baseLineSin);

  return (gsl_vector *) NULL;
  
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

  getIFData(190903008, 190903004, 1);
  //getIFData(190903005);
  //getIFData(190416004);
  
  return true;

}
