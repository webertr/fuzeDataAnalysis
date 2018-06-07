#include "DHI/abelInversion.h"

static int getRadialDensityProfileDHI(gsl_vector* leftCrossSection, gsl_vector* rightCrossSection, 
				      gsl_vector* crossSection, int* centroidLocation,
				      gsl_matrix* projectMatrix, int centroidIterations,
				      int centroidIndexTest, int colNumber, 
				      holographyParameters* param);
static int findDensityOffsetDHI(gsl_vector *largeCrossSection, gsl_vector *smallCrossSection,
				int length, holographyParameters *param);
static int solveRightSystemLinearEqDHI(gsl_matrix *mInput, gsl_vector *vInput, 
				       gsl_vector* vOutput, int rightSize);
static int solveLeftSystemLinearEqDHI(gsl_matrix *mInput, gsl_vector *vInput, 
				      gsl_vector *vOutput, int leftSize);
static gsl_matrix* getProjectMatrixDHI(int sizeM, double res);
static int axialVariationCorrectionDHI(gsl_matrix *leftDensityProfile, 
				       gsl_matrix *rightDensityProfile, gsl_matrix *imageM, 
				       gsl_vector *centroidLocation, holographyParameters* param);
static gsl_matrix *getErrorBarsDHI(gsl_matrix *projectMatrix, gsl_matrix *leftProfile,
				   gsl_matrix *rightProfile, gsl_matrix *image);


/******************************************************************************
 * Function: invertImage
 * Inputs: gsl_matrix*
 * Returns: int
 * Description: For an onion model, with each shell/layer being given by the column
 * number, j, and each impact parameter given by the row number, i, find the
 * matrix that represents the amount each shell contributes to the impact
 * parameter, i. M(i,j), where M = 0 for j<i because if the impact parameter
 * is greater then the shell, it can't have any contribution
 ******************************************************************************/

