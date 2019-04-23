#include "cpp/test.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

/******************************************************************************
 * Function: testMDSplus
 * Inputs: 
 * Returns: int
 * Description: Testing to see if the MDSplus works
 ******************************************************************************/

int testMDSplus() {

  std::string experiment = "fuze";
  int shotNumber = 190416014;

  gsl_vector *time = readMDSplusVectorDim(shotNumber, "\\I_P", "fuze");
  gsl_vector_scale(time, 1E6);
  gsl_vector *ip = readMDSplusVector(shotNumber, "\\I_P", "fuze");
  gsl_vector_scale(ip, 1E-3);

  std::string label1 = "with line lw 3 lc rgb 'black' title 'I_{p}'";
  std::string label2 = "with line lw 3 lc rgb 'red' title 'I_{p}'";
  std::string ipKeyWords = "set xrange[0:100]\n"
    "set title 'I_{P}'\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set yrange[0:]";
  std::string tempDataFile = "data/test.txt";
  std::string tempScriptFile = "data/test.sh";

  plot2VectorData(time, ip, label1, ip, label2, 
		  ipKeyWords, tempDataFile, tempScriptFile);

  plotVectorData(time, ip, label1, ipKeyWords, tempDataFile, tempScriptFile);

  // Freeing GSL vectors
  gsl_vector_free(time);
  gsl_vector_free(ip);

  return 0;

}

