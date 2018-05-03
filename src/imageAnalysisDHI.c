#include "imageAnalysisDHI.h"

/******************************************************************************
 * Example Usage:
 *
 * int test() {
 *
 * holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
 *
 * hologramMain(&param, "data/leftProfile.dat", "data/rightProfile.dat",
 *		"data/centroidLocation.dat");
 *
 * return 0;
 *
 *}
 *
 ******************************************************************************/

static int quadSwapComplexImage (gsl_matrix_complex *mInput);
static int hyperbolicWindow (gsl_matrix *mInput, int param);
static int overlayCenterLine(gsl_matrix *mInput, char *fileCentroid);

/*
 * Declare constants and ZaP-HD plasma parameters:
 * e_0 = 8.85e-12; % Permitivity of free space
 * mu_0 = 1.257e-6; % Permeability of free space
 * k = 1.38e-23; % Boltzmann's constant in SI units
 * q = 1.6e-19; % electron charge [C]
 * c = 3e8; % speed of light [m/s]
 * mi = 1.67e-27; % proton mass [kg]
 * me = 9.12e-31; % electron mass [kg]
 * lambda = 532e-9; % laser wavelength [m]
 * R_electrode = 8*2.54/100;
 * res = (3.85e-6); % [m] Pixel size of Nikon 3200D 4.3e-6 for Canon Rebel T2i
 */


/******************************************************************************
 * Function: fresnel
 * Inputs: gsl_matrix
 * Returns: gsl_matrix_complex*
 * Description: Executes a fresnel transformation on the input image.
 ******************************************************************************/

gsl_matrix_complex *fresnel (gsl_matrix *mInput, holographyParameters* param) {

  /************* Reconstruction Parameters *******************************
 * res = (3.85e − 6); // [m] Pixel size of Nikon 3200D, 4.3e − 6 for Canon T2i
 * lambda = 532e − 9; // [m] Nd:YAG wavelength, ruby is 694.3e − 9 [m]
 * d =.50;            // [m] Reconstruction distance
 * n_c = 2.312e27;    // [m^ − 3] Critical plasma density
 **********************************************************************/

  double d = param->d,           // reconstruction distance
    lambda = param->lambda,      // laser wavelength
    res = param->res,            // CCD resolution
    del_xisq = gsl_pow_2(res),   // pixel size in x − direction
    del_etasq = gsl_pow_2(res);  // pixel size in y − direction

  int ii, jj,
    numRows = param->numRows, // Number of Rows, pixels along the y-direction
    numCols = param->numCols; // Number of Cols, pixels along the x-direction 
  
  /* Setting deltaX and and deltaY values */
  param->deltaX = d*lambda/(numCols*res);
  param->deltaY = d*lambda/(numRows*res);

  gsl_matrix_complex* hol_ref = gsl_matrix_complex_alloc(numRows, numCols);
  gsl_complex holC, chirpC, expC;

  /* Applying hyperbolic filter */
  hyperbolicWindow(mInput, param->hyperbolicWin);

  /*
   * Multiply the hologram intensity distribution with the reference beam:
   */
  double tis, tjs;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      /* Getting the complex value of the holgraph */
      holC = gsl_complex_rect(gsl_matrix_get(mInput, ii, jj), 0);

      /* Getting the complex value of the "chirp" function */
      tis = gsl_pow_2(ii - numRows/2.0);
      tjs = gsl_pow_2(jj - numCols/2.0);
      expC = gsl_complex_rect(0, (M_PI/(d*lambda))*((tis*del_etasq)+
						    (tjs*del_xisq)));

      chirpC = gsl_complex_exp(expC);

      /* Multiple the hol_ref by the chirp function and set*/
      gsl_matrix_complex_set(hol_ref, ii, jj,
			     gsl_complex_mul(chirpC,holC));

    }
  }

  /*
   * data is just a pointer to the double/float data, where
   * every other data point is a complex value. The stride
   * is how the data will be accessed, as in data[i*stride].
   * So you could get it equal to the row width to do a column?
   * and the size_t n is the size of the row. How far to go
   * over in i
   * typedef struct
   * {
   *  double dat[2];
   * } gsl_complex;
   * typedef double* gsl_complex_packed_array;
   */
  gsl_complex_packed_array data = &(hol_ref->data[0]);

  gsl_fft_complex_wavetable * wavetableRows;
  gsl_fft_complex_workspace * workspaceRows;
  gsl_fft_complex_wavetable * wavetableCols;
  gsl_fft_complex_workspace * workspaceCols;

  wavetableRows = gsl_fft_complex_wavetable_alloc(numRows);
  workspaceRows = gsl_fft_complex_workspace_alloc(numRows);
  wavetableCols = gsl_fft_complex_wavetable_alloc(numCols);
  workspaceCols = gsl_fft_complex_workspace_alloc(numCols);

  /*
   * Using a row-column algorithm
   */
  /* 
   * 1D FFT of each row, stride = 1 
   * A stride parameter allows the user to perform transforms 
   * on the elements z[stride*i] instead of z[i]
   * int gsl_fft_complex_forward (gsl complex packed array data , size t
   * stride , size t n , const gsl fft complex wavetable * wavetable ,
   * gsl fft complex workspace * work )
   */
  for (ii = 0; ii < numRows; ii++) {

    data = &(hol_ref->data[ii*numCols*2]);
    gsl_fft_complex_forward(data, 1, (size_t) numCols,
			    wavetableCols, workspaceCols);

  }

  /* 1D FFT of each column, stride = numCols */
  for (ii = 0; ii < numCols; ii++) {

    data = &(hol_ref->data[ii*2]);
    gsl_fft_complex_forward(data, numCols, (size_t) numRows,
			    wavetableRows, workspaceRows);
    
  }

  gsl_fft_complex_wavetable_free(wavetableRows);
  gsl_fft_complex_workspace_free(workspaceRows);
  gsl_fft_complex_wavetable_free(wavetableCols);
  gsl_fft_complex_workspace_free(workspaceCols);

  /* 
   * Mike Ross does this in his code. It's equivalent  to a
   * ifftshift in matlab. It seems to make the image centered
   * It it something like a zero frequency shift in the fft like
   * matlab's fftshift() 
   */
  quadSwapComplexImage(hol_ref);
  
  return hol_ref;
  
}

  
/******************************************************************************
 * Function: phaseDiffHolo
 * Inputs: gsl_matrix_complex, gsl_matrix_complex
 * Returns: gsl_matrix*
 * Description: Calculates the phase diference between two complex matrices
 ******************************************************************************/