gsl_matrix *invertImageDHI(gsl_matrix* imageM, holographyParameters* param) {

  int ii, jj, centroid,
    maxIndex,
    centroidIndexTest,
    centroidIterations,                // For loop to scan center vals to find best cen of plasma
    numCentroids = param->centroidNum, // +/- number to vary around max val to find the true center
    numRows = imageM->size1,           // Number of rows 
    numCols = imageM->size2;           // Number of cols

  /* The matrices to hold the final radial profiles, both left and right, for each column */
  gsl_matrix *leftDensityProfile = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix *rightDensityProfile = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix *fullDensityProfile = gsl_matrix_alloc(numRows, numCols);

  /* This is the centroid location in absolute pixel-space */
  gsl_vector *centroidLocation = gsl_vector_alloc(numCols),
    *crossSection = gsl_vector_alloc(numRows),       // The crosss section through image
    *leftCrossSection = gsl_vector_alloc(numRows),   // Vec of the leftDensityProfile
    *rightCrossSection = gsl_vector_alloc(numRows);  // Vec of the rightDensityProfile


  /* Get the matrix that will project the radial profile to the line integrated density */
  gsl_matrix *projectMatrix = getProjectMatrixDHI(numRows, param->deltaY);

  /*
   * Starting to iterate through the cross sections. Each
   * value represents line integrated data. Each cross section
   * is represented by a single column
   */
  for (jj = 0; jj < numCols; jj++) {

    /* The cross section of the image */
    gsl_matrix_get_col(crossSection, imageM, jj);

    /* Subtract the minimum value */
    
    /* Getting index with maximum line-integrated density of cross section*/
    maxIndex = gsl_vector_max_index(crossSection);

    /* This will be the first index to start on for a guess at the plasma center */
    centroidIndexTest = maxIndex-numCentroids;
    
    /* 
     * Reset the centroid iterations if it was reduces to accomodate centroids close
     * to the bounds of the data
     */
    centroidIterations = numCentroids;

    /* 
     * If this centroid is near the lower, set continue to next for loop
     * after setting the final matrices needed
     */
    if ( centroidIndexTest <= 0 ) {

      /* 
       * leftDensityProfile, rightDensityProfile need to have set to a "NaN" (Not a Number) value,
       */
      gsl_vector_set_all(leftCrossSection, GSL_NAN);
      gsl_vector_set_all(rightCrossSection, GSL_NAN);

      /* This is the centroid location in absolute pixel-space */
      gsl_vector_set(centroidLocation, jj, 0);

      continue;
      
    }

    /* 
     * If this centroid is near the upper, set continue to next for loop
     * after setting the final matrices needed
     */
    if ( (maxIndex+numCentroids) >= (numRows-1) ) {

      /* 
       * leftDensityProfile, rightDensityProfile need to have set to a "NaN" (Not a Number) value,
       */
      gsl_vector_set_all(leftCrossSection, GSL_NAN);
      gsl_vector_set_all(rightCrossSection, GSL_NAN);

      /* This is the centroid location in absolute pixel-space */
      gsl_vector_set(centroidLocation, jj, numRows-1);

      continue;
      
    }

    centroidIterations = 2*numCentroids;

    /*
     * This function will find the true center by iterating through different center lines,
     * then it will set the values for leftDensityProfile, and rightDensityProfile, 
     * the left and right radial densities. 
     * Each column will represent a radial densities generated from 1 cross-section.
     * It will also set the centroidLocation vector which represents the plasma center in
     * each cross section
     */
    getRadialDensityProfileDHI(leftCrossSection, rightCrossSection, crossSection, &centroid,
			       projectMatrix, centroidIterations, centroidIndexTest, jj, param);

    gsl_vector_set(centroidLocation, jj, centroid);
    
    gsl_matrix_set_col(leftDensityProfile, jj, leftCrossSection);
    gsl_matrix_set_col(rightDensityProfile, jj, rightCrossSection);
    
  }


  /*
   * This will go through and correct for any axial phase variations. It will basically, for
   * the longer of the left/right profiles, take edge value of the line integrated density,
   * then convert it to density (divide by maximum length at that axial point before the circle
   * 2*(sqrt(R^2 - y^2), and add that to both the left and right radial density profiles.
   * I don't know if this is necessary.
   */
  if (param->axialCorrection == 1) {

    axialVariationCorrectionDHI(leftDensityProfile, rightDensityProfile, imageM,
				centroidLocation, param);

  }

  /* 
   * Assemblying full density profile image 
   */
  for (jj = 0; jj < numCols; jj++) {
    gsl_matrix_get_col(leftCrossSection, leftDensityProfile, jj);
    gsl_matrix_get_col(rightCrossSection, rightDensityProfile, jj);
    centroid = (int) gsl_vector_get(centroidLocation, jj);

    if ( (centroid == 0) || (centroid == (numRows - 1))) {
      continue;
    }
    for (ii = centroid; ii > 0; ii--) {
      gsl_matrix_set(fullDensityProfile, ii, jj,
		     gsl_vector_get(leftCrossSection, centroid-ii));
    }
    for (ii = centroid+1; ii < numRows; ii++) {
      gsl_matrix_set(fullDensityProfile, ii, jj,
		     gsl_vector_get(rightCrossSection, ii-centroid));
    }
  }

  
  /* Adding in first row of postion information */
  gsl_matrix *leftDensityProfileTemp = gsl_matrix_alloc(numRows, numCols+1);
  gsl_matrix *rightDensityProfileTemp = gsl_matrix_alloc(numRows, numCols+1);
  gsl_matrix_view matrixViewLeftTemp = gsl_matrix_submatrix(leftDensityProfileTemp, 
							    0, 1, numRows, numCols);
  gsl_matrix_view matrixViewRightTemp = gsl_matrix_submatrix(rightDensityProfileTemp, 
							     0, 1, numRows, numCols);

  gsl_matrix_memcpy(&matrixViewLeftTemp.matrix, leftDensityProfile);
  gsl_matrix_memcpy(&matrixViewRightTemp.matrix, rightDensityProfile);

  for (ii = 0; ii < numRows; ii++) {
    gsl_matrix_set(leftDensityProfileTemp, ii, 0, ii*param->deltaY);
    gsl_matrix_set(rightDensityProfileTemp, ii, 0, ii*param->deltaY);
  }


  /* 
   * Calculating the error bars by forward projecting the average of the left and right
   * profiles, and then finding the difference between that forward projected image
   * and the passed image. Then, abel inverting that difference
   */
  gsl_matrix *errorBars = getErrorBarsDHI(projectMatrix, leftDensityProfile, rightDensityProfile,
					  imageM);  


  /*
   * Saving data, leftDensityProfile, rightDensityProfile, and the centroidLocation
   */
  saveMatrixData(leftDensityProfileTemp, param->fileLeftInvert);
  saveMatrixData(rightDensityProfileTemp, param->fileRightInvert);
  saveMatrixData(errorBars, param->fileError);
  saveVectorData(centroidLocation, param->fileCentroid);

  /* Deleting vectors and matrices */
  gsl_vector_free(centroidLocation);
  gsl_vector_free(crossSection);
  gsl_vector_free(leftCrossSection);
  gsl_vector_free(rightCrossSection);
  gsl_matrix_free(leftDensityProfile);
  gsl_matrix_free(rightDensityProfile);
  gsl_matrix_free(leftDensityProfileTemp);
  gsl_matrix_free(rightDensityProfileTemp);
  gsl_matrix_free(projectMatrix);
  gsl_matrix_free(errorBars);
  
  return fullDensityProfile;

}


