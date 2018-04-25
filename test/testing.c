#include "testing.h"

int testPlasmaParam() {

  gyroFrequencies(1E4);

  return 0;

}

int testMagneticData() {

  int shotNumber = 170817005,
    sigSize = getSignalLengthMDSplus("\\b_n95_000_sm", shotNumber);

  gsl_vector *data = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(170817005, "\\b_n95_000_sm", data, time);

  printf("Data Length: %d", (int) data->size);
  printf("20th Element: %f\n", gsl_vector_get(data, 20));

  plotVectorData(time, data, "set xrange[0:50E-6]");


  gsl_vector_free(data);
  gsl_vector_free(time);

  return 0;

}


int testPlotImageData() {

  char *filenamePlasma = "/home/fuze/DHI_Images/Calibration/DSC_0088.JPG";

  gsl_matrix* imagePlasma = readJPEGImage(filenamePlasma);  

  plotImageData(imagePlasma);

  return 0;

}

gsl_matrix *testSmoothRows(gsl_matrix *mIn, int smoothWidth) {

  int numRows = mIn->size1,
    numCols = mIn->size2,
    ii, jj, kk;

  double sum = 0;

  gsl_matrix *newM = gsl_matrix_alloc(numRows, numCols);
  
  for (ii = 0; ii < numRows; ii++) {
    for (jj = smoothWidth; jj < (numCols-smoothWidth); jj++) {
      
      sum = 0;
      for (kk = -smoothWidth; kk <= smoothWidth; kk++) {

	sum = sum + gsl_matrix_get(mIn, ii, jj+kk);

      }
      gsl_matrix_set(newM, ii, jj, sum/(double) smoothWidth);
    }
  }

  return newM;
  
}
    

int testGetMaxIndexComplex(gsl_vector_complex *vIn) {

  int numCols = vIn->size,
    ii, retI = 0;

  gsl_complex numC;

  double maxD = 0,
    maxTest;
  
  for (ii = 0; ii < numCols; ii++) {

    numC = gsl_vector_complex_get(vIn, ii);

    maxTest = gsl_complex_arg(numC);
    
    if (maxD < maxTest) {

      retI = ii;
      maxD = maxTest;

    }

  }

  return retI;

}

  
void testSmoothData() {

  gsl_matrix *data = readGSLMatrix("data/plasmaMatrix.dat");
  
  int numRows = data->size1,
    numCols = data->size2,
    ii, jj,
    maxI;

  gsl_complex numC;
  
  gsl_matrix_complex* dataC = gsl_matrix_complex_alloc(numRows, numCols);

  gsl_vector_complex_view dataV;
  
  for (ii = 0; ii < numRows; ii++) {
    for(jj = 0; jj < numCols; jj++) {

      numC = gsl_complex_rect(gsl_matrix_get(data, ii, jj), 0);
      
      gsl_matrix_complex_set(dataC, ii, jj, numC);

    }
  }
  
  gsl_fft_complex_wavetable * wavetableCols;
  gsl_fft_complex_workspace * workspaceCols;
  gsl_fft_complex_wavetable * wavetableRows;
  gsl_fft_complex_workspace * workspaceRows;

  wavetableCols = gsl_fft_complex_wavetable_alloc(numCols);
  workspaceCols = gsl_fft_complex_workspace_alloc(numCols);
  wavetableRows = gsl_fft_complex_wavetable_alloc(numRows);
  workspaceRows = gsl_fft_complex_workspace_alloc(numRows);

  gsl_complex_packed_array dataP = &(dataC->data[0]);

  for (ii = 0; ii < numRows; ii++) {

    dataP = &(dataC->data[ii*numCols*2]);
    gsl_fft_complex_forward(dataP, 1, (size_t) numCols,
			    wavetableCols, workspaceCols);

  }

  for (jj = 0; jj < numCols; jj++) {

    dataP = &(dataC->data[jj*numRows*2]);
    gsl_fft_complex_forward(dataP, 1, (size_t) numRows,
			    wavetableRows, workspaceRows);

  }

  numC = gsl_complex_rect(0, 0);

  int zeroOffset = 10;

  double maxIL, maxIU;
  
  for (ii = 0; ii < numRows; ii++) {

    dataV = gsl_matrix_complex_row(dataC, ii);
    maxI = testGetMaxIndexComplex(&dataV.vector);
    
    maxIL = maxI - zeroOffset;
    maxIU = maxI + zeroOffset;
    if (maxIL < 0) {

      maxIL = 0;

    }

    if (  maxIU >= numCols) {

      maxIU = numCols;

    }
    
    for (jj = maxIL; jj < maxIU; jj++) {
      
      //gsl_vector_complex_set(&dataV.vector, jj, numC);

    }
      
  }

  for (jj = 0; jj < numCols; jj++) {

    dataV = gsl_matrix_complex_column(dataC, jj);
    maxI = testGetMaxIndexComplex(&dataV.vector);

    maxIL = maxI - zeroOffset;
    maxIU = maxI + zeroOffset;
    if (maxIL < 0) {

      maxIL = 0;

    }

    if (  maxIU >= numRows) {

      maxIU = numRows;

    }
    
    for (ii = maxIL; ii < maxIU; ii++) {
      
      //gsl_vector_complex_set(&dataV.vector, ii, numC);

    }
      
  }

  for (ii = 0; ii < numRows; ii++) {

    dataP = &(dataC->data[ii*numCols*2]);
    gsl_fft_complex_backward(dataP, 1, (size_t) numCols,
  			     wavetableCols, workspaceCols);

  }

  for (jj = 0; jj < numCols; jj++) {

    dataP = &(dataC->data[jj*numRows*2]);
    gsl_fft_complex_backward(dataP, 1, (size_t) numRows,
  			     wavetableRows, workspaceRows);

  }

  for (ii = 0; ii < numRows; ii++) {
    for(jj = 0; jj < numCols; jj++) {
      
      gsl_matrix_set(data, ii, jj,
		     GSL_REAL(gsl_matrix_complex_get(dataC, ii, jj)));

    }
  }

  lineIntegratedSave(data, "data/lineIntegratedNew.dat");

  system("script/plot_line_integrated.sh");

  return;

}
  
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

  char *filenameRef = "/home/webertr/DHI_Images/Calibration/DSC_0009.JPG";

  gsl_matrix* imagePlasma = readJPEGImage(filenameRef);
  
  //int numRows = imagePlasma->size1,
  //numCols = imagePlasma->size2;

  int ii, jj;

  for (ii = 1634; ii < 2805; ii++) {
    for (jj = 462; jj< 1694; jj++) {

      gsl_matrix_set(imagePlasma, ii, jj, 0);

    }
  }

  saveJPEGImage(imagePlasma, "/home/fuze/DHI_Images/Calibration/Test.JPG");

  return 0;

}


