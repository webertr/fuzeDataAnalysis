#include "imageAnalysisDHI.h"

/******************************************************************************
 * Example Usage:
 *
 * holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
 *
 * param.res = 3.85E-6;             // CCD Resolution
 * param.lambda = 532E-9;           // Wavelength of laser
 * param.d = 0.37;                  // Reconstruction distance
 * param.deltaN = 1E23;             // Density offset delta for inversion
 * param.hyperbolicWin = 8;         // Hyperbolic window parameter
 * param.sampleInterval = 10;       // Sampling interval of line-integrated density 
 * param.centroidNum = 10;          // number of centroids to vary +/- around maximum (10)
 * param.offsetIter = 10;           // Number of offset iterations (15)
 * param.boxCarSmoothWidth = 10;    // Width of box car smoothing on phase
 * param.unwrapThresh = 1.0*M_PI;   // Threshold to trigger the phase unwrapping
 * param.signTwin = 1;              // Sign to density conversion +/-1. Depends on laser setup (-1)
 * param.debugPhase = 0;            // 1 means save and plot a col profile of phase 
 * param.debugPhaseColNum = 10;     // Col number to save for the phase and unwrapped phase
 * param.debugPhaseRowNum = 61;     // Row number to save for the phase and unwrapped phase
 * param.hologramPreview = 0;       // 1 means to preview the hologram before extracting twin image
 * param.invertImage = 0;           // 1 means to invert the image.
 * param.plotRadialProfile = 1;     // 1 means to plot the inverted radial profile and slice throu
 * param.plotColNum = 20;          // Column number to plot for the inverted radial profile and a 
 * param.plotLineIntegrated = 1;    // 1 means to plot the line integrated data
 * param.plotRawHologram = 0;       // 1 means it will plot the raw hologram
 * param.plotRawHologramRow = 100;  // 1 means it will plot a row of the raw hologram
 * param.plotRawHologramCol = 100;  // 1 means it will plot a column of the raw hologram
 * param.plotTwinImage = 0;         // 1 means it will plot a column of the twin image  
 * param.rotateImage = 1;           // 1 means to rotate the image by 90 degrees CW
 * param.flipImageRows = 0;         // 1 means to flip the rows 0 <-> end index
 * param.flipImageCols = 0;         // 1 means to flip the cols 0 <-> end index
 * param.refSubtract = 1;           // 1 means to subtract the reference image
 * param.zPosition = .145;          // Z position of the hologram at the center of the image
 * char *filenameRef = "/home/fuze/DHI_Images/Calibration/DSC_0087.JPG";
 * char *filenamePlasma = "/home/fuze/DHI_Images/Calibration/DSC_0088.JPG";
 *
 * param.xLL = 306;          // Lower left x pixel value of phase reconstruction
 * param.yLL = 1686;          // Lower left y pixel value of phase reconstruction
 * param.xUR = 1300;          // Upper right x pixel value of phase reconstruction
 * param.yUR = 3506;          // Upper right y pixel value of phase reconstruction
 *
 * hologramAnalysis(filenameRef, filenamePlasma, 
 *		   &param,
 *		   "data/leftProfile.dat", "data/rightProfile.dat",
 *		   "data/centroidLocation.dat");
 *
 ******************************************************************************/

