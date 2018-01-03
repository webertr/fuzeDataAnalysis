#include "imageAnalysis.h"
#include "testing.h"

int testBoxCarSmooth() {

  gsl_matrix* testMatrix = gsl_matrix_alloc(100,100);

  int ii, jj;

  for (ii = 0; ii < 100; ii++) {

    for (jj = 0; jj < 100; jj++) {

      if ( (0 == ii%15) && (0 == jj%15) ) {

	gsl_matrix_set(testMatrix, ii, jj, 1);

      }
      
    }

  }

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.boxCarSmoothWidth = 5;

  gsl_matrix *testMatrixSmooth = boxCarSmooth(testMatrix, &param);

  for (ii = 0; ii < 100; ii++) {

    printf("%f\t\n", gsl_matrix_get(testMatrixSmooth, ii, 10));

  }

  lineIntegratedSave(testMatrixSmooth, "data/lineIntegrated.dat");

  //system("script/plot_line_integrated.sh");
  exit(1);

  return 0;

}


int unwrapTest() {

  int N = 1000;

  gsl_matrix *phase = gsl_matrix_alloc(N,N);

  gsl_matrix *unwrappedPhase = gsl_matrix_alloc(N,N);

  double phase1,
    phase2;

  int ii, jj;

  for (ii = 0; ii < N; ii++) {

    for (jj = 0; jj < N; jj++) {

      phase1 = M_PI/10*ii + M_PI/5*jj;

      phase2 = atan2(sin(phase1), cos(phase1));

      gsl_matrix_set(phase, ii, jj, phase2); 
      gsl_matrix_set(unwrappedPhase, ii, jj, phase1); 

    }

  }

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.unwrapThresh = 1.0*M_PI;
  param.numRows = N;
  param.numCols = N;

  /* Unwrapping the columns */
  unwrapCols(phase, &param);

  /* Unwrapping the rows */
  unwrapRows(phase, &param);

  lineIntegratedSave(phase, "data/lineIntegrated.dat");

  system("script/plot_line_integrated.sh");
  exit(1);

  return 0;


}


gsl_matrix *getTestData() {

  int numRows = 100,
    numCols = 100;

  gsl_matrix *testData = gsl_matrix_alloc(numRows, numCols);

  double val;

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.numRows = numRows;
  param.numCols = numCols;
  
  /* Get the matrix that will project the radial profile to the line integrated density */
  gsl_matrix *projectMatrix = getProjectMatrix(param.numRows, param.deltaY);

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

  char *fileLeftProfile = "data/leftProfile.dat";
  char *fileRightProfile = "data/rightProfile.dat";
  char *fileCentroid = "data/centroidLocation.dat";
  invertImage(testData, fileLeftProfile, fileRightProfile,
  	      fileCentroid, &param);

  saveRadialProfileTest(fileLeftProfile, fileRightProfile, numRows, numCols,
			0, "data/leftRadialProfile0.txt", "data/rightRadialProfile0.txt");

  saveRadialProfileTest(fileLeftProfile, fileRightProfile, numRows, numCols,
			99, "data/leftRadialProfile99.txt", "data/rightRadialProfile99.txt");

  lineIntegratedCenterLine(testData, "data/lineIntegrated.dat", fileCentroid);

  radialProfileVec = gsl_matrix_column(radialProfile, 0);
  saveVectorTest(&radialProfileVec.vector, "data/radialProfile0.txt");
  radialProfileVec = gsl_matrix_column(radialProfile, 99);
  saveVectorTest(&radialProfileVec.vector, "data/radialProfile99.txt");

  saveLineIntegratedSlice(testData, 0, "data/lineIntegratedSlice0.txt");
  saveLineIntegratedSlice(testData, 99, "data/lineIntegratedSlice99.txt");
    
  system("script/plot_test.sh");

  //saveVectorTest(testVec, "data/lineIntegratedSlice.txt");

  //saveVectorTest(radialProfile, "data/radialProfile.txt");

  //system("script/plot_test.sh");

  //lineIntegratedSave(testData, "data/lineIntegrated.dat");

  //system("script/plot_line_integrated.sh");
  //exit(1);

  return testData;
  
}