/******************************************************************************
 * Function: getRadialDensityProfileDHI
 * Inputs: gsl_vector*, gsl_vector*, gsl_vector*, gsl_vector*, gsl_matrix*, 
 * gsl_matrix*, int, int
 * Returns: int
 * Description: This function will find the true center by iterating through 
 * different center lines, then, it will set the values for den_num_l, and 
 * den_num_r, the left and right radial densities. Each column will represent 
 * a radial densities generated from 1 cross-section. It will also set the 
 * centroid_abs vector which represents the plasma center in each cross section
 ******************************************************************************/

static int getRadialDensityProfileDHI(gsl_vector* leftCrossSection, gsl_vector* rightCrossSection, 
				      gsl_vector* crossSection, int* centroidLocation,
				      gsl_matrix* projectMatrix, int centroidIterations,
				      int centroidIndexTest, int colNumber, 
				      holographyParameters* param) {

  int ii,jj,
    numRows = crossSection->size,
    leftSize,
    rightSize,
    sizeTot,
    minRightSize,
    minLeftSize;

  gsl_vector *leftCrossSectionTemp = gsl_vector_alloc(leftCrossSection->size);
  gsl_vector *rightCrossSectionTemp = gsl_vector_alloc(rightCrossSection->size);

  double checkSum,
    minCheckSum = DBL_MAX,             // Minimum Check sum value. Set to max double
    val1;

  /*
   * This for loop will go through and try a plasma center at a number of
   * different values and see if which ones fits best, then it will set
   */
  for (ii = 0; ii < centroidIterations; ii++) {

    leftSize = centroidIndexTest+1;
    rightSize = numRows-(centroidIndexTest+1);

    /*
     * Solves the linear system of equations that is, a = M x b
     * where a is the line integrated density vector, and
     * b is the radial density profile, and M is the matrix 
     * that projects the radial density onto the line integrated density profile
     */
    solveLeftSystemLinearEqDHI(projectMatrix, crossSection, leftCrossSectionTemp, leftSize);
    solveRightSystemLinearEqDHI(projectMatrix, crossSection, rightCrossSectionTemp, rightSize);

    /* 
     * The shorter profile will have a large offset, and we want to vary the density
     * offset and find the one that minimizes the L2 value of the difference between the two
     * vectors, then apply that offset. Assuming cylindrical symetry, the L2 of the diff
     * should be equal to zero. We are doing this to try and maximize that symetry which
     * should be there. When taking the L2 norm later in this loop, you'll have to 
     * know the smallest size to loop over
     */
    if (leftSize > rightSize) {
      sizeTot = rightSize;
      findDensityOffsetDHI(leftCrossSectionTemp, rightCrossSectionTemp, sizeTot, param);
    }
    else {
      sizeTot = leftSize;
      findDensityOffsetDHI(rightCrossSectionTemp, leftCrossSectionTemp, sizeTot, param);
    }

    /* Zeroing check sum value */
    checkSum = 0;

    /*
     * This takes a running check sum total of the
     * square of the difference between the large
     * vector and the small vector the offset
     */
    for (jj = 0; jj < sizeTot; jj++) {

      val1 = gsl_vector_get(rightCrossSectionTemp, jj) - gsl_vector_get(leftCrossSectionTemp, jj);
      checkSum = checkSum + gsl_pow_2(val1);
	
    }    

    /* 
     * If the checkSum is at a minimum, then update the minimum centroid value
     */
    if (checkSum < minCheckSum) {

      minCheckSum = checkSum;
      minLeftSize = leftSize;
      minRightSize = rightSize;
      *centroidLocation = leftSize;
      gsl_vector_memcpy(rightCrossSection, rightCrossSectionTemp);
      gsl_vector_memcpy(leftCrossSection, leftCrossSectionTemp);

    }

    /* Iterating the centroid value to to prepare for next loop iteration */
    centroidIndexTest = centroidIndexTest + 1;

  }


  /* 
   * Then just zero out everything else in the vector in case it was written to previously
   */
  for (ii = minLeftSize; ii < numRows; ii++) {
    
    gsl_vector_set(leftCrossSection, ii, 0);
    
  }
  for (ii = minRightSize; ii < numRows; ii++) {
    
    gsl_vector_set(rightCrossSection, ii, 0);
    
  }


  gsl_vector_free(leftCrossSectionTemp);
  gsl_vector_free(rightCrossSectionTemp);
  
  return 0;

}


