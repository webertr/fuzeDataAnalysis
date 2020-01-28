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
  FILE *fp;
  
  if ( (fp = fopen(fileName.c_str(), "r")) ) {
    fclose(fp);
    if (remove(fileName.c_str()) != 0) {
      printf("Unable to delete the file");
    }
  }
  
  
  fp = fopen(fileName.c_str(), "w");
  
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

  FILE *fp;
  
  if ( (fp = fopen(fileName.c_str(), "r")) ) {
    fclose(fp);
    if (remove(fileName.c_str()) != 0) {
      printf("Unable to delete the file");
    }
  }
  
  fp = fopen(fileName.c_str(), "w");
  
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
 * Function: read2VectorData
 * Inputs: gsl_vector*, char *
 * Returns: int
 * Description: This will first delete the file that it is passed, then save 
 * the 1 vectors in a single text file for gnuplot to plot 
 ******************************************************************************/

bool read2VectorData(gsl_vector **vecX, gsl_vector **vecY, std::string fileName) {

  double xVal, yVal;
  int ii;
  int vecSize;
  
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

  *vecX = gsl_vector_alloc(vecSize);
  *vecY = gsl_vector_alloc(vecSize);
  
  fileStream.clear();
  fileStream.seekg(0, std::ios_base::beg);

  ii=0;
  while(fileStream >> xVal >> yVal) {
    gsl_vector_set(*vecX, ii, xVal);
    gsl_vector_set(*vecY, ii, yVal);
    ii++;
  }

  return true;

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

  int ii;
  
  std::string fileNameY = "data/testX.txt";
  std::string fileNameXY = "data/testXY.txt";

  gsl_vector *testWriteX = gsl_vector_alloc(3);
  gsl_vector *testWriteY = gsl_vector_alloc(3);
  
  gsl_vector_set(testWriteY, 0, 1.2);
  gsl_vector_set(testWriteY, 1, 4);
  gsl_vector_set(testWriteY, 2, 6);
  
  gsl_vector_set(testWriteX, 0, 1);
  gsl_vector_set(testWriteX, 1, 2);
  gsl_vector_set(testWriteX, 2, 3);

  saveVectorData(testWriteY, fileNameY);
  save2VectorData(testWriteX, testWriteY, fileNameXY);
  
  gsl_vector *testYRet = readVectorData(fileNameY);
  gsl_vector *xRet, *yRet;
  
  if ( !read2VectorData(&xRet, &yRet, fileNameXY) ){
    return false;
  }

  for (ii = 0; ii < 3; ii++) {
    if ( !((gsl_vector_get(xRet, ii) > (gsl_vector_get(testWriteX, ii)-0.1)) &&
	   (gsl_vector_get(xRet, ii) < (gsl_vector_get(testWriteX, ii)+0.1))) ) {
      return false;
    }
    if ( !((gsl_vector_get(yRet, ii) > gsl_vector_get(testWriteY, ii)-0.1) &&
	  (gsl_vector_get(yRet, ii) < gsl_vector_get(testWriteY, ii)+0.1)) ) {
      return false;
    }
  }
  
  if ( !((gsl_vector_get(testYRet, 0) > 1.1) &&
	 (gsl_vector_get(testYRet, 0) < 1.3)) ) {
    std::cout << gsl_vector_get(testYRet, 0) << "not equal to 1.2";
    return false;
  }

  if ( !((gsl_vector_get(testYRet, 1) > 3.9) &&
	 (gsl_vector_get(testYRet, 1) < 4.1)) ) {
    std::cout << gsl_vector_get(testYRet, 1) << "not equal to 4";
    return false;
  }

  if ( !((gsl_vector_get(testYRet, 2) > 5.9) &&
	 (gsl_vector_get(testYRet, 2) < 6.1)) ) {
    std::cout << gsl_vector_get(testYRet, 2) << "not equal to 6";
    return false;
  }
  
  return true;
    
}
  
