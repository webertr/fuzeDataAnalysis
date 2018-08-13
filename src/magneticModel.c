#include "magneticModel.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static double getJZ(double r, double z, double theta, double a, double I0, int m);
static double getBField(double rp, double zp, double thetap, double a, double I0, int m);


/******************************************************************************
 * Function: magneticModel
 * Inputs: void
 * Returns: int
 * Description: This will run call some functions to model some currents in
 * the fuze machine.
 ******************************************************************************/

int magneticModel() {


  return 0;

}


/******************************************************************************
 * Function: getBField
 * Inputs: double, double, double, double, double, int
 * Returns: int
 * Description: This will run call some functions to model some currents in
 * the fuze machine.
 ******************************************************************************/

static double getBField(double rp, double zp, double thetap, double a, double I0, int m) {


  int ii, jj, kk,
    Nr = 200,
    Nz = 200,
    Nt = 200;

  double sum = 0,
    r,z,theta,
    dval,
    curval,
    Rw = .10,                     // 10 cm
    L  = .50,                     // 50 cm, length of 1 side of compression region
    dr = Rw / ((double) Nr),      // r spacing
    dz = 2*L/ ((double) Nz),      // z spacing
    dt = 2*M_PI/ ((double) Nt);   // theta spacing

  double mu_0 = 1.2566E-6;

  for (ii = 0; ii < Nr; ii++) {
    for (jj = 0; jj < Nz; jj++) {
      for (kk = 0; kk < Nt; kk++) {
	r = dr*ii;
	z = dz*jj-L;
	theta = dt*kk;
	curval = getJZ(r, z, theta, a, I0, m);
	dval = gsl_pow_2(zp-z)+gsl_pow_2(r)+gsl_pow_2(rp)-2*r*rp*cos(thetap-theta);
	dval = sqrt(gsl_pow_3(dval));
	sum = sum + curval*dz*r*dr*dt*(rp-r)/dval;
      }
    }
  }
  sum = sum*mu_0/(4*M_PI);

  return sum;

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


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testTotalIP();
static int testAziBField();

int testMagneticModel() {

  testTotalIP();
  testAziBField();
  return 0;
}


static int testTotalIP() {

  double r = 0.1,
    z = 0,
    theta = 0,
    a = 2,
    I0 = 100E3;

  int m = 1,
    ii, jj,
    Nr = 200,
    Nt = 200;

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

  
  printf("Total Current (Should be %g A): %g A\n", I0, sum);

  return 0;

}


static int testAziBField() {

  double mu_0 = 1.2566E-6;
  
  double r = .1,
    z = 0,
    theta = 0,
    a = 0.01,
    I0 = 100E3,
    b,
    b0 = mu_0*I0/(2*M_PI*r);

  int m = 0;
    
  b = getBField(r, z, theta, a, I0, m);

  printf("B field is (Should be %g): %g\n", b0, b);
  
  return 0;

}
