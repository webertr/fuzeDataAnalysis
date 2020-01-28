#include "cpp/save.h"


/******************************************************************************
 * Function: save2VectorData
 * Inputs: gsl_vector*, gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 2 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, std::string fileName) {

  int ii;
  
  if (remove(fileName.c_str()) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName.c_str(), "w");
  
  if ( (fp == NULL) ) {
    printf("Error opening files to save for 2 vector save!\n");
    exit(1);
  }

  for (ii = 0; ii < (int) vec1In->size; ii++) {
    fprintf(fp, "%g\t%g\n", gsl_vector_get(vec1In, ii), gsl_vector_get(vec2In, ii));
  }

  fclose(fp);

  return 0;

}

/******************************************************************************
 * Function: saveVectorData
 * Inputs: gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 1 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

int saveVectorData(gsl_vector *vecIn, std::string fileName) {

  int ii;
  
  if (remove(fileName.c_str()) != 0) {
    printf("Unable to delete the file");
  }
  
  FILE *fp = fopen(fileName.c_str(), "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files to save for 1 vector save!\n");
    exit(1);

  }

  for (ii = 0; ii < (int) vecIn->size; ii++) {
    fprintf(fp, "%g\n", gsl_vector_get(vecIn, ii));
  }

  fclose(fp);

  return 0;

}


/******************************************************************************
 * Function: readVectorData
 * Inputs: gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 1 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

gsl_vector *readVectorData(std::string fileName) {

  gsl_vector *retVec;
  double yVal;
  int ii;
  int vecSize;
  
  //std::ifstream fileStream(fileName.c_str());
  std::ifstream fileStream(fileName);

  if (!fileStream.is_open() ) {
    printf("Error opening files to save for vector read!\n");
    exit(1);
  }

  ii = 0;
  while(fileStream >> yVal) {
    ii++;
  }
  vecSize = ii;

  retVec = gsl_vector_alloc(vecSize);
  
  fileStream.clear();
  fileStream.seekg(0, std::ios_base::beg);

  ii=0;
  while(fileStream >> yVal) {
    gsl_vector_set(retVec, ii, yVal);
    ii++;
  }

  return retVec;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/


static bool testReadWriteVector();

/******************************************************************************
 * Function: testSave
 * Inputs: 
 * Returns: 
 * Description: Testing the save.c functions
 ******************************************************************************/

int testSave() {

  if (!testReadWriteVector()) {
    std::cout << "Test read write vector Failed\n";
    return -1;
  }

  std::cout << "All save.cpp tests passed\n";
  
  return 0;
}


static bool testReadWriteVector() {

  gsl_vector *testVec = readVectorData("data/text.txt");

  if ( !((gsl_vector_get(testVec, 0) > 1.1) && (gsl_vector_get(testVec, 0) < 1.3)) ) {
    std::cout << gsl_vector_get(testVec, 0) << "not equal to 1.2";
    return false;
  }

  if ( !((gsl_vector_get(testVec, 1) > 3.9) && (gsl_vector_get(testVec, 1) < 4.1)) ) {
    std::cout << gsl_vector_get(testVec, 1) << "not equal to 4";
    return false;
  }

  if ( !((gsl_vector_get(testVec, 2) > 5.9) && (gsl_vector_get(testVec, 2) < 6.1)) ) {
    std::cout << gsl_vector_get(testVec, 2) << "not equal to 6";
    return false;
  }
  
  return true;
    
}
  
