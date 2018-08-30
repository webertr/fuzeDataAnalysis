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
static int testgetNeutronProduction();

/******************************************************************************
 * Function: getNeutronProduction
 * Inputs: gsl_vector *, gsl_vector *, int, double, double
 * Returns: double
 * Description: This function will return the neutrons generated assuming
 * a z-pinch of the specified density and temperature profiles, length, LP,
 * and duration time tauPulse. The density is the density of deterium.
 * This assumes neutrons are produced by the D-D -> N + HE reaction.
 * The maxIndex needs to be < the size of the radius vector - 2.
 * The temperature shouldbe in eV. The radius should be in meters.
 * The density should be m^-3.
 ******************************************************************************/

double getNeutronProduction(gsl_vector *density, gsl_vector *temperature, gsl_vector *radius,
			    int maxIndex, double Lp, double tauPulse) {

  int ii;
  double neutronProduction = 0.0;
  double r, dr,
    T, nD, sigmaV;

  if (maxIndex > (radius->size - 2)) {
    printf("ERROR. The maximum index must be less then the size of the radius vector\n");
    return -1.0;
  }

  for (ii = 0; ii < maxIndex; ii++) {
    nD = gsl_vector_get(density, ii);
    T = gsl_vector_get(temperature, ii)*1E-3;
    r = gsl_vector_get(radius, ii);
    sigmaV = getSigmaVDDHNE(T);
    dr = gsl_vector_get(radius,ii+1)-gsl_vector_get(radius,ii);
    neutronProduction = neutronProduction + nD*nD*sigmaV*r*dr;
  }

  return neutronProduction*tauPulse*Lp*M_PI;
  
}


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
 * a neutron for D-D fusion. T range is 0.2 - 100 keV. T is in keV
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
  testgetNeutronProduction();

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

static int testgetNeutronProduction() {

  /*
   * The good data is from 180215012.
   * "\\T_DHI" = the temperature in eV, "\\T_DHI:R" = the radius in meters
   * "\\NE_DHI" = The density in m^-3, "\\NE_DHI:R" = The radius in meters
   */
  

  int connectionID,
    connectionStatus,
    signalDescriptor,
    valueStatus;

  int shotNumber = 180215012;

  /* Connecting to mdsplus database "fuze" */
  connectionID = MdsConnect("10.10.10.240");

  /* Checking to see if Connected */
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  /* Opening a tree */
  connectionStatus = MdsOpen("fuze", &shotNumber);

  /* Checking to see if opened correctly */
  if ( !( (connectionStatus & 1) == 1 ) ) {
    fprintf(stderr,"Error opening tree for shot: %d.\n",shotNumber);
    MdsDisconnect();
    return -1;
  }


  int sizeArray,
    null = 0,
    dtype_long = DTYPE_LONG;
  char *buf = "SIZE(\\T_DHI:R)";
  int idesc = descr(&dtype_long, &sizeArray, &null);

  /* use MdsValue to get the signal length */
  valueStatus = MdsValue(buf, &idesc, &null, 0);

  if ( !( (valueStatus & 1) == 1 ) ) {
    fprintf(stderr,"Unable to get length.\n");
    MdsDisconnect();
    return -1;
  }

  printf("Length: %d\n", sizeArray);

  gsl_vector *temperature = gsl_vector_alloc(sizeArray);
  gsl_vector *density = gsl_vector_alloc(sizeArray);
  gsl_vector *radius = gsl_vector_alloc(sizeArray);

  int dtype_dbl = DTYPE_DOUBLE,
    len;
  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, temperature->data, &sizeArray, &null);
 
  /* retrieve signal */
  valueStatus = MdsValue("\\T_DHI", &signalDescriptor, &null, &len);
  if ( !( (valueStatus & 1) == 1 ) ) {
    fprintf(stderr,"Unable to get temperature.\n");
    MdsDisconnect();
    return -1;
  }

  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, density->data, &sizeArray, &null);
 
  /* retrieve signal */
  valueStatus = MdsValue("\\NE_DHI", &signalDescriptor, &null, &len);
  if ( !( (valueStatus & 1) == 1 ) ) {
    fprintf(stderr,"Unable to get temperature.\n");
    MdsDisconnect();
    return -1;
  }

  /* create a descriptor for this signal */
  signalDescriptor = descr(&dtype_dbl, radius->data, &sizeArray, &null);
 
  /* retrieve signal */
  valueStatus = MdsValue("\\T_DHI:R", &signalDescriptor, &null, &len);
  if ( !( (valueStatus & 1) == 1 ) ) {
    fprintf(stderr,"Unable to get temperature.\n");
    MdsDisconnect();
    return -1;
  }


  plotVectorData(radius, temperature, "");
  plotVectorData(radius, density, "");


  double Lp = 15E-2;
  double tauPulse = 5E-6;
  double maxDensity = 0;
  int maxIndex, ii;
  for (ii = 0; ii < density->size; ii++) {
    if (gsl_vector_get(density, ii) > maxDensity) {
      maxDensity = gsl_vector_get(density, ii);
    }
  }
  for (ii = 0; ii < density->size; ii++) {
    if (gsl_vector_get(density, ii) < maxDensity * 0.1) {
      maxIndex = ii;
      break;
    }
  }
  double neutrons = getNeutronProduction(density, temperature, radius, maxIndex, Lp, tauPulse);

  printf("Neutron Production: %g\n", neutrons);

  
  return 0;

}