gsl_matrix *phaseDiffHolo (gsl_matrix_complex *mInput1, gsl_matrix_complex *mInput2) {

  int ii, jj,
    numRows = mInput1->size1, // Number of Rows
    numCols = mInput1->size2; // Number of Cols

  gsl_matrix* phase_diff = gsl_matrix_alloc(numRows, numCols);
  gsl_complex image1C, image2C, imageMul;
  double anglediff;

  /* 
   * Iterates through each element of the matrices and
   * calculates the phase difference between the elements
   */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      image1C = gsl_matrix_complex_get(mInput1, ii, jj);
      image2C = gsl_matrix_complex_get(mInput2, ii, jj);
      
      imageMul = gsl_complex_mul(image1C, gsl_complex_conjugate(image2C));
      anglediff = gsl_complex_arg (imageMul );

      gsl_matrix_set(phase_diff, ii, jj, anglediff);

    }
  }

  return phase_diff;
  
}


/******************************************************************************
 * Function: quadSwapComplexImage
 * Inputs: gsl_matrix_complex
 * Returns: int
 * Description: This swaps the upper left and lower right quadrants, and then
 * the upper right and lower left quadrant.
 ******************************************************************************/

static int quadSwapComplexImage (gsl_matrix_complex *mInput) {

  int ii, jj,
    numRows = mInput->size1, // Number of Rows
    numCols = mInput->size2; // Number of Cols

  /* Making a temporary matrix */
  gsl_matrix_complex* mTemp = gsl_matrix_complex_alloc(numRows, numCols);
  gsl_matrix_complex_memcpy(mTemp, mInput);
  gsl_complex tempC;

  /* 
   * Put first quadrant in 3rd quadrant
   * Top left to lower right
   */
  for (ii = 0; ii < numRows/2; ii++) {
    for (jj = 0; jj < numCols/2; jj++) {

      tempC = gsl_matrix_complex_get(mTemp, ii, jj);
      gsl_matrix_complex_set(mInput, ii+numRows/2, jj+numCols/2, tempC);

    }
  }

  /* 
   * Put third quadrant in 1st quadrant
   * Lower right to top left
   */
  for (ii = 0; ii < numRows/2; ii++) {
    for (jj = 0; jj < numCols/2; jj++) {

      tempC = gsl_matrix_complex_get(mTemp, ii+numRows/2, jj+numCols/2);
      gsl_matrix_complex_set(mInput, ii, jj, tempC);

    }
  }

  /* 
   * Put top right quadrant in lower left quadrant
   */
  for (ii = 0; ii < numRows/2; ii++) {
    for (jj = 0; jj < numCols/2; jj++) {

      tempC = gsl_matrix_complex_get(mTemp, ii, jj+numCols/2);
      gsl_matrix_complex_set(mInput, ii+numRows/2, jj, tempC);

    }
  }

  /* 
   * Put lower left quadrant in top right quadrant
   */
  for (ii = 0; ii < numRows/2; ii++) {
    for (jj = 0; jj < numCols/2; jj++) {

      tempC = gsl_matrix_complex_get(mTemp, ii+numRows/2, jj);
      gsl_matrix_complex_set(mInput, ii, jj+numCols/2, tempC);

    }
  }

  return 0;
}


