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
    norm = gsl_complex_rect(I0/(M_PI*a*a*2), 0);
  }
  else {
    norm = gsl_complex_rect(I0/(M_PI*a*a), 0);
  }

  retVal = gsl_complex_mul(norm, gsl_complex_add(gsl_complex_rect(1,0),
						 gsl_complex_polar(1, -m*theta)));

  return GSL_REAL(retVal);  

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testTotalIP();
static int testAziBField();
static int testAziBFieldMode();

int testMagneticModel() {

  if (1) {
    testTotalIP();
    testAziBField();
  }
  testAziBFieldMode();
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

  int m = 1;
    
  b = getBField(r, z, theta, a, I0, m);

  printf("B field is (Should be %g): %g\n", b0, b);
  
  return 0;

}


static int testAziBFieldMode() {
  
  int n = 8,
    m1 = 0,
    ii, jj,
    Nx = 200,
    Ny = 200;

  double rw = .1,
    x, y, r, jz,
    z = 0,
    theta = 0,
    a = 0.03,
    I0 = 100E3,
    bArray[n],
    mu_0 = 1.2566E-6,
    b0 = mu_0*I0/(2*M_PI*rw),
    dx = 2*rw/Nx,
    dy = 2*rw/Ny,
    norm;
  
  for (ii = 0; ii < n; ii++) {
    theta = ii*2*M_PI/((double) n);
    bArray[ii] = getBField(rw, z, theta, a, I0, m1);
  }

  gsl_matrix *bField = gsl_matrix_alloc(Nx, Ny);
  
  for (ii = 0; ii < Nx; ii++) {
    for (jj = 0; jj < Ny; jj++) {
      x = ii*dx - rw;
      y = jj*dy - rw;
      r = sqrt(gsl_pow_2(x)+gsl_pow_2(y));
      theta = atan(y/x);
      if ( (x<=0) && (y<0) ) {
	theta = theta - M_PI;
      }
      if ( (x<=0) && (y>=0) ) {
	theta = theta + M_PI;
      }
      jz = getJZ(r, z, theta, a, I0, m1);
      gsl_matrix_set(bField, ii, jj, jz);
    }
  }
  
  gsl_fft_real_wavetable * wavetableCols;
  gsl_fft_real_workspace * workspaceCols;

  wavetableCols = gsl_fft_real_wavetable_alloc(n);
  workspaceCols = gsl_fft_real_workspace_alloc(n);

  gsl_fft_real_transform(bArray, 1, (size_t) n, wavetableCols, workspaceCols);
  bArray[0] = bArray[0]/8;
  norm = 2/8.0;
  bArray[1] = sqrt(gsl_pow_2(bArray[1]) + gsl_pow_2(bArray[2]))*norm;
  bArray[2] = sqrt(gsl_pow_2(bArray[3]) + gsl_pow_2(bArray[4]))*norm;
  bArray[3] = sqrt(gsl_pow_2(bArray[5]) + gsl_pow_2(bArray[6]))*norm;

  printf("m = 0: %g (Should be: %g)\n", bArray[0], b0);
  printf("m = 1: %g \n", bArray[1]);
  printf("(m = 1)/(m = 0): %g\n", bArray[1]/bArray[0]);
  printf("(m = 2)/(m = 0): %g\n", bArray[2]/bArray[0]);
  printf("Golingo's Formula (m1/m0=2*dr/RW) Delta r = %g\n", bArray[1]/bArray[0]/2.0*rw);

  gsl_fft_real_wavetable_free(wavetableCols);
  gsl_fft_real_workspace_free(workspaceCols);

  plotImageData(bField, dx, dy, "");

  gsl_matrix_free(bField);
  
  return 0;

}
