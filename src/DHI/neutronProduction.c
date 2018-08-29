#include "DHI/neutronProduction.h"

/******************************************************************************
 *
 * This file will contain functions to calculate the neutorn production
 * given a density and temperature profile. The calculation is based
 * on the publication,
 * "Improved formulas for fusion cross-sections and thermal reactivities"
 * Bosch, Hale, 1992 Nuclear Fusion 32 611
 *
 ******************************************************************************/

static int testgetSigmaVDDHNE();
static double getSigmaVDDHNE(double T);

/******************************************************************************
 * Function: getSigmaVDDNHE
 * Inputs: gsl_vector *, gsl_vector *, gsl_vector *, double
 * Returns: double
 * Description: This function will return the value of the
 * velocity difference between two particles multiplied by x the fusion 
 * cross section for those two particles averaged over a maxwellian distribution
 * of some temperature T. The cross section multiplied by the velocity
 * distribution gives the collision volume per second. The cross section
 * is for the D + D -> N + HE reaction. This is the reaction that will emit
 * a neutron for D-D fusion. T range is 0.2 - 100 keV
 ******************************************************************************/

static double getSigmaVDDHNE(double T) {

  const double C1 = 5.43360E-12,
    C2 = 5.85778E-3,
    C3 = 7.68222E-3,
    C4 = 0.0,
    C5 = -2.96400E-6,
    C6 = 0.0,
    C7 = 0.0;

  const double MC2 = 937814;

  const double BG = 31.3970;

  double tsi,
    theta,
    sigmaV;

  theta = T / (1 - (T*(C2+T*(C4+T*C6)))/(1+T*(C3+T*(C5+T*C7))));
  tsi = pow(BG*BG/(4.0*theta), 1.0/3.0);
  sigmaV = C1*theta*sqrt(tsi/(MC2*T*T*T))*exp(-3*tsi);

  return sigmaV;

}



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int testNeutronProduction() {

  testgetSigmaVDDHNE();

  return 0;

}


static int testgetSigmaVDDHNE() {

  /*
   * T = 0.2, sigmaV = 4.482E-28
   * T = 2.0, sigmaV = 3.11E-21
   * T = 10.0, sigmaV = 6.023E-19
   *
   */

  printf("T = 0.2 keV, sigmaV = %g (Should be 4.482E-28)\n",
	 getSigmaVDDHNE(0.2));

  printf("T = 2.0 keV, sigmaV = %g (Should be 3.11E-21)\n",
	 getSigmaVDDHNE(2.0));

  printf("T = 10.0 keV, sigmaV = %g (Should be 6.023E-19)\n",
	 getSigmaVDDHNE(10.0));

  return 0;

}