gsl_vector *testMatrixMult(gsl_matrix *mInput, gsl_vector *vInput) {

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


void saveVectorTest(gsl_vector *vecIn, char *fileName) {

  int numRows = vecIn->size;

  FILE *fp1;
  fp1 = fopen(fileName, "w");

  int ii;
  for (ii = 0; ii < numRows; ii++) {

    fprintf(fp1, "%f\n",
	    gsl_vector_get(vecIn, ii));

  }
  fclose(fp1);

  return;

}


int saveRadialProfileTest(char *fileLeftMatrix, char *fileRightMatrix,
			  int numRows, int numCols,
			  int colNum, char *fileLeftProfile,
			  char *fileRightProfile) {

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
  for (ii = 0; ii < numRows; ii++) {

    fprintf(fp3, "%f\n",
	    gsl_matrix_get(tempMatrixLeft, ii, colNum));
    fprintf(fp4, "%f\n",
	    gsl_matrix_get(tempMatrixRight, ii, colNum));

  }
  fclose(fp3);
  fclose(fp4);

  gsl_matrix_free(tempMatrixLeft);
  gsl_matrix_free(tempMatrixRight);

  return 0;

}


gsl_matrix *getRadialProfileImageTest(char *fileLeftMatrix, char *fileRightMatrix,
				      char *fileNameCentroid,
				      int numRows, int numCols) {

  gsl_matrix* tempMatrixLeft = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix* tempMatrixRight = gsl_matrix_alloc(numRows, numCols);

  gsl_matrix* retImage = gsl_matrix_alloc(numRows, numCols);

  FILE *fp1;
  fp1 = fopen(fileLeftMatrix, "rb");
  gsl_matrix_fread(fp1, tempMatrixLeft);
  fclose(fp1);

  FILE *fp2;
  fp2 = fopen(fileRightMatrix, "rb");
  gsl_matrix_fread(fp2, tempMatrixRight);
  fclose(fp2);

  gsl_vector* tempVector = gsl_vector_alloc(numCols);
  FILE *fp3;
  fp3 = fopen(fileNameCentroid, "rb");
  gsl_vector_fread(fp3, tempVector);
  fclose(fp3);

  int ii, jj,
    rowCen = numRows/2,
    colCen = numCols/2;

  double r;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      if ( (ii == 50) && (jj == 50) ) {
	continue;
      }

      r = sqrt(gsl_pow_2(ii-rowCen) + gsl_pow_2(jj-colCen));

      gsl_matrix_set(retImage, ii, jj, 
		     gsl_matrix_get(tempMatrixLeft, (int) r, numCols/2));

    }
  }

  gsl_matrix_free(tempMatrixLeft);
  gsl_matrix_free(tempMatrixRight);
  gsl_vector_free(tempVector);

  return retImage;

}

int testWaveletFitRow(gsl_vector *vecIn, int nc) {

  int n = vecIn->size;
  double *abscoeff = malloc (n * sizeof (double));
  size_t *p = malloc (n * sizeof (size_t));

  int ii;

  gsl_wavelet *w;
  gsl_wavelet_workspace *work;

  w = gsl_wavelet_alloc(gsl_wavelet_daubechies_centered, 4);
  work = gsl_wavelet_workspace_alloc(n);

  gsl_wavelet_transform_forward(w, vecIn->data, 1, n, work);

  for (ii = 0; ii < n; ii++) {
    abscoeff[ii] = fabs(vecIn->data[ii]);
  }
  
  gsl_sort_index(p, abscoeff, 1, n);
  
  for (ii = 0; (ii + nc) < n; ii++) {
    vecIn->data[p[ii]] = 0;
  }

  gsl_wavelet_transform_inverse(w, vecIn->data, 1, n, work);

  gsl_wavelet_free(w);
  gsl_wavelet_workspace_free(work);

  free(abscoeff);
  free(p);

  return 0;

}


gsl_matrix *testWaveletGetFit(char *fileName, int nc) {

  int colNumber = 3000,
    rowNumber = 3000;

  gsl_matrix* imageRef = readJPEGImage(fileName);
  //gsl_matrix* imageRef = readJPEGImage("data/2017_10_05_023.JPG");

  saveHologramImageBinaryOne(imageRef, "data/rawHologram.dat");

  saveLineIntegratedSlice(imageRef, colNumber, "data/rawHologramCol.txt");
  saveLineIntegratedRow(imageRef, rowNumber, "data/rawHologramRow.txt");

  int numRowsScale = 4096,
    numColsScale = 8192;

  gsl_matrix* refScale = gsl_matrix_alloc(numRowsScale, numColsScale);

  int ii, jj;

  int numRows = imageRef->size1,
    numCols = imageRef->size2;

  gsl_vector_view rowRef, 
    colRef;

  for(ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(refScale, ii, jj,
		     gsl_matrix_get(imageRef, ii, jj));

    }
    rowRef = gsl_matrix_row(refScale, ii);
    testWaveletFitRow(&rowRef.vector, nc);
  }

  for (jj = 0; jj < numCols; jj++) {
    colRef = gsl_matrix_column(refScale, jj);
    testWaveletFitRow(&colRef.vector, nc);
  }

  for(ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(imageRef, ii, jj,
		     gsl_matrix_get(refScale, ii, jj));

    }
  }  

  //saveLineIntegratedSlice(imageRef, colNumber, "data/fitHologramCol.txt");
  //saveLineIntegratedRow(imageRef, rowNumber, "data/fitHologramRow.txt");

  //testPutVerticalLine(imageRef, colNumber, 10);
  //testPutHorizontalLine(imageRef, rowNumber, 10);
  //saveHologramImageBinaryOne(imageRef, "data/fitHologram.dat");

  //saveGSLMatrix(gsl_matrix imageRef, "data/2017_10_05_021_Smooth.dat");
  // Spacing appears to be 150 pixels
  
  //system("script/plot_test.sh");

  return imageRef;

}


