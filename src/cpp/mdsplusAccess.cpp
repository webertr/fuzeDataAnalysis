#include "cpp/mdsplusAccess.h"


/******************************************************************************
 *
 * This is the source file to get mdsplus access to the fuze experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: readDHIMDSplusVector
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: 
 ******************************************************************************/

gsl_vector *readMDSplusVector(int shotNumber, std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::Data *data;
  int numElements;
  double *doubleArray;
  std::string readOnly = "READONLY";
  gsl_vector *nullVec = 0;

  /*
   * Trying to open an experiment and shot number
   */
  try {

    /*
     * The tree name has to match the path environment variable xxx_path
     * fuze_path = fuze.fuze::/usr/local/trees/fuze/newdata/~t
     */
    tree = new MDSplus::Tree(treeName.c_str(), shotNumber, readOnly.c_str());

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return nullVec;

  } catch(...) {

    std::cout << "Default error";
    return nullVec;

  }

  data = MDSplus::execute(nodeName.c_str(), tree);

  doubleArray = data->getDoubleArray(&numElements);

  gsl_vector *retVec = gsl_vector_alloc(numElements);

  int ii;
  for (ii = 0; ii < numElements; ii++) {
    gsl_vector_set(retVec, ii, doubleArray[ii]);
  }

  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  // Delete pointer to memory that mdsplus created. Shouldn't already be deleted?
  delete doubleArray;

  return retVec;

}


/******************************************************************************
 * Function: readDHIMDSplusVectorDim
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: 
 ******************************************************************************/

gsl_vector *readMDSplusVectorDim(int shotNumber, std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::Data *data;
  int numElements;
  double *doubleArray;
  std::string readOnly = "READONLY";
  char buf[1024];
  gsl_vector *nullVec = 0;

  /*
   * Trying to open an experiment and shot number
   */
  try {

    /*
     * The tree name has to match the path environment variable xxx_path
     * fuze_path = fuze.fuze::/usr/local/trees/fuze/newdata/~t
     */
    tree = new MDSplus::Tree(treeName.c_str(), shotNumber, readOnly.c_str());

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return nullVec;

  } catch(...) {

    std::cout << "Default error";
    return nullVec;
  }

  snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)",nodeName.c_str());
  data = MDSplus::execute(buf, tree);

  doubleArray = data->getDoubleArray(&numElements);

  gsl_vector *retVec = gsl_vector_alloc(numElements);

  int ii;
  for (ii = 0; ii < numElements; ii++) {
    gsl_vector_set(retVec, ii, doubleArray[ii]);
  }

  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  // Delete pointer to memory that mdsplus created. Shouldn't already be deleted?
  delete doubleArray;

  return retVec;

}


/******************************************************************************
 * Function: getCurrentPulseNumber
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 ******************************************************************************/

int getCurrentPulseNumber() {

  MDSplus::Data *data;
  std::string getShot = "current_shot('fuze')";
  int pulseNumber;
  
  data = MDSplus::execute(getShot.c_str());
  pulseNumber = data->getLong();

  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  /* return pulse number */
  return pulseNumber;

}


/******************************************************************************
 * Function: readDHIMDSplusDouble
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: 
 ******************************************************************************/

double readMDSplusDouble(int shotNumber, std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::Data *data;
  double doubleValue;
  std::string readOnly = "READONLY";

  /*
   * Trying to open an experiment and shot number
   */
  try {

    /*
     * The tree name has to match the path environment variable xxx_path
     * fuze_path = fuze.fuze::/usr/local/trees/fuze/newdata/~t
     */
    tree = new MDSplus::Tree(treeName.c_str(), shotNumber, readOnly.c_str());

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return -1;

  } catch(...) {

    std::cout << "Default error";
    return -1;

  }

  data = MDSplus::execute(nodeName.c_str(), tree);
  doubleValue = data->getDouble();
  
  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  return doubleValue;

}


/******************************************************************************
 * Function: writeDHIMDSplusDouble
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: Write double to the tree
 ******************************************************************************/

int writeMDSplusDouble(double valIn, int shotNumber, 
		       std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::TreeNode *node;
  std::string readOnly = "NORMAL";
  MDSplus::Float64 *data = new MDSplus::Float64(valIn);

  /*
   * Trying to open an experiment and shot number
   */
  try {

    /*
     * The tree name has to match the path environment variable xxx_path
     * fuze_path = fuze.fuze::/usr/local/trees/fuze/newdata/~t
     */
    tree = new MDSplus::Tree(treeName.c_str(), shotNumber, readOnly.c_str());

  } catch(MDSplus::MdsException &exc) {

    std::cout << "Cannot open tree " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return -1;

  } catch(...) {

    std::cout << "Default error";
    return -1;

  }

  /* Getting node from tree */
  try {
    node = tree->getNode(nodeName.c_str());
  } catch(MDSplus::MdsException &exc) {
    std::cout << "Cannot get node " << treeName  
	      << " shot " << shotNumber
	      << " Node " << nodeName << ": " << exc.what() << std::endl;
    return -1;

  } catch(...) {

    std::cout << "Default error";
    return -1;

  }
    
  /* Putting data */
  node->putData(data);
  
  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  return 1;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testMDSplusRead();
static int testMDSplusWriteDouble();

int testMDSplusAccess() {

  if (!testMDSplusRead()) {
    std::cout << "Test MDSplus Read test Failed\n";
    return -1;
  }

  if (!testMDSplusWriteDouble()) {
    std::cout << "Write double test Failed\n";
    return -1;
  }

  std::cout << "All MDSplus tests passed\n";

  return 0;

}

static int testMDSplusRead() {

  int shotNumber = 180517018; // DHI Time = 59E-5
  shotNumber = 190501030;     // DHI Time = 4.4E-5
  double dhiTime = 4.4E-5;

  double dhiTimeRead = readMDSplusDouble(shotNumber,"\\T_DHI_GATE" ,"fuze");

  if ( (abs(dhiTime - dhiTimeRead) > 1E-8) ) {
    std::cout << "MDSplus Read Test Failed\n"
	      << "DHI Time should be " << dhiTime <<" but is " << dhiTimeRead << "\n";
    return -1;
  }

  return 1;

}


static int testMDSplusWriteDouble() {
  
  int shotNumber = 123456; // Test shot number for FuZE tree
  std::string nodeName = "\\ICCD_01:X";
  std::string treeName = "fuze"; 
  double val = 0.15;
  double ret;

  writeMDSplusDouble(val, shotNumber, nodeName.c_str(), treeName.c_str());

  ret = readMDSplusDouble(shotNumber, nodeName.c_str(), treeName.c_str());

  if ( (abs(ret - val) > 1E-8) ) {
    std::cout << "Test failed. Value should be " << val << " but read back: " << ret << "\n";
    return 0;
  }

  return 1;
  
}