/******************************************************************************
 * Function: findDensityOffsetDHI
 * Inputs: gsl_vector*, gsl_vector*
 * Returns: double
 * Description: This function should determine what is the appropriate offset
 * for the smaller profile to make it equal to the larger profile. Equal is
 * defined by the L2 norm of the vector, which is the just the vector length.
 ******************************************************************************/

static int findDensityOffsetDHI(gsl_vector *largeCrossSection, gsl_vector *smallCrossSection,
				int length, holographyParameters *param) {

  double  offsetValue,          // The actual offset value
    val1,                       // Value to set the vector to
    minCheckSum = DBL_MAX,      // Minimum Check sum value. Set to max double
    minOffsetValue,             // Minimum offset value
    checkSum,                   // The value to see how the offset worked
    offsetDelta = param->deltaN; // Delta of offset values

  int offsetIteration = param->offsetIter,  // The number of offsets to try
    ii, jj;

  /*
   * Iterating through the different values for the density offset of the smaller profile
   */
  for (ii = -offsetIteration + 1; ii < offsetIteration; ii++) {

    offsetValue = ii*offsetDelta;
    checkSum = 0;

    /*
     * This takes a running check sum total of the square of the difference between the large
     * vector and the small vector the offset
     */
    for (jj = 0; jj < length; jj++) {

      val1 = gsl_vector_get(largeCrossSection, jj) - (gsl_vector_get(smallCrossSection, jj) -
						      offsetValue);
      checkSum = checkSum + gsl_pow_2(val1);
	
    }

    /* 
     * If the checkSum is at a minimum, then update the
     * minimum offset value
     */
    if (checkSum < minCheckSum) {
      
      minOffsetValue  = offsetValue;
      minCheckSum = checkSum;

    }

  }

  /*
   * Setting the offset for the smaller vector
   */
  for (jj = 0; jj < length; jj++) {

    gsl_vector_set(smallCrossSection, jj, gsl_vector_get(smallCrossSection, jj) - minOffsetValue);

  }

  return 0;

}


/******************************************************************************
 * Function: solveRightSystemLinearEqDHI
 * Inputs: gsl_matrix*, gsl_vector*, gsl_vector*
 * Returns: int
 * Description: This will go through and solve the linear system,
 * vInput(ii) = mInput(ii,jj) x vOutput(jj)
 * assuing that mInput is a matrix with all zeros in the lower left half,
 * but not counting the diagonal
 * The left/right is because the "left" line integrated density slice is
 * with the highest density at the end, while the "right" is the opposite.
 ******************************************************************************/

