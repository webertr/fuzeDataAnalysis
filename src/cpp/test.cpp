#include "cpp/test.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: testGSL
 * Inputs: 
 * Returns: int
 * Description: Testing to see if the GSL works.
 ******************************************************************************/

int testGSL() {

  gsl_vector *test = gsl_vector_alloc(3);

  for (int ii =0; ii <3; ii++) {
    
    std::cout << "iter: "<< ii << " Vector:" << gsl_vector_get(test, ii) << "\n";

  }

  return 0;

}


/******************************************************************************
 * Function: testMDSplus
 * Inputs: 
 * Returns: int
 * Description: Testing to see if the MDSplus works
 ******************************************************************************/

int testMDSplus() {

  MDSplus::Tree *tree;
  MDSplus::Data *evalData;
  std::string experiment = "fuze";
  int shotNumber = 0;


  char experimentName[experiment.length()];

  strcpy(experimentName, experiment.c_str());

  /*
   * Trying to open an experiment and shot number
   */
  try {

    tree = new MDSplus::Tree(experimentName, shotNumber);

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << experiment  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    exit(0);

  } catch(...) {

    std::cout << "Default error";

  }

  return 0;

}


/******************************************************************************
 * Function: testMDSplusPlotData
 * Inputs: 
 * Returns: int
 * Description: Testing to see if the MDSplus works
 ******************************************************************************/

int testMDSplusPlotData() {

  MDSplus::Tree *tree;
  MDSplus::Data *evalData;
  std::string experiment = "fuze";
  int shotNumber = 0;


  char experimentName[experiment.length()];

  strcpy(experimentName, experiment.c_str());

  /*
   * Trying to open an experiment and shot number
   */
  try {

    tree = new MDSplus::Tree(experimentName, shotNumber);

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << experiment  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    exit(0);

  } catch(...) {

    std::cout << "Default error";

  }

  return 0;

}
