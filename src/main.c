#include "dataAnalysis.h"


/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: 
 ******************************************************************************/

int main(int argc, char *argv[]) {

  /* 
   * struct containing all the holography parameters.
   * Setting to default values.
   */
  dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");

  printf("Data: %d, %f\n", data->length, data->deltaT);

  int ii;
  for (ii =0; ii < data->length; ii++) {

    printf("Data: %f\n", gsl_vector_get(data->data, ii));
    
  }
  
  return 0;

}
