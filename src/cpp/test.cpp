#include "cpp/test.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: hologramAnalysis
 * Inputs: 
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int testGSL() {

  gsl_vector *test = gsl_vector_alloc(3);

  for (int ii =0; ii <3; ii++) {
    
    std::cout << "iter: "<< ii << " Vector:" << gsl_vector_get(test, ii) << "\n";

  }

  return 0;

}