int testRealFFT() {
  
  int i, n = 100;
  double data[n];

  gsl_matrix *testData = gsl_matrix_alloc(1, n+1);
  
  gsl_fft_real_wavetable * real;
  gsl_fft_real_workspace * work;

  for (i = 0; i < n; i++)
    {
      data[i] = 0.0;
      gsl_matrix_set(testData, 0, i+1, 0.0);
    }

  for (i = n / 3; i < 2 * n / 3; i++)
    {
      data[i] = 1.0;
      gsl_matrix_set(testData, 0, i+1, 1.0);
    }

  work = gsl_fft_real_workspace_alloc (n);
  real = gsl_fft_real_wavetable_alloc (n);

  gsl_fft_real_transform (data, 1, n, real, work);

  for (i = 0; i < n; i++)
    {
      
    }
  printf ("\n");


  gsl_fft_real_workspace_free (work);

  gsl_matrix_set(testData, 0, 0, 0.0);
  
  getAzimuthalArrayModes(testData);

  for (i = 0; i < n-1; i++)
    {
      printf("%d: %g \t\t%g\n", i, gsl_matrix_get(testData, 0, i+1), data[i]);
    }


  return 0;

}


int testAziMode() {
  
  int ii, n = 8;
  double data[n];
  double val;

  gsl_matrix *testData = gsl_matrix_alloc(1, n+1);

  double m0 = 3.4,
    m1 = 1.4,
    m2 = 0,
    m3 = 1.8;
  
  for (ii = 0; ii < n; ii++) {

    val = m0 +m1*gsl_sf_sin(1*ii*2*M_PI/8)+m2*gsl_sf_sin(2*ii*2*M_PI/8)+m3*gsl_sf_sin(3*ii*2*M_PI/8);
    data[ii] = val;
    gsl_matrix_set(testData, 0, ii+1, val);
    //printf("Value for %d: %g\n", ii, val);

  }
    
  getAzimuthalArrayModes(testData);

  printf("m = 0: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 1), m0);
  printf("m = 1: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 2), m1/m0);
  printf("m = 2: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 3), m2/m0);
  printf("m = 3: %g (Should be %g)\n", gsl_matrix_get(testData, 0, 4), m3/m0);

  gsl_fft_real_wavetable * real;
  gsl_fft_real_workspace * work;

  work = gsl_fft_real_workspace_alloc (n);
  real = gsl_fft_real_wavetable_alloc (n);

  gsl_fft_real_transform (data, 1, n, real, work);

  for (ii = 0; ii < n; ii++) {
    //printf("Data[%d]: %g\n", ii, data[ii]);
  }

  gsl_fft_real_workspace_free (work);
  
  return 0;

}