static int quadSwapComplexImage (gsl_matrix_complex *mInput);
static int hyperbolicWindow (gsl_matrix *mInput, int param);

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

  /* 
   * If debugPhase is on, this will write a radial profile plot of the phase
   * then later, of the unwrapped phase
   */
  if (param->debugPhase == 1) {

    FILE *fp1;
    fp1 = fopen("data/phaseCol.txt", "w");
    int ii;
    for (ii = 0; ii < numRows; ii++) {

      fprintf(fp1, "%f\n",
	      gsl_matrix_get(mRet, ii, param->debugPhaseColNum));

    }
    fclose(fp1);

    FILE *fp2;
    fp2 = fopen("data/phaseRow.txt", "w");
    int jj;
    for (jj = 0; jj < numCols; jj++) {

      fprintf(fp2, "%f\n",
	      gsl_matrix_get(mRet, param->debugPhaseRowNum, jj));

    }
    fclose(fp2);

    lineIntegratedSave(mRet, "data/lineIntegratedWrappedPhase.dat");

  }

  /* Unwrapping the columns */
  unwrapCols(mRet, param);

  /* Unwrapping the rows */
  unwrapRows(mRet, param);

  /* 
   * If debugPhase is on, this will write a radial profile plot of the unwrapped phase
   * then exit and plot the phase and unwrapped phase.
   */
  if (param->debugPhase == 1) {

    FILE *fp1;
    fp1 = fopen("data/phaseUnwrapCol.txt", "w");
    int ii;
    for (ii = 0; ii < numRows; ii++) {

      fprintf(fp1, "%f\n",
	      gsl_matrix_get(mRet, ii, param->debugPhaseColNum));

    }
    fclose(fp1);
    
    FILE *fp2;
    fp2 = fopen("data/phaseUnwrapRow.txt", "w");
    int jj;
    for (jj = 0; jj < numCols; jj++) {

      fprintf(fp2, "%f\n",
	      gsl_matrix_get(mRet, param->debugPhaseRowNum, jj));

    }
    fclose(fp2);

    lineIntegratedSave(mRet, "data/lineIntegratedUnWrappedPhase.dat");

    system("script/plot_phase.sh");

    exit(1);
    
  }

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