/******************************************************************************
 * Function: hyperbolicWindows
 * Inputs: gsl_matrix*
 * Returns: int
 * Description: Applies a smoothing function to the holograms to remove
 * Gibbs phenomena in the resulting reconstructions
 ******************************************************************************/

static int hyperbolicWindow (gsl_matrix *mInput, int param) {

  double xi,       // x value for tanh function along row
    yi,            // y value for tanh function along column
    val1;
  
  int ii, jj,
    numRows = mInput->size1, // Number of Rows
    numCols = mInput->size2; // Number of Cols

  /* Applying tanh across rows, in x values. */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      xi = -param*M_PI+2*M_PI*param/numCols*jj;
      val1 = (tanh(xi+(param-1)*M_PI)-tanh(xi-(param-1)*M_PI)) *
	gsl_matrix_get(mInput,ii,jj);
      
      /* Setting the matrix to itself multipled by the filter*/
      gsl_matrix_set(mInput, ii, jj, val1);

    }
  }

  /* Applying tanh down columns, in y values. */
  for (jj = 0; jj < numCols; jj++) {
    for (ii = 0; ii < numRows; ii++) {

      yi = -param*M_PI+2*M_PI*param/numRows*ii;
      val1 = (tanh(yi+(param-1)*M_PI)-tanh(yi-(param-1)*M_PI)) *
	gsl_matrix_get(mInput,ii,jj);
      
      /* Setting the matrix to itself multipled by the filter*/
      gsl_matrix_set(mInput, ii, jj, val1);

    }
  }

  return 0;

}


/******************************************************************************
 * Function: boxCarSmooth
 * Inputs: gsl_matrix*, int width
 * Returns: gsl_matrix*
 * Description: Applies a box smoothing that averages all points in a box that
 * size of the input width paramter. Width should be multiple of two. It
 * iterates over a shannon square. Drops edges that are too close to border
 * to be averaged.
 ******************************************************************************/

gsl_matrix* boxCarSmooth (gsl_matrix *mInput, holographyParameters* param) {

  int ii, jj, kk, ll,
    width = param->boxCarSmoothWidth,    // Box car smoothing width
    numRows = mInput->size1,             // Number of Rows
    numCols = mInput->size2,             // Number of Cols
    sWidth = (2*width+1) * (2*width+1);  // square of width

  double sum;

  /* Returned matrix will be smaller because we won't include the edges */
  gsl_matrix *mRet = gsl_matrix_alloc(numRows-2*width, numCols-2*width);
  
  /* Go over each pixel and calculating the box car average */
  for (ii = width; ii < (numRows-width); ii++) {
    for (jj = width; jj < (numCols-width); jj++) {

      sum = 0;
      for (kk = 0; kk < (2*width+1); kk++) {
	for (ll = 0; ll < (2*width+1); ll++) {
	
	  sum = sum + gsl_matrix_get(mInput,ii-width+kk,jj-width+ll);

	}
      }

      gsl_matrix_set(mRet, ii-width, jj-width, sum/sWidth);

    }
  }

  /* Updating size of new matrices */
  param->numRows = mRet->size1;
  param->numCols = mRet->size2;

  return mRet;

}