static int solveRightSystemLinearEqDHI(gsl_matrix *mInput, gsl_vector *vInput, 
				       gsl_vector* vOutput, int rightSize) {

  gsl_vector_view vInputView = gsl_vector_subvector(vInput, vInput->size-rightSize-1, rightSize);

  int ii, jj,
    numRows = rightSize,
    numCols = rightSize,
    projectLength = (&vInputView.vector)->size;

  double vec,
    sum1,
    offset = gsl_vector_min(&vInputView.vector);

  (void) offset;

  gsl_matrix_view rightProjectMatrix; // Sub-matrix view of right inversion matrix

  /* Getting matrix to solve equation with */
  rightProjectMatrix = gsl_matrix_submatrix(mInput, 0, 0, rightSize, rightSize);

    
  /* Error checking */
  if (numRows != numCols) {

    fprintf(stderr, "Matrix needs to be square. From solveRightSystemLinearEq.\n");
    exit(0);

  }

  if (projectLength != rightSize) {

    fprintf(stderr, "Vectors need to be the same size. From solveRightSystemLinearEq.\n");
    exit(0);

  }

  if (numRows != projectLength) {

    fprintf(stderr, "Matrix and vectors dims need to match. From solveRightSystemLinearEq.\n");
    exit(0);

  }

  /* Iterating through vector starting at end */
  for (ii = (numRows-1); ii >=0; --ii) {

    /* Zeroing vector sum variable */
    vec = 0;

    /* Running across the columns */
    for (jj = (numCols-1); jj > ii; --jj) {
      
      vec = vec + gsl_matrix_get(&rightProjectMatrix.matrix, ii, jj) *
	gsl_vector_get(vOutput, jj);

    }

    /* Subtracting constant value */
    sum1 = gsl_vector_get(&vInputView.vector, ii) - offset - vec;
    sum1 = sum1 / gsl_matrix_get(&rightProjectMatrix.matrix, ii, ii);

    gsl_vector_set(vOutput, ii, sum1);

  }

  for (ii = numRows; ii < vOutput->size; ii++) {
    gsl_vector_set(vOutput, ii, 0);
  }
  
  return 0;

}


/******************************************************************************
 * Function: solveLeftSystemLinearEqDHI
 * Inputs: gsl_matrix*, gsl_vector*, gsl_vector*
 * Returns: int
 * Description: This will go through and solve the linear system,
 * vInput(ii) = mInput(ii,jj) x vOutput(jj)
 * assuing that mInput is a matrix with all zeros in the lower left half,
 * but not counting the diagonal
 * The left/right is because the "left" line integrated density slice is
 * with the highest density at the end, while the "right" is the opposite.
 ******************************************************************************/

static int solveLeftSystemLinearEqDHI(gsl_matrix *mInput, gsl_vector *vInput, 
				      gsl_vector *vOutput, int leftSize) {
  
  gsl_vector_view vInputView = gsl_vector_subvector(vInput, 0, leftSize);
  
  int ii, jj,
    numRows = leftSize,
    numCols = leftSize,
    projectLength = (&vInputView.vector)->size;

  double vec,
    sum1,
    offset = gsl_vector_min(&vInputView.vector);    

  (void) offset;

  gsl_matrix_view leftProjectMatrix; // Sub-matrix view of right inversion matrix

  /* Getting matrix to solve equation with */
  leftProjectMatrix = gsl_matrix_submatrix(mInput, 0, 0, leftSize, leftSize);


  /* Error checking */
  if (numRows != numCols) {

    fprintf(stderr, "Matrix needs to be square. From solveLeftSystemLinearEq.\n");
    exit(0);

  }

  if (projectLength != leftSize) {

    fprintf(stderr, "Vectors need to be the same size. From solveLeftSystemLinearEq.\n");
    exit(0);

  }

  if (numRows != projectLength) {

    fprintf(stderr, "Matrix and vectors dims need to match. From solveLeftSystemLinearEq.\n");
    exit(0);

  }

  /* Iterating through vector starting at end */
  for (ii = (numRows-1); ii >=0; --ii) {

    /* Zeroing vector sum variable */
    vec = 0;

    /* Running across the columns */
    for (jj = (numCols-1); jj > ii; --jj) {
      
      vec = vec + gsl_matrix_get(&leftProjectMatrix.matrix, ii, jj) *
	gsl_vector_get(vOutput, jj);

    }

    /* Subtracting constant value */
    sum1 = gsl_vector_get(&vInputView.vector, (numRows-1)-ii) - offset - vec;
    sum1 = sum1 / gsl_matrix_get(&leftProjectMatrix.matrix, ii, ii);

    gsl_vector_set(vOutput, ii, sum1);

  }

  for (ii = numRows; ii < vOutput->size; ii++) {
    gsl_vector_set(vOutput, ii, 0);
  }

  return 0;

}