gsl_vector *getImageYVectorHol(gsl_matrix *mInput, holographyParameters* param) {

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

gsl_vector *getImageXVectorHol(gsl_matrix *mInput, holographyParameters* param) {

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
 * Function: saveHologramImageBinary
 * Inputs: gsl_matrix *, gsl_vector *, gsl_vector *, char *
 * Returns: int
 * Description: Save binary matrix data to be read by gnuplot such as:
 * MS = zeros(length(x)+1,length(y)+1);
 * MS(1,1) = length(x);
 * MS(1,2:end) = y;
 * MS(2:end,1) = x;
 * MS(2:end,2:end) = M';
 * % Write data into the file
 * fid = fopen(file,'w');
 * fwrite(fid,MS,'float');
 * fclose(fid);
 * plot 'color_map.bin' binary matrix with image
 * Example:
 * plot 'data/lineIntegrated.dat' binary matrix with image title "Line Integrated"
 ******************************************************************************/

int saveHologramImageBinary(gsl_matrix *mInput, gsl_vector* xVec, gsl_vector* yVec,
			    char *fileName) {

  int numRows = yVec->size;
  int numCols = xVec->size;

  /* Creates matrix to get written */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) gsl_vector_get(yVec, ii-1));
  }
  /* Setting x vector values */
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) gsl_vector_get(xVec, ii-1));
  }
  /* Setting matrix values */
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }
  
  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen(fileName, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: saveHologramImageBinary
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: Save binary matrix data to be read by gnuplot such as:
 * MS = zeros(length(x)+1,length(y)+1);
 * MS(1,1) = length(x);
 * MS(1,2:end) = y;
 * MS(2:end,1) = x;
 * MS(2:end,2:end) = M';
 * % Write data into the file
 * fid = fopen(file,'w');
 * fwrite(fid,MS,'float');
 * fclose(fid);
 * plot 'color_map.bin' binary matrix with image
 * Example:
 * plot 'data/test.dat' binary matrix with image title "Line Integrated"
 ******************************************************************************/

int saveHologramImageBinaryOne(gsl_matrix *mInput, char *fileName) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Allocating the matrix to save */  
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values to indices*/
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }
  /* Setting x vector values to indices*/
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }
  /* Setting matrix values */
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }

  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen(fileName, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

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
  double e_0 = 8.85e-12,       // Permitivity of free space
    q = 1.6e-19,               // electron charge [C]
    c = 3e8,                   // speed of light [m/s]
    me = 9.12e-31;             // electron mass [kg]

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
 * Function: saveRadialProfileWithPosition
 * Inputs: gsl_matrix *, gsl_vector *, int, char *
 * Returns: int
 * Description: This saves a radial profile along with position information.
 * You can read this using gnuplot with the following command:
 * plot 'data/leftRadialProfile.txt' using ($1*1000):2 title "Left Profile",\
 *      'data/rightRadialProfile.txt' using ($1*1000):2 title "Right Profile"
 ******************************************************************************/

int saveRadialProfileWithPosition(char *fileLeftMatrix, char *fileRightMatrix,
				  int numRows, int numCols,
				  gsl_vector* yVec, int colNum, char *fileLeftProfile,
				  char *fileRightProfile) {

  int numRowsVector = yVec->size;

  double offsetVector = gsl_vector_get(yVec, 0);

  gsl_matrix* tempMatrixLeft = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix* tempMatrixRight = gsl_matrix_alloc(numRows, numCols);

  FILE *fp1;
  fp1 = fopen(fileLeftMatrix, "rb");
  gsl_matrix_fread(fp1, tempMatrixLeft);
  fclose(fp1);
  FILE *fp2;
  fp2 = fopen(fileRightMatrix, "rb");
  gsl_matrix_fread(fp2, tempMatrixRight);
  fclose(fp2);

  FILE *fp3;
  fp3 = fopen(fileLeftProfile, "w");
  FILE *fp4;
  fp4 = fopen(fileRightProfile, "w");

  int ii;
  for (ii = 0; ii < numRowsVector; ii++) {

    fprintf(fp3, "%f %f\n",
	    gsl_vector_get(yVec, ii)-offsetVector,
	    gsl_matrix_get(tempMatrixLeft, ii, colNum));
    fprintf(fp4, "%f %f\n",
	    gsl_vector_get(yVec, ii)-offsetVector,
	    gsl_matrix_get(tempMatrixRight, ii, colNum));

  }
  fclose(fp3);
  fclose(fp4);

  gsl_matrix_free(tempMatrixLeft);
  gsl_matrix_free(tempMatrixRight);

  return 0;

}


/******************************************************************************
 * Function: saveRadialProfileWithPosition
 * Inputs: gsl_matrix *, gsl_vector *, int, char *
 * Returns: int
 * Description: This saves a radial profile along with position information.
 * You can read this using gnuplot with the following command:
 * plot 'data/leftRadialProfile.txt' using ($1*1000):2 title "Left Profile",\
 *      'data/rightRadialProfile.txt' using ($1*1000):2 title "Right Profile"
 ******************************************************************************/

int saveLineIntegratedSlice(gsl_matrix *mInput, int colNumber, 
			    char *fileSaveName) {


  int numRows = mInput->size1;

  FILE *fp1;
  fp1 = fopen(fileSaveName, "w");

  int ii;
  for (ii = 0; ii < numRows; ii++) {

    fprintf(fp1, "%f\n",
	    gsl_matrix_get(mInput, ii, colNumber));

  }
  fclose(fp1);

  return 0;

}


/******************************************************************************
 * Function: saveRadialProfileWithPosition
 * Inputs: gsl_matrix *, gsl_vector *, int, char *
 * Returns: int
 * Description: This saves a radial profile along with position information.
 * You can read this using gnuplot with the following command:
 * plot 'data/leftRadialProfile.txt' using ($1*1000):2 title "Left Profile",\
 *      'data/rightRadialProfile.txt' using ($1*1000):2 title "Right Profile"
 ******************************************************************************/

int saveLineIntegratedRow(gsl_matrix *mInput, int rowNumber, 
			  char *fileSaveName) {

  int numCols = mInput->size2;

  FILE *fp1;
  fp1 = fopen(fileSaveName, "w");

  int jj;
  for (jj = 0; jj < numCols; jj++) {

    fprintf(fp1, "%f\n",
	    gsl_matrix_get(mInput, rowNumber, jj));

  }
  fclose(fp1);

  return 0;

}


/******************************************************************************
 * Function: readRawFileHol
 * Inputs: char *
 * Returns: gsl_matrix*
 * Description: Read a raw file, and return a gsl matrix.
 ******************************************************************************/

gsl_matrix *readRawFileHol (char * filename, int numRows, int numCols) {

  char tempBuf;
  int tempInt;
  
  FILE *fp1;
  fp1 = fopen(filename, "rb");

  gsl_matrix* tempMatrix = gsl_matrix_alloc(numRows,numCols);
  int ii, jj;
  for (ii = 0; ii < numRows; ii++ ) {
    for (jj = 0; jj < numCols; jj++) {
    
      fread(&tempBuf, sizeof(char), 1, fp1);
      tempInt = (int) tempBuf;
      gsl_matrix_set(tempMatrix, ii, jj, tempInt);

    }
  }
  
  fclose(fp1);

  return tempMatrix;

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
 * Function: getMatlabImage
 * Inputs: char*, int, int
 * Returns: gsl_matrix*
 * Description: Reads in a image file saved from matlab (octave). Save script in
 * octave or matlab:
 * fileName = '../../data/Base_Shot_26.JPG';
 * hol_base_rgb = imread(fileName);
 * rgb_red = 0.3;
 * rgb_green = 0.59;
 * rgb_blue = 0.11;
 * hol_base = double(hol_base_rgb(:,:,1).*rgb_red + hol_base_rgb(:,:,2).*rgb_green \
 * + hol_base_rgb(:,:,3).*rgb_blue)';
 * newFileName = strrep(fileName,'JPG','dat');
 * fidR = fopen(newFileName,'w');
 * fwrite(fidR,hol_base,'double');
 * fclose(fidR);
 * 
 * Or, once you have the hol_base matrix:
 * fidR = fopen(newFileName,'w');
 * fwrite(fidR,hol_base,'double');
 * fclose(fidR);
 ******************************************************************************/

gsl_matrix* getMatlabImage (char *fileName, int numRows, int numCols) {

  gsl_matrix* tempMatrix = gsl_matrix_alloc(numCols, numRows);
  gsl_matrix* retMatrix = gsl_matrix_alloc(numRows, numCols);

  FILE *fp;
  fp = fopen(fileName, "rb");
  gsl_matrix_fread(fp, tempMatrix);
  fclose(fp);
  
  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(retMatrix, ii, jj,
		     gsl_matrix_get(tempMatrix, jj, ii));

    }
  }

  /* Freeing temporary matrix */
  gsl_matrix_free(tempMatrix);

  return retMatrix;

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
 * Function: lineIntegratedCenterLine
 * Inputs: 
 * Returns: int
 * Description: This puts a varying center line across a line integrated
 * image and saves it. Read this with:
 * plot 'data/test.dat' binary matrix with image title ""
 ******************************************************************************/

int lineIntegratedCenterLine(gsl_matrix *mInput, char *fileNameImage, char* fileNameCentroid) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Temporary matrix to construct matrix to write */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Format so that gnuplot knows how many columns */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;

  /* Temporary vector to read centroid coordinates */
  gsl_vector* tempVector = gsl_vector_alloc(numCols);

  /* Reading in centroid coordinates */
  FILE *fp3;
  fp3 = fopen(fileNameCentroid, "rb");
  gsl_vector_fread(fp3, tempVector);
  fclose(fp3);
  
  /* Writting matrix to be read by gnuplot */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }

  int val1;

  /* Setting selected centroid values to 0 */
  for (jj = 1; jj < numCols+1; jj++) {

    val1 = 1 + (int) gsl_vector_get(tempVector, jj-1);
    
    for (ii = 1; ii < numRows+1; ii++) {
      
      if (ii == val1) {
	gsl_matrix_float_set(temp, ii, jj, 0.0);
      }

    }
  }

  FILE *fp2;
  fp2 = fopen(fileNameImage, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: lineIntegratedSave
 * Inputs: 
 * Returns: int
 * Description: This saves it with no center line. Read this with:
 * plot 'data/test.dat' binary matrix with image title ""
 ******************************************************************************/

int lineIntegratedSave(gsl_matrix *mInput, char *fileNameImage) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Temporary matrix to construct matrix to write */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Format so that gnuplot knows how many columns */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;

  /* Writting matrix to be read by gnuplot */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }

  FILE *fp2;
  fp2 = fopen(fileNameImage, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: saveGSLMatrix
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: This saves a gsl matrix and the dimensions in a txt file
 * gsl_matrix* imagePlasma = readJPEGImage(plasmaFileName);
 * saveGSLMatrix(imagePlasma, "data/imagePlasma");
 * gsl_matrix *imagePlasma = readGSLMatrix("data/imagePlasma");
 ******************************************************************************/

int saveGSLMatrix(gsl_matrix *mInput, char *fileName) {

  int numRows = mInput->size1,
    numCols = mInput->size2;

  char fileNameDim[100];
  char fileNameMatrix[100];

  strcpy(fileNameDim, fileName);
  strcat(fileNameDim, ".txt");

  strcpy(fileNameMatrix, fileName);
  strcat(fileNameMatrix, ".dat");

  /* 
   * Writting dimesion information into another file
   */
  FILE *fp1;
  fp1 = fopen(fileNameDim, "w");
  fprintf(fp1, "%d\n%d",
	  numRows,
	  numCols);
  fclose(fp1);

  /*
   * Writting binary matix information
   */
  FILE *fp2;
  fp2 = fopen(fileNameMatrix, "wb");
  gsl_matrix_fwrite (fp2, mInput);
  fclose(fp2);

  return 0;

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
 * Function: readGSLMatrix
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: This saves a gsl matrix and the dimensions in a txt file
 * Usage:
 * gsl_matrix* imagePlasma = readJPEGImage(plasmaFileName);
 * saveGSLMatrix(imagePlasma, "data/imagePlasma");
 * gsl_matrix *imagePlasma = readGSLMatrix("data/imagePlasma");
 ******************************************************************************/

gsl_matrix *readGSLMatrix(char *fileName) {

  int numRows, numCols;

  char fileNameDim[100];
  char fileNameMatrix[100];

  strcpy(fileNameDim, fileName);
  strcat(fileNameDim, ".txt");

  strcpy(fileNameMatrix, fileName);
  strcat(fileNameMatrix, ".dat");

  /* 
   * Writting dimesion information into another file
   */
  FILE *fp1;
  fp1 = fopen(fileNameDim, "r");
  fscanf(fp1, "%d", &numRows);
  fscanf(fp1, "%d", &numCols);
  fclose(fp1);

  gsl_matrix* mRet = gsl_matrix_alloc(numRows, numCols);

  /*
   * Reading binary matix information
   */
  FILE *fp2;
  fp2 = fopen(fileNameMatrix, "rb");
  gsl_matrix_fread (fp2, mRet);
  fclose(fp2);

  return mRet;

}


/******************************************************************************
 * Function: save2DInvertedProfile
 * Inputs: gsl_matrix
 * Returns: int
 * Description: Takes the left inverted density profile, right inverted density
 * profile, and the centroid, and saves them into a 2D image to be plotted
 * by gnuplot using:
 * plot 'data/test.dat' binary matrix with image title ""
 ******************************************************************************/

int save2DInvertedProfile (char *imageSave, char *fileLeftProfile, char* fileRightProfile, 
			   char* fileCentroid, holographyParameters* param) {

  int numRows = param->numRows,
    numCols = param->numCols;

  gsl_matrix* mLeftTemp = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix* mRightTemp = gsl_matrix_alloc(numRows, numCols);
  gsl_vector* tempVector = gsl_vector_alloc(numCols);

  /* Temporary matrix to construct matrix to write */
  gsl_matrix_float* tempMatrix = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Format so that gnuplot knows how many columns */
  gsl_matrix_float_set(tempMatrix,0,0,(float) numCols);

  /*
   * Reading binary matix information for left and right profiles
   */
  FILE *fp1;
  fp1 = fopen(fileLeftProfile, "rb");
  gsl_matrix_fread (fp1, mLeftTemp);
  fclose(fp1);

  FILE *fp2;
  fp2 = fopen(fileRightProfile, "rb");
  gsl_matrix_fread (fp2, mRightTemp);
  fclose(fp2);
  
  /*
   * Reading vector information for the centroid
   */
  FILE *fp3;
  fp3 = fopen(fileLeftProfile, "rb");
  gsl_vector_fread (fp3, tempVector);
  fclose(fp3);

  int ii, jj;
  /* Writting matrix to be read by gnuplot */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(tempMatrix, ii, 0,
			 (float) ii);
  }
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(tempMatrix, 0, ii,
			 (float) ii);
  }
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(tempMatrix, ii, jj,
			   (float) gsl_matrix_get(mLeftTemp,ii-1, jj-1));

    }
  }

  gsl_matrix_float_free(tempMatrix);
  gsl_matrix_free(mLeftTemp);
  gsl_matrix_free(mRightTemp);
  gsl_vector_free(tempVector);

  return 0;

}