int testWaveletFit() {

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  param.res = 3.85E-6;             // CCD Resolution
  param.lambda = 532E-9;           // Wavelength of laser
  param.d = 0.43;                  // Reconstruction distance
  param.deltaN = 1E23;             // Density offset delta for inversion
  param.hyperbolicWin = 8;         // Hyperbolic window parameter
  param.sampleInterval = 1;        // Sampling interval of line-integrated density 
  param.centroidNum = 10;          // number of centroids to vary +/- around maximum (10)
  param.offsetIter = 10;           // Number of offset iterations (15)
  param.boxCarSmoothWidth = 40;    // Width of box car smoothing on phase
  param.unwrapThresh = 1.0*M_PI;   // Threshold to trigger the phase unwrapping
  param.signTwin = 1;              // Sign to density conversion +/-1. Depends on laser setup (-1)
  param.debugPhase = 1;            // 1 means save and plot a col profile of phase 
                                   // and unwrapped phase
  param.debugPhaseColNum = 12;     // Col number to save for the phase and unwrapped phase
  param.debugPhaseRowNum = 100;    // Row number to save for the phase and unwrapped phase
  param.hologramPreview = 0;       // 1 means to preview the hologram before extracting twin image
  param.invertImage = 0;           // 1 means to invert the image.
  param.plotRadialProfile = 0;     // 1 means to plot the inverted radial profile and slice through
                                   // the line integrated image (at plotColNum)
  param.plotColNum = 50;           // Column number to plot for the inverted radial profile and a 
                                   // slice of line integrated data 
  param.plotLineIntegrated = 0;    // 1 means to plot the line integrated data
  param.plotRawHologram = 0;       // 1 means it will plot the raw hologram
  param.plotRawHologramRow = 100;  // 1 means it will plot a row of the raw hologram
  param.plotRawHologramCol = 100;  // 1 means it will plot a column of the raw hologram
  param.plotTwinImage = 0;         // 1 means it will plot a column of the twin image  

  gsl_matrix* imageRef = testWaveletGetFit("data/2017_10_05_021.JPG", 55);
  gsl_matrix* imagePlasma = testWaveletGetFit("data/2017_10_05_023.JPG", 55);

  param.numRows = imagePlasma->size1;
  param.numCols = imagePlasma->size2;

  /* Taking fresnel transformation */
  gsl_matrix_complex *fftPlasma = fresnel(imagePlasma, &param);
  gsl_matrix_complex *fftRef = fresnel(imageRef, &param);

  /* Finding phase difference between plasma and reference reconstructions */
  gsl_matrix *phase = phaseDiffHolo(fftPlasma, fftRef);

  saveHologramImageBinaryOne(phase, "data/hologramPreview.dat");
  system("script/plot_hologram_preview.sh");
  exit(1);

  return 0;

}


int testPutVerticalLine(gsl_matrix* mInput, int colNumber, int width) {
  
  int numRows = mInput -> size1;

  int ii, jj;
  for (ii = 0; ii < numRows; ii ++) {

    for (jj = -width; jj < width+1; jj++) {

      gsl_matrix_set(mInput, ii, colNumber+jj, 0);

    }

  }

  return 0;

}


int testPutHorizontalLine(gsl_matrix* mInput, int rowNumber, int width) {
  
  int numCols = mInput -> size2;

  int ii, jj;
  for (jj = 0; jj < numCols; jj++) {

    for (ii = -width; ii < width+1; ii++) {

      gsl_matrix_set(mInput, rowNumber+ii, jj, 0);

    }

  }

  return 0;

}