/******************************************************************************
 * Function: getProjectMatrixDHI
 * Inputs: int, double
 * Returns: gsl_matrix*
 * Description: For an onion model, with each shell/layer being given by the column
 * number, j, and each impact parameter given by the row number, i, find the
 * matrix that represents the amount each shell contributes to the impact
 * parameter, i. M(i,j), where M = 0 for j<i because if the impact parameter
 * is greater then the shell, it can't have any contribution.
 ******************************************************************************/

static gsl_matrix* getProjectMatrixDHI(int sizeM, double res) {

  int ii, jj;
  double num;

  gsl_matrix *projectMatrix = gsl_matrix_alloc(sizeM,sizeM);

  /* 
   * Here is the method I came up with to get the length of the
   * chord of impact parameter ii, through the shell, jj. The chord
   * is the chord through the center of the 1 pixel width rectangle
   * that passes through the plasma
   */
  for (jj = 0; jj < sizeM; jj++) {
    for (ii = 0; ii <= jj; ii++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(ii+0.5))
	  -sqrt(gsl_pow_2(jj)-
		gsl_pow_2(ii+0.5));

	num = 2*num*res;

	gsl_matrix_set(projectMatrix, ii, jj, num);
	
    }
  }

  /* 
   * Double back over cases where i = j, because the formula doesn't work
   * for that case.
   */
  for (jj = 0; jj < sizeM; jj++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(jj+0.5));
	
	num = 2*num*res;

	gsl_matrix_set(projectMatrix, jj, jj, num);
	
  }

  return projectMatrix;

}
 

/******************************************************************************
 * Function: axialVariationCorrectionDHI
 * Inputs: gsl_matrix*, gsl_matrix*, gsl_matrix*, gsl_vector*
 * Returns: int
 * Description: This will go through and correct for any axial phase variations. It 
 * will basically, for the longer of the left/right profiles, take edge value of 
 * the line integrated density, then convert it to density (divide by maximum 
 * length at that axial point before the circle 2*(sqrt(R^2 - y^2), and add 
 * that to both the left and right radial density profiles.
 ******************************************************************************/

static int axialVariationCorrectionDHI(gsl_matrix *leftDensityProfile, 
				       gsl_matrix *rightDensityProfile, gsl_matrix *imageM, 
				       gsl_vector *centroidLocation, holographyParameters* param) {

  int numRows = param->numRows, // Number of Rows, pixels along the y-direction
    numCols = param->numCols,   // Number of Cols, pixels along the x-direction 
    centroid,                   // Holds the value of the centroid
    leftLen,                    // Length of left radial density profile
    rightLen;                   // Length of right radial density profile

  gsl_vector_view leftCrossSection,   // Vector views of matrix to represent col
    rightCrossSection;                // Vector views of matrix to represent col

  double densityOffset,
    dy = param->deltaY,
    yval,
    Relec = param->R_electrode;

  /* y value for the top/bottom edge, assuming laser is centered in assembly region */
  yval = dy * numRows/2;

  /*
   * Iterating through each column and offsetting it up the phase on the top or bottom edge
   */
  int jj;
  for (jj = 0; jj < numCols; jj++) {

    /* Centroid location for jjth column */
    centroid = gsl_vector_get(centroidLocation, jj);

    /* Skip if the centroid was not determined */
    if ( (centroid == 0) || (centroid == (numRows-1)) ) {

      continue;

    }

    /* Column view of the radial densities matrices */
    leftCrossSection  = gsl_matrix_column(leftDensityProfile, jj);
    rightCrossSection = gsl_matrix_column(rightDensityProfile, jj);

    leftLen = centroid;
    rightLen = numRows - centroid;

    /* Checking which side is bigger, left or right, and taking top or bottom density value */
    if ( leftLen > rightLen) {

      /* y value at the outer most cylindrical shell. Assumes plasma centered in machine */
      yval = dy * leftLen;

      /* Dividing the line integrated data by the chord length of the of edge */
      densityOffset = gsl_matrix_get(imageM, 0, jj)/(2*sqrt(gsl_pow_2(Relec) 
							    - gsl_pow_2(yval)));

      /* Raising left radial profile up by the minimum background value */
      gsl_vector_add_constant(&leftCrossSection.vector, densityOffset);

      /* Raising right radial profile up by the minimum background value */
      gsl_vector_add_constant(&rightCrossSection.vector, densityOffset);

    } else {

      /* y value at the outer most cylindrical shell. Assumes plasma centered in machine */
      yval = dy * rightLen;
      
      /* Dividing the line integrated data by the chord length of the of edge */
      densityOffset = gsl_matrix_get(imageM, numRows-1, jj)/(2*sqrt(gsl_pow_2(Relec) 
								    - gsl_pow_2(yval)));

      /* Raising left radial profile up by the minimum background value */
      gsl_vector_add_constant(&leftCrossSection.vector, densityOffset);

      /* Raising right radial profile up by the minimum background value */
      gsl_vector_add_constant(&rightCrossSection.vector, densityOffset);

    }
    
  }

  return 0;

}