/******************************************************************************
 * Function: hologramAnalysis
 * Inputs: char *, char, char*, double, double, double, int, int
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int hologramAnalysis(char* baseFileName, char *plasmaFileName,
		     holographyParameters* param,
		     char* fileLeftProfile, char* fileRightProfile,
		     char* fileCentroidLocation) {

  /* Reading in the jpeg file and getting cols/rows */
  gsl_matrix* imagePlasma = readJPEGImage(plasmaFileName);
  gsl_matrix* imageRef = readJPEGImage(baseFileName);

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
 
  /*
   * If plotRawHologram is set, plot the raw hologram
   */
  if (param->plotRawHologram == 1) {

    saveHologramImageBinaryOne(imagePlasma, "data/rawHologram.dat");
    saveLineIntegratedSlice(imagePlasma, param->plotRawHologramCol, "data/rawHologramCol.txt");
    saveLineIntegratedRow(imagePlasma, param->plotRawHologramRow, "data/rawHologramRow.txt");
    system("script/plot_raw_hologram.sh");
    exit(1);

  }

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
   * If the holgramPreview debug parameter is set,
   * then save the file, plot it and exit
   */
  if (param->hologramPreview == 1) {

    saveHologramImageBinaryOne(phase, "data/hologramPreview.dat");
    system("script/plot_hologram_preview.sh");
    exit(1);

  }
    
  /* Extracting the 1st order image from the reconstruction */
  gsl_matrix *twinImage = extractTwinImage(phase, param);

  /*
   * If the twinPlot is set, plot the twin image
   */
  if (param->plotTwinImage == 1) {

    saveHologramImageBinaryOne(twinImage, "data/hologramPreview.dat");
    system("script/plot_hologram_preview.sh");
    exit(1);
    
  }

  /* Taking every param->sampleInterval the element */
  gsl_matrix *twinImageReduce = matrixReduceElements(twinImage, param);

  /* Taking sine/cos, smoothing, then unwrapping phase */
  gsl_matrix* twinImageUnwrap = smoothUnwrapPhase(twinImageReduce, param);

  /* Converting phase difference to electron density */
  convertPhaseDensity(twinImageUnwrap, param);

  /* Getting position vectors for line integrated image */
  gsl_vector *yPhase = getImageYVectorHol(twinImageUnwrap, param); 
  gsl_vector *xPhase = getImageXVectorHol(twinImageUnwrap, param); 

  /* If the invertImage is specified, do the inversion */
  if (param->invertImage == 1) {

    /* Inverting the image and saving the radial profiles and centroid locations */
    invertImage(twinImageUnwrap, fileLeftProfile, fileRightProfile,
		fileCentroidLocation, param);

    /*
     * Saving a slice through the line integrated density.
     */
    saveLineIntegratedSlice(twinImageUnwrap, param->plotColNum, "data/lineIntegratedSlice.txt");
    
    /*
     * Saving the 2D inverted profile
     */
    save2DInvertedProfile ("data/inverted2DImage.dat", fileLeftProfile, 
			   fileRightProfile, fileCentroidLocation, param);

    /*
     * If specified, plotting the radial profils for a specific column
     */
    if (param->plotRadialProfile == 1) {

      /* Extracting and saving radial profile for specific column */
      saveRadialProfileWithPosition(fileLeftProfile, fileRightProfile, twinImageUnwrap->size1, 
				    twinImageUnwrap->size2,
				    yPhase, param->plotColNum, "data/leftRadialProfile.txt",
				    "data/rightRadialProfile.txt");

      system("script/plot_radial_profile.sh");
      exit(1);

    }

  }

  /*
   * if specified, plotting the line integrated data with/without centroid or position information
   */
  if (param->plotLineIntegrated == 1) {

    /* 
     * Option to save the image as a GSL matrix
     */
    //saveGSLMatrix(twinImageUnwrap, "data/plasmaMatrix.dat");
    
    /*
     * Saving line integrated with pixel number and center line encoded on it
     * plot 'data/test.dat' binary matrix with image title "Line Integrated"
     */
    //lineIntegratedCenterLine(twinImageUnwrap, "data/lineIntegrated.dat", fileCentroidLocation);

    /*
     * Saving line integrated with pixel number and NO center line encoded on it
     * plot 'data/test.dat' binary matrix with image title "Line Integrated"
     */
    //lineIntegratedSave(twinImageUnwrap, "data/lineIntegrated.dat");

    /*
     * Save line integrated with position information
     */
    saveHologramImageBinary(twinImageUnwrap, xPhase, yPhase, "data/lineIntegrated.dat");

    /* 
     * Save line integrated with no position or centroid information
     */
    //saveHologramImageBinaryOne(twinImageUnwrap, "data/lineIntegrated.dat");
    
    system("script/plot_line_integrated.sh");
    exit(1);

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


/******************************************************************************
 * Function: hologramAnalysis
 * Inputs: char *, char, char*, double, double, double, int, int
 * Returns: int
 * Description: This does the full hologram analysis of a hologram and saves the file
 * so you can look it at it in gnuplot.
 ******************************************************************************/

int hologramMain() {

  /* 
   * struct containing all the holography parameters.
   * Setting to default values.
   */
  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  param.res = 3.85E-6;             // CCD Resolution
  param.lambda = 532E-9;           // Wavelength of laser
  param.d = 0.37;                  // Reconstruction distance
  param.deltaN = 1E23;             // Density offset delta for inversion
  param.hyperbolicWin = 8;         // Hyperbolic window parameter
  param.sampleInterval = 10;       // Sampling interval of line-integrated density 
  param.centroidNum = 10;          // number of centroids to vary +/- around maximum (10)
  param.offsetIter = 10;           // Number of offset iterations (15)
  param.boxCarSmoothWidth = 10;    // Width of box car smoothing on phase
  param.unwrapThresh = 1.0*M_PI;   // Threshold to trigger the phase unwrapping
  param.signTwin = 1;              // Sign to density conversion +/-1. Depends on laser setup (-1)
  param.debugPhase = 0;            // 1 means save and plot a col profile of phase 
                                   // and unwrapped phase
  param.debugPhaseColNum = 10;     // Col number to save for the phase and unwrapped phase
  param.debugPhaseRowNum = 61;     // Row number to save for the phase and unwrapped phase
  param.hologramPreview = 0;       // 1 means to preview the hologram before extracting twin image
  param.invertImage = 0;           // 1 means to invert the image.
  param.plotRadialProfile = 1;     // 1 means to plot the inverted radial profile and slice through
                                   // the line integrated image (at plotColNum)
  param.plotColNum = 20;          // Column number to plot for the inverted radial profile and a 
                                   // slice of line integrated data
  param.plotLineIntegrated = 1;    // 1 means to plot the line integrated data
  param.plotRawHologram = 0;       // 1 means it will plot the raw hologram
  param.plotRawHologramRow = 100;  // 1 means it will plot a row of the raw hologram
  param.plotRawHologramCol = 100;  // 1 means it will plot a column of the raw hologram
  param.plotTwinImage = 0;         // 1 means it will plot a column of the twin image  
  param.rotateImage = 1;           // 1 means to rotate the image by 90 degrees CW
  param.flipImageRows = 0;         // 1 means to flip the rows 0 <-> end index
  param.flipImageCols = 0;         // 1 means to flip the cols 0 <-> end index
  param.refSubtract = 1;           // 1 means to subtract the reference image
  param.zPosition = .145;          // Z position of the hologram at the center of the image

  /******** Holography Analysis *************/

  char *filenameRef = "/home/webertr/DHI_Images/180417/180417004.JPG";
  char *filenamePlasma = "/home/webertr/DHI_Images/180417/180417004_Baseline.JPG";

  /* Setting bounds of reconstructed image */
  param.xLL = 2758;          // Lower left x pixel value of phase reconstruction
  param.yLL = 2733;          // Lower left y pixel value of phase reconstruction
  param.xUR = 3771;          // Upper right x pixel value of phase reconstruction
  param.yUR = 4797;          // Upper right y pixel value of phase reconstruction


  /* Obtained line integrated data and do an abel inversion */
  hologramAnalysis(filenameRef, filenamePlasma, 
		   &param,
		   "data/leftProfile.dat", "data/rightProfile.dat",
		   "data/centroidLocation.dat");

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
