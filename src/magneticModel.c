#include "magneticModel.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static double getJZ(double r, double z, double theta, double a, double I0, int m);

/******************************************************************************
 * Function: magneticModel
 * Inputs: void
 * Returns: int
 * Description: This will run call some functions to model some currents in
 * the fuze machine.
 ******************************************************************************/

int magneticModel() {

  double r = 0.1,
    z = 0,
    theta = 0,
    a = 1,
    I0 = 100E3;

  int m = 1,
    ii, jj,
    Nr = 1000,
    Nt = 1000;

  double sum = 0,
    dr = a/((double) Nr),
    dt = 2*M_PI/((double) Nt),
    jz;
  
  for (ii = 0; ii < Nr; ii++) {
    for (jj = 0; jj < Nt; jj++) {
      r = ii*dr;
      theta = dt*jj;
      jz = getJZ(r, z, theta, a, I0, m);
      sum = sum + jz*dr*r*dt;
    }
  }

  
  printf("Total Current: %g\n", sum);
  
  return 0;

}


/******************************************************************************
 * Function: getJZ
 * Inputs: double, double, double, double, double, int
 * Returns: double
 * Description: This will get the current in the z direction given the
 * formula, 
 * jz =  C1 x (1+exp(-i x m x theta)) for r <= a
 * jz = 0 for r > a
 * The total current inside radius a should be I0
 ******************************************************************************/

static double getJZ(double r, double z, double theta, double a, double I0, int m) {

  gsl_complex norm,
    retVal;
  
  if (r > a) {
    return 0.0;
  }

  if ( m == 0) {
    norm = gsl_complex_rect(I0/(M_PI*a*a*2.0), 0);
  } else {
    norm = gsl_complex_div(gsl_complex_rect(I0/(a*a), 0),
			   gsl_complex_add(gsl_complex_rect(M_PI,0),
					   gsl_complex_add(gsl_complex_rect(0,-2.0/(2.0*m)),
							   gsl_complex_polar(1.0/(2.0*m),
									     M_PI/2-2*M_PI*m))));
  }

  retVal = gsl_complex_mul(norm, gsl_complex_add(gsl_complex_rect(1,0),
						 gsl_complex_polar(1, m*theta)));

  return GSL_REAL(retVal);  

}
