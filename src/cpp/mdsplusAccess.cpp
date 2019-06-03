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
     * If not data in tree, %TREE-E-NODATA
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

  try {
    
    data = MDSplus::execute(nodeName.c_str(), tree);

  } catch (MDSplus::MdsException &exc) {
      
    std::cout << "Error in executing " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return nullVec;

  }

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

  try {
    
    data = MDSplus::execute(buf, tree);

  } catch (MDSplus::MdsException &exc) {
      
    std::cout << "Error in executing " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return nullVec;

  }

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
 * Function: readDHIMDSplusMatrix
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: 
 ******************************************************************************/

gsl_matrix_ushort *readMDSplusMatrix(int shotNumber, std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::Data *data;
  int numElements;
  int ii, jj;
  int numRows, numCols;
  std::string readOnly = "READONLY";
  gsl_matrix_ushort *nullMat = 0;
  int numDim;
  int *shape;
  unsigned short *ushortArray;

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
    return nullMat;

  } catch(...) {

    std::cout << "Default error";
    return nullMat;

  }

  try {
    
    data = MDSplus::execute(nodeName.c_str(), tree);

  } catch (MDSplus::MdsException &exc) {
      
    std::cout << "Error in executing " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return nullMat;

  }

  /* get shape */
  shape = data->getShape(&numDim);

  if (numDim != 2) {
    std::cout << "Error in reading matrix from mdsplus. Data not 2D image\n";
    return nullMat;
  }

  numRows = shape[0];
  numCols = shape[1];

  /* Getting unsigned short integer data */
  ushortArray = data->getShortUnsignedArray(&numElements);

  /* Breaking down into 2d array */
  gsl_matrix_ushort *retMat = gsl_matrix_ushort_alloc(numRows, numCols);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_ushort_set(retMat, ii, jj, ushortArray[jj+ii*numCols]);
    }
  }

  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  // Delete pointer to memory that mdsplus created. Shouldn't already be deleted?
  delete shape;

  // Should already be freed?
  free(ushortArray);

  return retMat;

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

  try {
    
    data = MDSplus::execute(nodeName.c_str(), tree);

  } catch (MDSplus::MdsException &exc) {
      
    std::cout << "Error in executing " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return -1;

  }

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
 * Function: writeDHIMDSplusVector
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: Write vector to the tree
 ******************************************************************************/

int writeMDSplusVector(gsl_vector *vecIn, int shotNumber, 
		       std::string nodeName, std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::TreeNode *node;
  std::string readOnly = "NORMAL";
  MDSplus::Float64Array *data = new MDSplus::Float64Array(vecIn->data, vecIn->size);

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
 * Function: writeDHIMDSplusMatrix
 * Inputs: int, char *, char *, char *
 * Returns: gsl_vector *
 * Description: Write matrix to the tree. Data will get converted or casted
 * to unsigned short even though it expects doubles
 ******************************************************************************/

int writeMDSplusMatrix(gsl_matrix_ushort *matIn, int shotNumber, std::string nodeName, 
		       std::string treeName) {

  MDSplus::Tree *tree;
  MDSplus::TreeNode *node;
  std::string readOnly = "NORMAL";
  int numRows = matIn->size1;
  int numCols = matIn->size2;
  int dims[2] = {numRows, numCols};
  int ii, jj;

  /* 
   * gsl_matrix_get(m,i,j) -> m->data[i * m->tda + j]
   * where m->tda is the physical length of a row, or the number of columns with a memory
   * correction factor?
   * m->size1 = the number of rows
   * m->size2 = the number of columns
   * For mdsplus, 3x2 integer array [[1,2],[11,22],[111,222]]
   * int arr[6]={1,2,11,22,111,222};
   * int dims[2] = {3,2};
   * 3 = number of rows, 2 = number of cols
   * Data *arrD = new Int32Array(arr, 2, dims);
   * This wrote as [0, 2, 4, ...], when I wrote it,
   * [[0, 1, 2, ...], [2, 3, 4, ...], ...]
   * with [1,2,3...] being a row
   * So it wrote columns first. It seems like it should write the rows first
   */
  unsigned short *dataShort = (unsigned short *)malloc(numRows*numCols * sizeof(short));
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      dataShort[jj+ii*numCols] = gsl_matrix_ushort_get(matIn, ii, jj);
    }
  }

  MDSplus::Uint16Array *data = new MDSplus::Uint16Array((unsigned short *const)dataShort, 
							2, dims);

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

    free(data);
    std::cout << "Cannot open tree " << treeName  
	      << " shot " << shotNumber << ": " << exc.what() << std::endl;
    return -1;

  } catch(...) {

    free(data);
    std::cout << "Default error";
    return -1;

  }

  /* Getting node from tree */
  try {
    node = tree->getNode(nodeName.c_str());
  } catch(MDSplus::MdsException &exc) {

    free(data);
    std::cout << "Cannot get node " << treeName  
	      << " shot " << shotNumber
	      << " Node " << nodeName << ": " << exc.what() << std::endl;
    return -1;

  } catch(...) {

    free(data);
    std::cout << "Default error";
    return -1;

  }
    
  /* Putting data */
  node->putData(data);
  
  // Data objects must be freed via routine deleteData()
  MDSplus::deleteData(data);

  // Tree objects use C++ delete. Can't find a MDSplus::deleteTree()
  delete tree;

  // Freeing memory
  free(dataShort);

  return 1;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testMDSplusRead();