/******************************************************************************
 * Function: extractTwinImage
 * Inputs: gsl_matrix*
 * Returns: int
 * Description: Pass it a matrix, with the lower left x and y indices, and
 * the upper right x and y indices, and this function will return the twin image.
 ******************************************************************************/

gsl_matrix *extractTwinImage (gsl_matrix *mInput, holographyParameters* param) {
  
  int ii, jj,
    xLL = param->xLL,
    yLL = param->yLL,
    xUR = param->xUR,
    yUR = param->yUR,
    twinRows = (yUR-yLL),    // Number of rows to be returned
    twinCols = (xUR-xLL);    // Number of Cols to be returned

  /* The twin image that will be returned */
  gsl_matrix* twinImage = gsl_matrix_alloc(twinRows, twinCols);

  /* Setting param struct to be used later in program */
  param->numRows = twinRows;
  param->numCols = twinCols;

  double val1;

  /* Extracting sub-matrix. */
  for (ii = 0; ii < twinRows; ii++) {
    for (jj = 0; jj < twinCols; jj++) {

      val1 = gsl_matrix_get(mInput,yLL+ii,xLL+jj);
      
      gsl_matrix_set(twinImage, ii, jj, val1);

    }
  }

  return twinImage;

}


/******************************************************************************
 * Function: smoothUnwrapPhase
 * Inputs: gsl_matrix*
 * Returns: gsl_matrix*
 * Description: Pass it a matrix, and it will take the sin and cos, then do
 * then go back to the phase angle, then do an unwrapping
 * procedure where it looks for jumps greater then pi, and subtracts 2pi
 ******************************************************************************/

gsl_matrix *smoothUnwrapPhase (gsl_matrix *mInput, holographyParameters* param) {
			      
  int ii, jj,
    numRows = param->numRows,
    numCols = param->numCols;

  double aTemp, mMax;
  
  /* Using sine and cosine to maintain fringe jumps */
  gsl_matrix* mTempSin = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix* mTempCos = gsl_matrix_alloc(numRows, numCols);
    
  /* take sine and cosine of phase angle */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(mTempSin, ii, jj,
  		     sin(gsl_matrix_get(mInput,ii,jj)));
      gsl_matrix_set(mTempCos, ii, jj,
  		     cos(gsl_matrix_get(mInput,ii,jj)));

    }
  }

  /* Box car average of two sin/cos */
  gsl_matrix* mBoxSin = boxCarSmooth(mTempSin,param);
  gsl_matrix* mBoxCos = boxCarSmooth(mTempCos,param);

  /* Updating size of new matrices */
  param->numRows = mBoxSin->size1;
  param->numCols = mBoxSin->size2;
  numRows = param->numRows;
  numCols = param->numCols;

  /* Allocating new matrix to return */
  gsl_matrix* mRet = gsl_matrix_alloc(numRows, numCols);

  /* Going back to phase angle via atan. This will maintain a fringe jump */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      aTemp = atan2(gsl_matrix_get(mBoxSin, ii, jj),gsl_matrix_get(mBoxCos,ii,jj));      
      gsl_matrix_set(mRet, ii, jj, aTemp);

    }
  }


  /* Unwrapping the columns */
  unwrapCols(mRet, param);

  /* Unwrapping the rows */
  unwrapRows(mRet, param);


  /* Freeing matrices no longer needed */
  gsl_matrix_free(mTempSin);
  gsl_matrix_free(mTempCos);
  gsl_matrix_free(mBoxSin);
  gsl_matrix_free(mBoxCos);

  /* 
   * Subtracting maximum value. No idea why. M. Ross does it
   */
  mMax = gsl_matrix_max(mRet);
  gsl_matrix_add_constant(mRet, -mMax);

  return mRet;

}


