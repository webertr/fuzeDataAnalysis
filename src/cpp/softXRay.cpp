#include "cpp/softXRay.h"

/******************************************************************************
 *
 * This is the source file to analyze soft X-ray data
 *
 ******************************************************************************/

/******************************************************************************
 * Example Usage:
 *
 *
 ******************************************************************************/

/******************************************************************************
 * Static Function declarations
 ******************************************************************************/
static int clearGlobalVariables();
static int setGlobalVariables(int shotNumber);

/******************************************************************************
 * Global variables
 ******************************************************************************/
static gsl_vector *timeNeutronGlobal;
static gsl_vector *neutron10Global;
static gsl_vector *neutron12Global;
static gsl_vector *xRay2Global;
static gsl_vector *xRay4Global;


/******************************************************************************
 * Function: softXRayRun
 * Inputs:
 * Returns: 
 * Description: Runs soft X-ray analysis
 ******************************************************************************/

int softXRayRun() {

  int shotNumber,
    currShotNumber = getCurrentPulseNumber();

  printf("\nEnter Pulse Number> ");
  scanf("%d", &shotNumber);
  
 if (shotNumber <= 0) {
    shotNumber = currShotNumber+shotNumber;
  }

  getchar();


  setGlobalVariables(shotNumber);


  
  clearGlobalVariables();
  
  return 0;
  
}


/******************************************************************************
 * Function: setGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will set the global variables
 ******************************************************************************/

static int setGlobalVariables(int shotNumber) {
  
  timeNeutronGlobal = readMDSplusVectorDim(shotNumber, "\\neutron_10_s", "fuze");
  neutron10Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  neutron12Global = readMDSplusVector(shotNumber, "\\neutron_12_s", "fuze");
  xRay2Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  xRay4Global = readMDSplusVector(shotNumber, "\\neutron_12_s", "fuze");

  return 0;

}


/******************************************************************************
 * Function: clearGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will clear the global variables.
 ******************************************************************************/

static int clearGlobalVariables() {

  gsl_vector_free(timeNeutronGlobal);
  gsl_vector_free(neutron10Global);
  gsl_vector_free(neutron12Global);
  gsl_vector_free(xRay2Global);
  gsl_vector_free(xRay4Global);

  return 0;
  
}

/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testThisFunction();

int testSoftXRay() {

  if (!testThisFunction()) {
    std::cout << "Testing this function FAILED\n";
    return false;
  }

  std::cout << "All softXRay.cpp tests passed\n";

  return true;

}


static bool testThisFunction() {

  return false;

}