int testAsymetricProjectionGenerate() {

  int numRows = 100,
    numCols = 100;

  gsl_matrix* plasmaForwardProject = gsl_matrix_alloc(numRows, numCols*numRows);
  gsl_matrix* plasmaImage = gsl_matrix_alloc(numRows, numCols);
  gsl_matrix* plasmaImageProject = gsl_matrix_alloc(numRows, numCols);

  gsl_vector* forwardProjectData = gsl_vector_alloc(numRows*numCols);
  gsl_vector* forwardProjectImage = gsl_vector_alloc(numRows);

  /*
   * ii, jj => ii*numCols + jj
   * A line across starts at a given y value, at the maximum x, and just
   * iterates across x values. It simply needs to caculate the length or amount
   * of time it spends in each. But that should be an easy caculation. Just a
   * binary 1 in at all the x values for each y. Hmm. Not that hard, I guess.
   */

  int ind, ii, jj;
  /* Iterating down the rows. Each row should be a y value */
  for (ii = 0; ii < numRows; ii++) {
    /* 
     * Iterating across the single value that represents all x and y values
     * jj => ii*numCols + jj
     */
    for (ind = ii*numCols; ind < ii*numCols+numCols; ind++) {
      
	gsl_matrix_set(plasmaForwardProject, ii, ind, 1);

    }
  }

  /*
   * Now we need x, y data that is a vector that is numRows * numCols in length
   * Then, we multiply this vector by the matrix to get the vector that is
   * numRows in length. This should be a slice of the line integrated data
   */
  testGetDensitySlice(forwardProjectData, numRows, numCols);

  /*
   * Matrix multiplication
   */
  double sum;
  for (ii = 0; ii < numRows; ii++) {
    sum = 0;
    for (jj = 0; jj < numRows*numCols; jj++) {

      sum = sum + gsl_vector_get(forwardProjectData, jj) 
	* gsl_matrix_get(plasmaForwardProject, ii, jj);

    }
    gsl_vector_set(forwardProjectImage, ii, sum);
  }

  FILE *fp1;
  fp1 = fopen("data/forwardProjectImage.txt", "w");

  for (ii = 0; ii < numRows; ii++) {

    fprintf(fp1, "%f\n",
	    gsl_vector_get(forwardProjectImage, ii));

  }
  fclose(fp1);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      gsl_matrix_set(plasmaImage, ii, jj,
		     gsl_vector_get(forwardProjectData, ii*numCols + jj));
      gsl_matrix_set(plasmaImageProject, ii, jj,
		     gsl_vector_get(forwardProjectImage, ii));

    }
  }

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
  param.numRows = numRows;
  param.numCols = numCols;
  param.deltaX = 1;
  param.deltaY = 1;

  param.deltaN = 0.05;
  param.centroidNum = 10;
  param.offsetIter = 20;

  char *fileLeftProfile = "data/leftProfile.dat";
  char *fileRightProfile = "data/rightProfile.dat";
  char *fileCentroid = "data/centroidLocation.dat";
  invertImage(plasmaImageProject, fileLeftProfile, fileRightProfile,
  	      fileCentroid, &param);

  gsl_matrix *plasmaImageRecon = getRadialProfileImageTest(fileLeftProfile, fileRightProfile,
  							   fileCentroid, 
  							   numRows, numCols);

  saveRadialProfileTest(fileLeftProfile, fileRightProfile, numRows, numCols,
			numCols/2, "data/leftRadialProfile.txt", "data/rightRadialProfile.txt");
  
  saveHologramImageBinaryOne(plasmaImage, "data/plasmaImage.dat");
  saveHologramImageBinaryOne(plasmaImageProject, "data/plasmaImageProject.dat");
  saveHologramImageBinaryOne(plasmaImageRecon, "data/plasmaImageRecon.dat");

  system("script/plot_test.sh");
    
  return 0;

}

int testGetDensitySlice(gsl_vector *forwardProjectData, int numRows, int numCols) {

  int ii, jj, ind,
    rowCen = numRows/2,
    colCen = numCols/2;

  double r, theta,
    norm = 1.0;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {

      if ( (ii == 50) && (jj == 50) ) {
	ind = ii*numCols + jj;
	gsl_vector_set(forwardProjectData, ind, norm);
	continue;
      }

      r = sqrt(gsl_pow_2(ii-rowCen) + gsl_pow_2(jj-colCen));
      theta = atan((jj-colCen)/(double) (ii-rowCen));

      ind = ii*numCols + jj;
      gsl_vector_set(forwardProjectData, ind, 
		     (norm + 0.1*norm*cos(theta*4))*gsl_sf_exp(-gsl_pow_2(r/20)));

    }
  }

  return 1;

}

int testModifyJPEG() {

  char *filenameRef = "/home/fuze/DHI_Images/Calibration/DSC_0009.JPG";

  gsl_matrix* imagePlasma = readJPEGImage(filenameRef);
  
  int numRows = imagePlasma->size1,
    numCols = imagePlasma->size2;

  int ii, jj;

  for (ii = 1634; ii < 2805; ii++) {
    for (jj = 462; jj< 1694; jj++) {

      gsl_matrix_set(imagePlasma, ii, jj, 0);

    }
  }

  saveJPEGImage(imagePlasma, "/home/fuze/DHI_Images/Calibration/Test.JPG");

  return 0;

}