/******************************************************************************
 * Function: unwrapRows
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: Unwraps the rows of the passed phase matrix
 ******************************************************************************/

int unwrapRows(gsl_matrix *mInput, holographyParameters* param) {

  int ii, jj, kk,
    numRows = param->numRows,
    numCols = param->numCols;

  double p1, p2, dp,
    thres = param->unwrapThresh;              // Threshold to trigger phase unwrapping

  /* Unwrapping the rows, eliminate changes > pi */
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 1; jj < numCols; jj++) {

      p1 = gsl_matrix_get(mInput, ii, jj-1);
      p2 = gsl_matrix_get(mInput, ii, jj);

      dp = p2 - p1;

      /* Jump greater then pi means likely it rolled over from -pi */
      if ( dp > thres) {

  	for (kk = jj; kk < numCols; kk++) {
  	  gsl_matrix_set(mInput, ii, kk,
  	  		 gsl_matrix_get(mInput, ii, kk)-2*M_PI);
  	}

      }

      /* Jump less then -pi means likely it rolled over from +pi */
      if ( dp < -thres) {

  	for (kk = jj; kk < numCols; kk++) {
  	  gsl_matrix_set(mInput, ii, kk,
  	  		 gsl_matrix_get(mInput, ii, kk)+2*M_PI);
  	}

      }
      
    }
  }

  return 0;

}


/******************************************************************************
 * Function: unwrapCols
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: Unwraps the rows of the passed phase matrix
 ******************************************************************************/

int unwrapCols(gsl_matrix *mInput, holographyParameters* param) {

  int ii, jj, kk,
    numRows = param->numRows,
    numCols = param->numCols;

  double p1, p2, dp,
    thres = param->unwrapThresh;              // Threshold to trigger phase unwrapping

  /* Unwrapping the cols, eliminate changes > pi */
  for (jj = 0; jj < numCols; jj++) {
    for (ii = 1; ii < numRows; ii++) {

      p1 = gsl_matrix_get(mInput, ii-1, jj);
      p2 = gsl_matrix_get(mInput, ii, jj);

      dp = p2 - p1;
      /* Jump greater then pi means likely it rolled over from -pi */
      if ( dp > thres) {

  	for (kk = ii; kk < numRows; kk++) {
  	  gsl_matrix_set(mInput, kk, jj,
  	  		 gsl_matrix_get(mInput, kk, jj)-2*M_PI);
  	}

      }

      /* Jump less then -pi means likely it rolled over from +pi */
      if ( dp < -thres) {

  	for (kk = ii; kk < numRows; kk++) {
  	  gsl_matrix_set(mInput, kk, jj,
  	  		 gsl_matrix_get(mInput, kk, jj)+2*M_PI);
  	}

      }
      
    }
  }

  return 0;

}


/******************************************************************************
 * Function: getImageYVectorHol
 * Inputs: gsl_matrix *, double, double, double, int, int 
 * Returns: gsl_vector*
 * Description: Returns the y positions for the reconstructed holograph
 ******************************************************************************/

gsl_vector *getImageYVectorHol(holographyParameters* param) {

  int ii,
    numRows = param->numRows;

  /* The twin image that will be returned */
  gsl_vector *rowVector = gsl_vector_alloc(numRows);

  double dY = param->deltaY;

  /* 
   * Creating vector of y positions 
   * Formula is:
   * y = n*deltaY
   * delta Y = d * lambda / (M x deltaEta)
   * deltaEta = res
   */
  for (ii = 0; ii < numRows; ii++) {

    gsl_vector_set(rowVector, ii, (ii-numRows/2)*dY);
    
  }

  return rowVector;

}


/******************************************************************************
 * Function: getImageXVectorHol
 * Inputs: gsl_matrix *, double, double, double, int, int
 * Returns: gsl_vector*
 * Description: Returns the x positions for the reconstructed holograph
 ******************************************************************************/