static int testMDSplusWriteDouble();
static int testMDSplusWriteVector();
static int testMDSplusWriteMatrix();

int testMDSplusAccess() {

  if (!testMDSplusRead()) {
    std::cout << "Test MDSplus Read test Failed\n";
    return -1;
  }

  if (!testMDSplusWriteDouble()) {
    std::cout << "Write double test Failed\n";
    return -1;
  }

  if (!testMDSplusWriteVector()) {
    std::cout << "Write vector test Failed\n";
    return -1;
  }

  if (!testMDSplusWriteMatrix()) {
    std::cout << "Write matrix test Failed\n";
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


static int testMDSplusWriteVector() {
  
  int shotNumber = 123456; // Test shot number for FuZE tree
  std::string nodeName = "\\ICCD:LAMBDA";
  std::string treeName = "fuze"; 
  int vecSize = 512;
  gsl_vector *vecIn = gsl_vector_alloc(vecSize);
  gsl_vector *vecRet;
  int ii;

  for (ii = 0; ii < vecSize; ii++) {
    gsl_vector_set(vecIn, ii, ii*2);
  }
  
  writeMDSplusVector(vecIn, shotNumber, nodeName.c_str(), treeName.c_str());

  vecRet = readMDSplusVector(shotNumber, nodeName.c_str(), treeName.c_str());

  if ( (abs(gsl_vector_get(vecIn,0) - gsl_vector_get(vecRet,0)) > 1E-8) ||
       (abs(gsl_vector_get(vecIn,4) - gsl_vector_get(vecRet,4)) > 1E-8) ||
       (abs(gsl_vector_get(vecIn,8) - gsl_vector_get(vecRet,8)) > 1E-8) ||
       (abs(gsl_vector_get(vecIn,12) - gsl_vector_get(vecRet,12)) > 1E-8) ||
       (vecIn->size != vecRet->size) ) {
    std::cout << "Test failed. Vectors not equal\n";

    return 0;
  }

  return 1;
  
}


static int testMDSplusWriteMatrix() {
  
  int shotNumber = 123456; // Test shot number for FuZE tree
  std::string nodeName = "\\ICCD:RAW";
  std::string treeName = "fuze"; 
  int numRows = 10;
  int numCols = 15;
  gsl_matrix_ushort *matIn = gsl_matrix_ushort_alloc(numRows, numCols);
  gsl_matrix_ushort *matRet;
  int ii, jj;

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_ushort_set(matIn, ii, jj, ii);
    }
  }
  
  writeMDSplusMatrix(matIn, shotNumber, nodeName.c_str(), treeName.c_str());

  matRet = readMDSplusMatrix(shotNumber, nodeName.c_str(), treeName.c_str());

  if ( (gsl_matrix_ushort_get(matIn,0, 1) != gsl_matrix_ushort_get(matRet,0, 1) ) ||
       (gsl_matrix_ushort_get(matIn,4, 1) != gsl_matrix_ushort_get(matRet,4, 1) ) ||
       (gsl_matrix_ushort_get(matIn,8, 2) != gsl_matrix_ushort_get(matRet,8,2) ) ||
       (gsl_matrix_ushort_get(matIn,2, 4) != gsl_matrix_ushort_get(matRet,2, 4) ) ||
       (matIn->size1 != matRet->size1) ||
       (matIn->size2 != matRet->size2) ) {

    std::cout << "Test failed. Matrices not equal\n";

    return 0;
  }

  return 1;
  
}
