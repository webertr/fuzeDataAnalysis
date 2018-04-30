#include "abelInversionDHI.h"


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

int invertImageDHI(gsl_matrix* imageM, holographyParameters* param) {

  int jj,
    maxIndex,
    centroidIndexTest,
    centroidIterations,                // For loop to scan center vals to find best cen of plasma
    numCentroids = param->centroidNum, // +/- number to vary around max val to find the true center
    numRows = imageM->size1,           // Number of rows 
    numCols = imageM->size2;           // Number of cols

  /* The matrices to hold the final radial profiles, both left and right, for each column */
  gsl_matrix *leftDensityProfile = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix *rightDensityProfile = gsl_matrix_alloc(numRows, numCols);

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
    if ( centroidIndexTest >= (numRows-1) ) {

      /* 
       * leftDensityProfile, rightDensityProfile need to have set to a "NaN" (Not a Number) value,
       */
      gsl_vector_set_all(leftCrossSection, GSL_NAN);
      gsl_vector_set_all(rightCrossSection, GSL_NAN);

      /* This is the centroid location in absolute pixel-space */
      gsl_vector_set(centroidLocation, jj, numRows-1);

      continue;
      
    }

    /* 
     * If the centroid is near (withint 2 x numCentroids) the upper boundary,
     * then adjust the number of centroid iterations appropriately. At this point,
     * the lower boundary isn't deal with. That is how it is in Mike's code.
     */
    if ( centroidIndexTest > ( numRows - 2*numCentroids)) {
	
	centroidIterations = 2*numCentroids - 
	(centroidIndexTest - (numRows - 1 - 2*numCentroids));
	
    }
    else {

	/* The number of centroid guesses to iterate through */
	centroidIterations = 2*numCentroids;
	
    }


    /*
     * This function will find the true center by iterating through different center lines,
     * then it will set the values for leftDensityProfile, and rightDensityProfile, 
     * the left and right radial densities. 
     * Each column will represent a radial densities generated from 1 cross-section.
     * It will also set the centroidLocation vector which represents the plasma center in
     * each cross section
     */
    getRadialDensityProfileDHI(leftCrossSection, rightCrossSection, crossSection, centroidLocation, 
			       projectMatrix, centroidIterations, centroidIndexTest, jj, param);

    gsl_matrix_set_col(leftDensityProfile, jj, leftCrossSection);
    gsl_matrix_set_col(rightDensityProfile, jj, rightCrossSection);
    
  }

  /*
   * This will go through and correct for any axial phase variations. It will basically, for
   * the longer of the left/right profiles, take edge value of the line integrated density,
   * then convert it to density (divide by maximum length at that axial point before the circle
   * 2*(sqrt(R^2 - y^2), and add that to both the left and right radial density profiles.
   */
  axialVariationCorrectionDHI(leftDensityProfile, rightDensityProfile, imageM,
			      centroidLocation, param);

  /*
   * Saving data, leftDensityProfile, rightDensityProfile, and the centroidLocation
   */
  FILE *fp1;
  fp1 = fopen(param->fileLeftInvert, "wb");
  gsl_matrix_fwrite (fp1, leftDensityProfile);
  fclose(fp1);

  FILE *fp2;
  fp2 = fopen(param->fileRightInvert, "wb");
  gsl_matrix_fwrite (fp2, rightDensityProfile);
  fclose(fp2);

  FILE *fp3;
  fp3 = fopen(param->fileCentroid, "wb");
  gsl_vector_fwrite (fp3, centroidLocation);
  fclose(fp3);

  /* Deleting vectors and matrices */
  gsl_vector_free(centroidLocation);
  gsl_vector_free(crossSection);
  gsl_vector_free(leftCrossSection);
  gsl_vector_free(rightCrossSection);
  gsl_matrix_free(leftDensityProfile);
  gsl_matrix_free(rightDensityProfile);
  gsl_matrix_free(projectMatrix);

  return 0;

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