gsl_vector *getImageXVectorHol(holographyParameters* param) {

  int ii,
    numCols = param->numCols;

  /* The twin image that will be returned */
  gsl_vector *colVector = gsl_vector_alloc(numCols);

  double dX = param->deltaX,
    z0 = param->zPosition;

  /* 
   * Creating vector of y positions 
   * Formula is:
   * y = n*deltaY
   * delta Y = d * lambda / (M x deltaEta)
   * deltaEta = res
   */
  for (ii = 0; ii < numCols; ii++) {

    gsl_vector_set(colVector, ii, z0 - (ii-numCols/2)*dX);
    
  }

  return colVector;

}


/******************************************************************************
 * Function: convertPhaseDensity
 * Inputs: gsl_matrix *
 * Returns: int, int
 * Description: Convert phase change to line integrated density
 ******************************************************************************/

int convertPhaseDensity(gsl_matrix *mInput, holographyParameters* param) {

  /*
   * Unused
   * R_electrode = 8*2.54/100,
   * mi = 1.67e-27,             // proton mass [kg]
   * mu_0 = 1.257e-6,           // Permeability of free space
   * k = 1.38e-23,              // Boltzmann's constant in SI units
   * 
   */
  double e_0 = param->e0,       // Permitivity of free space
    q = param->q,               // electron charge [C]
    c = param->c,               // speed of light [m/s]
    me = param->me;             // electron mass [kg]

  int ii, jj,
    numRows = param->numRows,
    numCols = param->numCols,
    signTwin = param->signTwin;

  double den,
    lambda = param->lambda;
  
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      den = gsl_matrix_get(mInput, ii, jj);
      den = signTwin*den/((-gsl_pow_2(q)/(4*M_PI*gsl_pow_2(c)*me*e_0))*lambda);
      gsl_matrix_set(mInput, ii, jj, den);

    }
  }

  return 0;

}


/******************************************************************************
 * Function: binMatrix
 * Inputs: gsl_matrix*, int width
 * Returns: gsl_matrix*
 * Description: Bins the matrix over the width box. Good if width
 * is a divisor of the row and col lengths.
 ******************************************************************************/

gsl_matrix* binMatrix (gsl_matrix *mInput, int width) {

  int ii, jj, kk, ll,
    numRows = mInput->size1,             // Number of Rows
    numCols = mInput->size2,             // Number of Cols
    sWidth = width*width,  // square of width
    newNumRows = numRows/width,
    newNumCols = numCols/width;
  
  double sum;

  gsl_matrix* retMatrix = gsl_matrix_alloc(newNumRows,newNumCols);
  
  /* Go over each pixel and calculating the box car average */
  for (ii = 0; ii < newNumRows; ii++) {
    for (jj = 0; jj < newNumCols; jj++) {

      sum = 0;
      for (kk = 0; kk < width; kk++) {
	for (ll = 0; ll < width; ll++) {
	
	  sum = sum + gsl_matrix_get(mInput,
				     ii*width+kk,
				     jj*width+ll);

	}
      }

      gsl_matrix_set(retMatrix, ii, jj, sum/sWidth);

    }
  }

  return retMatrix;

}


/******************************************************************************
 * Function: binVector
 * Inputs: gsl_vector*, int width
 * Returns: gsl_vector*
 * Description: Bins the vector over the width box. Good if width
 * is a divisor of the vector length.
 ******************************************************************************/

gsl_vector* binVector (gsl_vector *vInput, int width) {

  int ii, jj,
    vecSize = vInput->size,             // Number of Rows
    newVecSize = vecSize/width;
  
  double sum;

  gsl_vector* retVector = gsl_vector_alloc(newVecSize);
  
  /* Go over each pixel and calculating the box car average */
  for (ii = 0; ii < newVecSize; ii++) {

    sum = 0;
    for (jj = 0; jj < width; jj++) {
      
      sum = sum + gsl_vector_get(vInput, ii*width+jj);
				 
    }

    gsl_vector_set(retVector, ii, sum/width);
    
  }

  return retVector;

}


/******************************************************************************
 * Function: matrixReduceElements(gsl_matrix*, holographyParameters*)
 * Inputs: gsl_matrix*, holographyParameters*
 * Returns: gsl_matrix*
 * Description: This just reduces the matrix by taking every width element
 ******************************************************************************/

