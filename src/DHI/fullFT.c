/******************************************************************************
 * Function: full2DFT
 * Inputs: gsl_matrix_complex*
 * Returns: int
 * Description: A full 2D FT in all the gory detail.
 ******************************************************************************/

static int full2DFT (gsl_matrix_complex *mInput,
		     int rowMin, int rowMax,
		     int colMin, int colMax) {

  int p, q, j, k,
    m = mInput->size1, // Number of Rows
    n = mInput->size2; // Number of Cols

  gsl_matrix_complex *fftMatrix = gsl_matrix_complex_alloc(m, n);
  
  gsl_complex sum1, sum2, sumtot, exp1;
  double tempIm;
  int rowW = rowMax - rowMin;
  int colW = colMax - colMin;
  int iter = 0;
  
  /* Doing an FT in full detail */
  for (p = rowMin; p < rowMax; p++) {
    for (q = colMin; q < colMax; q++) {

      printf("Percentage left: %f\n", 100*iter/((float) (rowW*colW)));
      
      sumtot = gsl_complex_rect(0,0);
      for (j = 0; j < m; j++) {
	for (k = 0; k < n; k++) {

	  tempIm = -2*M_PI/m*j*p-2*M_PI/n*k*q;
	  exp1 = gsl_complex_rect(0, tempIm);
	  
	  sum1 = gsl_complex_exp(exp1);
	  sum2 = gsl_complex_mul(sum1,
				 gsl_matrix_complex_get(mInput, j, k));
	  
	  sumtot = gsl_complex_add(sum2, sumtot);
	  
	}
      }

      gsl_matrix_complex_set(fftMatrix, p, q, sumtot);

      iter = iter + 1;      
    }
  }

  gsl_matrix_complex_memcpy(mInput, fftMatrix);

  gsl_matrix_complex_free(fftMatrix);
  
  return 0;

}