int getRadialDensityProfileDHI(gsl_vector* leftCrossSection, gsl_vector* rightCrossSection, 
			       gsl_vector* crossSection, gsl_vector* centroidLocation,
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
     * Solves the linear system of equations that is,
     * a = M x b
     * where a is the line integrated density vector, and
     * b is the radial density profile, and M is the matrix 
     * that projects the radial density onto the line integrated density profile
     */
    solveRightSystemLinearEqDHI(projectMatrix, crossSection, rightCrossSection, rightSize);

    solveLeftSystemLinearEqDHI(projectMatrix, crossSection, leftCrossSection, leftSize);


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
      findDensityOffsetDHI(rightCrossSection, leftCrossSection, param);
    }
    else {
      sizeTot = leftSize;
      findDensityOffsetDHI(leftCrossSection, rightCrossSection, param);
    }

    /* Zeroing check sum value */
    checkSum = 0;

    /*
     * This takes a running check sum total of the
     * square of the difference between the large
     * vector and the small vector the offset
     */
    for (jj = 0; jj < sizeTot; jj++) {

      val1 = gsl_vector_get(rightCrossSection, jj) - gsl_vector_get(leftCrossSection, jj);
      checkSum = checkSum + gsl_pow_2(val1);
	
    }    

    /* 
     * If the checkSum is at a minimum, then update the minimum centroid value
     */
    if (checkSum < minCheckSum) {

      minCheckSum = checkSum;
      minLeftSize = leftSize;
      minRightSize = rightSize;

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

int findDensityOffsetDHI(gsl_vector* smallCrossSection, gsl_vector* largeCrossSection,
			 holographyParameters* param) {

  double  offsetValue,          // The actual offset value
    val1,                       // Value to set the vector to
    minCheckSum = DBL_MAX,      // Minimum Check sum value. Set to max double
    minOffsetValue,             // Minimum offset value
    checkSum,                   // The value to see how the offset worked
    offsetDelta = param->deltaN; // Delta of offset values

  int offsetIteration = param->offsetIter,  // The number of offsets to try
    length = smallCrossSection->size,
    ii, jj;

  /*
   * Iterating through the different values for the density offset of the smaller profile
   */
  for (ii = 0; ii < offsetIteration; ii++) {

    offsetValue = ii*offsetDelta;
    checkSum = 0;

    /*
     * This takes a running check sum total of the square of the difference between the large
     * vector and the small vector the offset
     */
    for (jj = 0; jj < length; jj++) {

      val1 = gsl_vector_get(largeCrossSection, jj) - (gsl_vector_get(smallCrossSection, jj) +
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

    gsl_vector_set(smallCrossSection, jj, gsl_vector_get(smallCrossSection, jj) + minOffsetValue);

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

int solveRightSystemLinearEqDHI(gsl_matrix* mInput, gsl_vector* vInput, gsl_vector* vOutput,
				int rightSize) {

  int ii,
    jj,
    numRows = rightSize,
    numCols = rightSize,
    vecSize1 = vInput->size,
    vecSize2 = vInput->size;

  double vec,
    sum1;

  gsl_matrix_view rightProjectMatrix; // Sub-matrix view of right inversion matrix

  /* Getting matrix to solve equation with */
  rightProjectMatrix = gsl_matrix_submatrix(mInput, 0, 0, rightSize, rightSize);

    
  /* Error checking */
  if (numRows != numCols) {

    fprintf(stderr, "Matrix needs to be square. From solveRightSystemLinearEq.\n");
    exit(0);

  }

  if (vecSize1 != vecSize2) {

    fprintf(stderr, "Vectors need to be the same size. From solveRightSystemLinearEq.\n");
    exit(0);

  }

  if (numRows != vecSize1) {

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
    sum1 = gsl_vector_get(vInput, ii) - vec;
    sum1 = sum1 / gsl_matrix_get(&rightProjectMatrix.matrix, ii, ii);

    gsl_vector_set(vOutput, ii, sum1);

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

int solveLeftSystemLinearEqDHI(gsl_matrix* mInput, gsl_vector* vInput, gsl_vector* vOutput,
			       int leftSize) {

  int ii,
    jj,
    numRows = leftSize,
    numCols = leftSize,
    vecSize1 = vInput->size,
    vecSize2 = vInput->size;

  double vec,
    sum1;


  gsl_matrix_view leftProjectMatrix; // Sub-matrix view of right inversion matrix

  /* Getting matrix to solve equation with */
  leftProjectMatrix = gsl_matrix_submatrix(mInput, 0, 0, leftSize, leftSize);


  /* Error checking */
  if (numRows != numCols) {

    fprintf(stderr, "Matrix needs to be square. From solveLeftSystemLinearEq.\n");
    exit(0);

  }

  if (vecSize1 != vecSize2) {

    fprintf(stderr, "Vectors need to be the same size. From solveLeftSystemLinearEq.\n");
    exit(0);

  }

  if (numRows != vecSize1) {

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
    sum1 = gsl_vector_get(vInput, (numRows-1)-ii) - vec;
    sum1 = sum1 / gsl_matrix_get(&leftProjectMatrix.matrix, ii, ii);

    gsl_vector_set(vOutput, ii, sum1);

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

gsl_matrix* getProjectMatrixDHI(int sizeM, double res) {

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

int axialVariationCorrectionDHI(gsl_matrix *leftDensityProfile, gsl_matrix *rightDensityProfile,
				gsl_matrix *imageM, gsl_vector *centroidLocation,
				holographyParameters* param) {

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