gsl_matrix *matrixReduceElements(gsl_matrix *mInput, holographyParameters* param) {

  int ii, jj,
    iin = 0,
    jjn = 0,
    numRows = param->numRows,
    numCols = param->numCols,
    width = param->sampleInterval,
    newNumRows = numRows/width+1,
    newNumCols = numCols/width+1;

  /* Setting param struct to be used later in program */
  param->numRows = newNumRows;
  param->numCols = newNumCols;
  param->deltaX = param->deltaX * width;
  param->deltaY = param->deltaY * width;

  gsl_matrix *mRet = gsl_matrix_alloc(newNumRows, newNumCols);
  
  /* Taking the width element amd setting it to the new matrix value */
  for (ii = 0; ii < numRows; ii=ii+width) {
    jjn = 0;
    for (jj = 0; jj <numCols; jj=jj+width) {

      gsl_matrix_set(mRet, iin, jjn, 
		     gsl_matrix_get(mInput, ii, jj));
      jjn = jjn + 1;

    }
    iin = iin + 1;
  }

  return mRet;

}


/******************************************************************************
 * Function: rotateImage90CW
 * Inputs: gsl_matrix *
 * Returns: gsl_matrix *
 * Description: These allocates some more memory, exchanged the rows and columns
 * of the passed matrix, then frees the memory, and returns the new allocated
 * memory.
 ******************************************************************************/

gsl_matrix *rotateImage90CW(gsl_matrix *imagePlasma) {

  int numRows = imagePlasma->size2,
    numCols = imagePlasma->size1;

  gsl_matrix *imagePlasmaReturn = gsl_matrix_alloc(numRows, numCols);

  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(imagePlasmaReturn, ii, jj,
		     gsl_matrix_get(imagePlasma, jj, ii));

    }
  }

  gsl_matrix_free(imagePlasma);

  return imagePlasmaReturn;

}


/******************************************************************************
 * Function: flipImageRows
 * Inputs: gsl_matrix *
 * Returns: gsl_matrix *
 * Description: These allocates some more memory, flips the columns meaning
 * 0 index becomes the end, and the last index becomes zero.
 ******************************************************************************/

gsl_matrix *flipImageRows(gsl_matrix *imagePlasma) {

  int numRows = imagePlasma->size1,
    numCols = imagePlasma->size2;

  gsl_matrix *imagePlasmaReturn = gsl_matrix_alloc(numRows, numCols);

  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(imagePlasmaReturn, ii, numCols - jj - 1,
		     gsl_matrix_get(imagePlasma, ii, jj));

    }
  }

  gsl_matrix_free(imagePlasma);

  return imagePlasmaReturn;

}


/******************************************************************************
 * Function: flipImageCols
 * Inputs: gsl_matrix *
 * Returns: gsl_matrix *
 * Description: These allocates some more memory, flips the rows meaning
 * 0 index becomes the end, and the last index becomes zero.
 ******************************************************************************/

gsl_matrix *flipImageCols(gsl_matrix *imagePlasma) {

  int numRows = imagePlasma->size1,
    numCols = imagePlasma->size2;

  gsl_matrix *imagePlasmaReturn = gsl_matrix_alloc(numRows, numCols);

  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(imagePlasmaReturn, numRows  - ii - 1 , jj,
		     gsl_matrix_get(imagePlasma, ii, jj));

    }
  }

  gsl_matrix_free(imagePlasma);

  return imagePlasmaReturn;

}


/******************************************************************************
 * Function: overlayCenterLine
 * Inputs: gsl_matrix, char *
 * Returns: int
 * Description: Overlays a center line on the inverted data
 ******************************************************************************/