/******************************************************************************
 * Function: getErrorBarsDHI
 * Inputs: gsl_matrix*, gsl_matrix*, gsl_matrix*, gsl_matrix *
 * Returns: gsl_matrix *
 * Description: This function will forward project an average of the left and
 * right density profile, then take the difference between that projection
 * and the original image. Then, it will abel invert that difference.
 ******************************************************************************/

static gsl_matrix *getErrorBarsDHI(gsl_matrix *projectMatrix, gsl_matrix *leftProfile,
				   gsl_matrix *rightProfile, gsl_matrix *image) {

  int numRows = image->size1,
    numCols = image->size2,
    ii, jj;

  double avg;

  gsl_matrix *mRet = gsl_matrix_alloc(numRows, numCols);

  gsl_matrix *average = gsl_matrix_alloc(numRows, numCols);


  /* Calculating the average of the left and right profile */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      avg = 0.5*(gsl_matrix_get(leftProfile, ii, jj) + gsl_matrix_get(rightProfile, ii, jj));
      gsl_matrix_set(average, ii, jj, avg);
    }
  }

  
  /* Forward projecting the average */
  


  /* Setting the return value */
  gsl_matrix_memcpy(mRet, average);


  gsl_matrix_free(average);

  return mRet;
  

}



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/


static gsl_vector *testMatrixMult(gsl_matrix *mInput, gsl_vector *vInput);
static int overlayCenterLineTest(gsl_matrix *mInput, char *fileCentroid);
static int testInvertImageDHI();

/* 1 is using meters, 100 if using CM */
#define CM_ADJUST 100

static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT = {
  .res = 3.85E-6*CM_ADJUST, 
  .lambda = 532E-9*CM_ADJUST,
  .d = 0.35*CM_ADJUST,
  .deltaX = 0.000115*CM_ADJUST,
  .deltaY = 0.000115*CM_ADJUST,
  .zPosition = 0.140*CM_ADJUST,
  .R_electrode = 0.100838*CM_ADJUST,
  .deltaN = 1E21/(CM_ADJUST*CM_ADJUST*CM_ADJUST),
  .c = 2.998E8*CM_ADJUST,
  .e0 = 8.854e-12/(CM_ADJUST*CM_ADJUST*CM_ADJUST),
  .q = 1.602e-19,
  .me = 9.109E-31,
  .Z = 2,
  .numRows = 4000,
  .numCols = 6016,
  .hyperbolicWin = 8,
  .sampleInterval = 10,
  .centroidNum = 10,
  .offsetIter = 10,
  .boxCarSmoothWidth = 10,
  .unwrapThresh = 1.0*M_PI,
  .signTwin = 1,
  .rotateImage = 1,
  .flipImageRows = 0,
  .flipImageCols = 0,
  .refSubtract = 1,
  .axialCorrection = 0,
  .convertDensity = 1,
  //.fileRef = "/home/fuze/DHI_Images/Calibration/May_16_2018/DSC_0006.JPG",
  //.filePlasma = "/home/fuze/DHI_Images/Calibration/May_16_2018/DSC_0007.JPG",
  .fileRef = "/home/fuze/DHI_Images/180516/180516014_Baseline.JPG",
  .filePlasma = "/home/fuze/DHI_Images/180516/180516014.JPG",
  .xLL = 2764,
  .yLL = 2452,
  .xUR = 3845,
  .yUR = 4911,
  .saveHologram = 1,
  .fileHologram = "data/hologram.dat",
  .saveWrappedPhase = 1,
  .fileWrappedPhase = "data/wrappedPhase.dat",
  .saveLineInt = 1,
  .fileLineIntPos = "data/lineIntegratedPosition.dat",
  .fileLineInt = "data/lineIntegrated.dat",
  .fileLineIntText = "data/lineIntegrated.txt",
  .invertImage = 0,
  .fileLeftInvert = "data/leftAbelInvert.txt",
  .fileRightInvert = "data/rightAbelInvert.txt",
  .fileFullInvert = "data/fullAbelInvert.dat",
  .fileFullInvertPos = "data/fullAbelInvertPosition.dat",
  .fileFullInvertText = "data/fullAbelInvert.txt",
  .fileCentroid = "data/centroid.txt"
};

