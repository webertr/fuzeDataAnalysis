#include "magneticModel.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int getJR(gsl_matrix **jr, gsl_vector **r, gsl_vector **z, gsl_vector **theta);

/******************************************************************************
 * Function: magneticModel
 * Inputs: void
 * Returns: int
 * Description: This will run call some functions to model some currents in
 * the fuze machine.
 ******************************************************************************/

int magneticModel() {

  gsl_matrix *jr;
  gsl_vector *r, *z, *theta;
  
  getJR(&jr, &r, &z, &theta);

  int numRows = jr->size1,
    numCols = jr->size2;

  int ii, jj;

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("JR(%d, %d): %g\n", ii, jj, gsl_matrix_get(jr, ii, jj));
    }
  }

  for (ii = 0; ii < r->size; ii++) {
    printf("R: %g\n", gsl_vector_get(r, ii));
  }

  for (ii = 0; ii < z->size; ii++) {
    printf("Z: %g\n", gsl_vector_get(z, ii));
  }

  for (ii = 0; ii < theta->size; ii++) {
    printf("Theta: %g\n", gsl_vector_get(theta, ii));
  }
  
  return 0;

}


/******************************************************************************
 * Function: getJR
 * Inputs: void
 * Returns: int
 * Description: This will get the current in the radial direction given the
 * formula, 
 * jr = 1/r x exp(-(i*k+1/row)*(r+z)) x exp(-i x m x theta)
 ******************************************************************************/

static int getJR(gsl_matrix **jr, gsl_vector **r, gsl_vector **z, gsl_vector **theta) {

  int ii, jj, kk,
    m = 1,
    Nr = 100,
    Nz = 100,
    Nt = 100;

  double L = 50,
    row = 1,
    k = 1,
    Rw = 10.0,
    dr = 10.0/((double) Nr),
    dz = L*2/((double) Nz),
    dt = 2*M_PI/((double) Nt),
    rVal, thetaVal, zVal;

  gsl_complex compVal, compExp1,
    compTerm1, compTerm2;
  
  gsl_matrix *jrTemp = gsl_matrix_alloc(Nr, Nz);
  gsl_vector *rTemp = gsl_vector_alloc(Nr);
  gsl_vector *zTemp = gsl_vector_alloc(Nz);
  gsl_vector *thetaTemp = gsl_vector_alloc(Nt);

  for (ii = 0; ii < Nr; ii++) {
    gsl_vector_set(rTemp, ii, dr*ii);
  }

  for (jj = 0; jj < Nz; jj++) {
    gsl_vector_set(zTemp, jj, dz*jj);
  }

  for (kk = 0; kk < Nt; kk++) {
    gsl_vector_set(thetaTemp, kk, dt*kk);
  }

  for (ii = 0; ii < Nr; ii++) {
    rVal = gsl_vector_get(rTemp, ii);
    for (jj = 0; jj < Nz; jj++) {
      zVal = gsl_vector_get(zTemp, jj);
      for (kk = 0; kk < Nt; kk++) {
	thetaVal = gsl_vector_get(thetaTemp, kk);
	compExp1 = gsl_complex_mul(gsl_complex_rect(-(rVal+zVal), 0.0),
				   gsl_complex_rect(1/row, k));
	compTerm1 = gsl_complex_exp(compExp1);
	compTerm2 = gsl_complex_polar(1/rVal, -m*thetaVal);
	compVal = gsl_complex_mul(compTerm1, compTerm2);
	gsl_matrix_set(jrTemp, ii, jj, GSL_REAL(compVal));
      
      }
    }
  }
  
  *jr = jrTemp;
  *r = rTemp;
  *z = zTemp;
  *theta = thetaTemp;
  
  return 0;

}
