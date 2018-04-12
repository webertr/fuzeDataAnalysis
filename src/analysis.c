#include "analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: plotPostShotData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotData(int shotNumber) {

  gsl_vector *data1 = 0,
    *data2 = 0,
    *time = 0;

  initializeMagneticDataAndTime(shotNumber, "\\b_n95_000_sm", &data1, &time);
  initializeMagneticData(shotNumber, "\\b_n10_000_sm", &data2);

  plot2VectorData(time, data1, data2);

  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(time);

  return 0;

}