int testAbelInversionDHI() {

  testInvertImageDHI();

  return 0;

}
  

static int testInvertImageDHI() {
  
  int numRows = 100,
    numCols = 100;

  gsl_matrix *testData = gsl_matrix_alloc(numRows, numCols);

  double val;

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.numRows = numRows;
  param.numCols = numCols;
  param.deltaY = 0.000115;

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
  
  saveMatrixData(radialProfile, "data/radialProfile.txt");

  int center;
  double offset;

  for (jj = 0; jj < numCols; jj++) {

    gsl_matrix_get_col(radialProfileVec, radialProfile, jj);
    testVec = testMatrixMult(projectMatrix, radialProfileVec);
    
    center = (int) (50.0 + ((float) jj / numCols)*80 - 40);
    offset = jj*0.005/numCols;
    for (ii = 0; ii < numRows; ii++) {

      gsl_matrix_set(testData, ii, jj, offset+gsl_vector_get(testVec, abs(center - ii)));
      
    }
  }

  param.deltaN = 0.02;
  param.centroidNum = 10;
  param.offsetIter = 10;

  plotImageData(testData, "set title 'Forward Projected Data'\n");
    
  gsl_matrix *invertedImage = invertImageDHI(testData, &param);
  overlayCenterLineTest(invertedImage, param.fileCentroid);
  saveImageData(invertedImage, param.fileFullInvert);
  
  int colPlot = 85;
  plot2MatrixColDataFile(param.fileLeftInvert, colPlot,
			 "data/radialProfile.txt", colPlot, "");
  plot2MatrixColDataFile(param.fileRightInvert, colPlot,
			 "data/radialProfile.txt", colPlot, "");
  
  plotImageDataFile(param.fileFullInvert, "set cbrange [0:1.2]\n");

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

/******************************************************************************
 * Function: overlayCenterLineTest
 * Inputs: gsl_matrix, char *
 * Returns: int
 * Description: Overlays a center line on the inverted data
 ******************************************************************************/

static int overlayCenterLineTest(gsl_matrix *mInput, char *fileCentroid) {

  int ii, jj, center,
    numRows = mInput->size1,
    numCols = mInput->size2;

  /* Temporary vector to read centroid coordinates */
  gsl_vector* tempVector = gsl_vector_alloc(numRows);

  double val;
  
  /* Reading in centroid coordinates */
  FILE *fp;
  fp = fopen(fileCentroid, "r");
  char buffer[100];
  ii = 0;
  while (fscanf(fp, "%s", buffer) == 1) {
    sscanf(buffer, "%lf", &val);
    gsl_vector_set(tempVector, ii, val);
    ii++;
  }
  fclose(fp);

  /* Setting selected centroid values to 0 */
  for (jj = 0; jj < numCols; jj++) {

    center = (int) gsl_vector_get(tempVector, jj);
    
    for (ii = 0; ii < numRows; ii++) {
      
      if (ii == center) {
	gsl_matrix_set(mInput, ii, jj, 0.0);
      }

    }
  }

  gsl_vector_free(tempVector);

  return 0;
  
}

