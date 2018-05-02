#include "testAbelInversionDHI.h"


static gsl_vector *testMatrixMult(gsl_matrix *mInput, gsl_vector *vInput);


int testInvertImageDHI() {
  
  int numRows = 100,
    numCols = 100;

  gsl_matrix *testData = gsl_matrix_alloc(numRows, numCols);

  double val;

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.numRows = numRows;
  param.numCols = numCols;
  
  /* Get the matrix that will project the radial profile to the line integrated density */
  gsl_matrix *projectMatrix = getProjectMatrixDHI(param.numRows, param.deltaY);

  gsl_matrix *radialProfile = gsl_matrix_alloc(numRows, numCols);

  gsl_vector *radialProfileVec = gsl_vector_alloc(numCols);
  
  gsl_vector *testVec;
  gsl_vector *xVec = gsl_vector_alloc(numCols);
  
  int ii, jj;
  for (jj = 0; jj < numCols; jj++) {

    gsl_vector_set(xVec, jj, (double) jj);
    
    for (ii = 0; ii < numRows; ii++) {

      val = gsl_sf_exp(-gsl_pow_2(ii/20.0));

      /*
	if (ii < 20) {
	val = 1.0;
	} else {
	val = 0.0;
	}
      */
    
      gsl_matrix_set(radialProfile, ii, jj, val);

    }
  }  
  

  int center;

  for (jj = 0; jj < numCols; jj++) {

    gsl_matrix_get_col(radialProfileVec, radialProfile, jj);
    testVec = testMatrixMult(projectMatrix, radialProfileVec);
    
    center = (int) (50.0 + ((float) jj / numCols)*80 - 40);
    
    for (ii = 0; ii < numRows; ii++) {

      gsl_matrix_set(testData, ii, jj, gsl_vector_get(testVec, abs(center - ii)));
      
    }
  }

  param.deltaN = 0.01;
  param.centroidNum = 10;
  param.offsetIter = 10;

  plotImageData(testData);
    
  gsl_matrix *invertedImage = invertImageDHI(testData, &param);

  saveImageData(invertedImage, param.fileFullInvert);
  
  plotMatrixDataFile(param.fileLeftInvert, 50, "");
  
  plotImageDataFile(param.fileFullInvert, "");
  
  return 0;

}


static gsl_vector *testMatrixMult(gsl_matrix *mInput, gsl_vector *vInput) {

  int numRows = mInput -> size1,
    numCols = mInput -> size2;

  gsl_vector *vecRet = gsl_vector_alloc(numRows);

  double val;

  int ii, jj;
  for (jj = 0; jj < numCols; jj++) {

    val = 0;
    for (ii = 0; ii < numRows; ii++) {

      val = val + gsl_vector_get(vInput, ii) * gsl_matrix_get(mInput, jj, ii);
      
    }

    gsl_vector_set(vecRet, jj, val);

  }
		     
  return vecRet;

}

