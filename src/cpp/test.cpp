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
  std::string experiment = "fuze";
  char experimentName[experiment.length()];
  strcpy(experimentName, experiment.c_str());
  int shotNumber = 190416014;
  MDSplus::Data *data;
  MDSplus::Data *dataT;
  int numElements;
  double *doubleArray;
  double *timeArray;

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

  data = MDSplus::execute("\\I_P", tree);
  dataT = MDSplus::execute("DIM_OF(\\I_P)", tree);

  doubleArray = data->getDoubleArray(&numElements);
  timeArray = dataT->getDoubleArray(&numElements);

  gsl_vector *timeVector = gsl_vector_alloc(numElements);
  gsl_vector *ipVector = gsl_vector_alloc(numElements);

  int ii;
  for (ii = 0; ii < numElements; ii++) {
    gsl_vector_set(timeVector, ii, timeArray[ii]);
    gsl_vector_set(ipVector, ii, doubleArray[ii]);
  }

  std::string label1 = "with line lw 3 lc rgb 'black' title 'I_{p}'";
  std::string label2 = "with line lw 3 lc rgb 'red' title 'I_{p}'";
  std::string keywords = "set title 'Corn dogs'\n";
  std::string tempDataFile = "data/test.txt";
  std::string tempScriptFile = "data/test.sh";

  plot2VectorData(timeVector, ipVector, label1.c_str(), ipVector, label2.c_str(), 
		  keywords.c_str(), tempDataFile.c_str(), tempScriptFile.c_str());

  // Freeing GSL vectors
  gsl_vector_free(timeVector);
  gsl_vector_free(ipVector);

  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);
  MDSplus::deleteData(dataT);
 
  // Tree objects use C++ delete
  delete tree;

  // Delete pointer to memory that mdsplus created. Shouldn't already be deleted?
  delete doubleArray;
  delete timeArray;

  return 0;

}