static int overlayCenterLine (gsl_matrix *mInput, char *fileCentroid) {

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


/******************************************************************************
 * Function: hologramMain
 * Inputs: holographParameters*, char*, char*, char*
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int hologramMain(holographyParameters* param) {

  /* Reading in the jpeg file and getting cols/rows */
  gsl_matrix* imagePlasma = readJPEGImage(param->filePlasma);
  gsl_matrix* imageRef = readJPEGImage(param->fileRef);

  /* Rotating image by 90 degrees CW if specified */
  if (param->rotateImage == 1) {

    imagePlasma = rotateImage90CW(imagePlasma);
    imageRef = rotateImage90CW(imageRef);

  }

  /* Flipping rows left - right if specified */
  if (param->flipImageRows == 1) {

    imagePlasma = flipImageRows(imagePlasma);
    imageRef = flipImageRows(imageRef);

  }

  /* Flipping cols left - right if specified */
  if (param->flipImageCols == 1) {
    
    imagePlasma = flipImageCols(imagePlasma);
    imageRef = flipImageCols(imageRef);

  }

  param->numRows = imagePlasma->size1;
  param->numCols = imagePlasma->size2;
 

  /* Taking fresnel transformation */
  gsl_matrix_complex *fftPlasma = fresnel(imagePlasma, param);
  gsl_matrix_complex *fftRef = fresnel(imageRef, param);

  /*
   * Don't subtract reference image if refSubtract is zero
   */
  if (param->refSubtract == 0) {

    fftRef = gsl_matrix_complex_alloc(param->numRows, param->numCols);
    gsl_matrix_complex_set_all(fftRef, GSL_COMPLEX_ONE);

  }

  /* Finding phase difference between plasma and reference reconstructions */
  gsl_matrix *phase = phaseDiffHolo(fftPlasma, fftRef);
  
  /*
   * If the saveHologram parameter is set, then save the file
   */
  if (param->saveHologram == 1) {

    saveImageData(phase, param->fileHologram);

  }
    
  /* Extracting the 1st order image from the reconstruction */
  gsl_matrix *twinImage = extractTwinImage(phase, param);

  /* Taking every param->sampleInterval the element */
  gsl_matrix *twinImageReduce = matrixReduceElements(twinImage, param);

  /* Taking sine/cos, smoothing, then unwrapping phase */
  gsl_matrix* twinImageUnwrap = smoothUnwrapPhase(twinImageReduce, param);

  /* Converting phase difference to electron density */
  convertPhaseDensity(twinImageUnwrap, param);

  /* Getting position vectors for line integrated image */
  gsl_vector *yPhase = getImageYVectorHol(param); 
  gsl_vector *xPhase = getImageXVectorHol(param); 

  /*
   * if specified, plotting the line integrated data with/without centroid or position information
   */
  if (param->saveLineIntPos == 1) {

    saveImageDataWithPosition(twinImageUnwrap, xPhase, yPhase, param->fileLineIntPos);
    saveImageData(twinImageUnwrap, param->fileLineInt);
    
  }

  /*
   * If specified, perform an abel inversion on the image
   */
  if (param->invertImage == 1) {

    gsl_matrix *invertedImage = invertImageDHI(twinImageUnwrap, param);
    overlayCenterLine(invertedImage, param->fileCentroid);
    saveImageDataWithPosition(invertedImage, xPhase, yPhase, param->fileFullInvertPos);
    saveImageData(invertedImage, param->fileFullInvert);
    overlayCenterLine(twinImageUnwrap, param->fileCentroid);
    saveImageDataWithPosition(twinImageUnwrap, xPhase, yPhase, param->fileLineIntPos);
    saveImageData(twinImageUnwrap, param->fileLineInt);
    
  }

  gsl_matrix_free(imagePlasma);
  gsl_matrix_free(imageRef);
  gsl_matrix_complex_free(fftPlasma);
  gsl_matrix_complex_free(fftRef);
  gsl_matrix_free(phase);
  gsl_matrix_free(twinImage);
  gsl_matrix_free(twinImageReduce);
  gsl_matrix_free(twinImageUnwrap);
  gsl_vector_free(xPhase);
  gsl_vector_free(yPhase);

  return 0;

}


/*
 * Holography Parameters:
 *
 * Resolution of Camera CCD Pixels = 3.85E-6,
 * Wavelength of the laser = 532E-9,
 * Reconstruction distance = 0.5;
 * Bounds of reconstructed image: xLL, yLL, xUR, yUR
 * Hyperbolic Window Fit Parameter: 8
 * Sampling interval of line-integrated density: 10
 * Box car average window of phase: 8
 * Sign of the twin image (depends on laser setup): -1
 *
 * Inversion Parameters:
 * Numbers of centroids to vary +/- around the maximum: 10
 * Density offset delta = 5E18
 * Number of offset iterations: 15
 *
 * 1 radian = 6.7E20 number/m^2
 */
