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

  gsl_vector_view radialProfileVec;
  gsl_vector *testVec;
  
  double offset;
  
  int ii, jj;
  for (jj = 0; jj < numCols; jj++) {

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
  

  int leftLength;

  for (jj = 0; jj < numCols; jj++) {

    radialProfileVec = gsl_matrix_column(radialProfile, jj);
    
    testVec = testMatrixMult(projectMatrix, &radialProfileVec.vector);
    leftLength = (int) (20 + (float) jj /numCols*60);
    offset = (double) (0 + (double) jj / numCols * 0.005);
    
    for (ii = 0; ii < numRows; ii++) {
    
      if (ii < leftLength) {

	gsl_matrix_set(testData, ii, jj, 
		       gsl_vector_get(testVec, leftLength - ii - 1)+offset);

      }
      else {

	gsl_matrix_set(testData, ii, jj, 
		       gsl_vector_get(testVec, ii - leftLength)+offset);
      }
    }
  }

  param.deltaN = 0.05;
  param.centroidNum = 10;
  param.offsetIter = 20;


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

