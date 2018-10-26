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
static int testPlot();

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
 * The density should be m^-3. T is in eV
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
    /* Converting to keV */
    T = gsl_vector_get(temperature, ii)*1E-3;
    r = fabs(gsl_vector_get(radius, ii));
    dr = gsl_vector_get(radius,ii+1)-gsl_vector_get(radius,ii);
    /* Converting to m^3/second */
    sigmaV = getSigmaVDDHNE(T)*1E-6;
    neutronProduction = neutronProduction+nD*nD*sigmaV*r*dr;
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
 * In units of cm^3/secon
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
   * T = 0.2 keV, sigmaV = 4.482E-28
   * T = 2.0 keV, sigmaV = 3.11E-21
   * T = 10.0 keV, sigmaV = 6.023E-19
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

  int shotNumber = 180215012;

  gsl_matrix *temperatureM = readDHIMDSplusImage(shotNumber, "\\T_DHI", "fuze", "10.10.10.240");
  gsl_matrix *temperatureMError = readDHIMDSplusImage(shotNumber, "\\T_DHI:Error",
						      "fuze", "10.10.10.240");
  gsl_matrix *densityM = readDHIMDSplusImage(shotNumber, "\\NE_DHI", "fuze", "10.10.10.240");
  gsl_matrix *densityMError = readDHIMDSplusImage(shotNumber, "\\NE_DHI:Error",
						  "fuze", "10.10.10.240");
  gsl_vector *radius = readDHIMDSplusVector(shotNumber, "\\T_DHI:R", "fuze", "10.10.10.240");

  int colNum =  60;
  gsl_vector *temperature = gsl_vector_alloc(temperatureM->size1);
  gsl_vector *temperatureError = gsl_vector_alloc(temperatureMError->size1);
  gsl_vector *density = gsl_vector_alloc(densityM->size1);
  gsl_vector *densityError = gsl_vector_alloc(densityMError->size1);
  gsl_matrix_get_col(temperature, temperatureM, colNum);
  gsl_matrix_get_col(temperatureError, temperatureMError, colNum);
  gsl_matrix_get_col(density, densityM, colNum);
  gsl_matrix_get_col(densityError, densityMError, colNum);

  /* 20 % deterium */
  gsl_vector_scale(density, 0.2);
  //plotVectorData(radius, temperature, "");
  //plotVectorData(radius, density, "");

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
  /* Dead data point. */
  gsl_vector_set(temperature, 0,
		 gsl_vector_get(temperature, 1));
  gsl_vector_set(temperatureError, 0,
		 gsl_vector_get(temperatureError, 1));

  save3VectorData(radius, temperature, temperatureError,
		  "/home/fuze/Github/fuzeDataAnalysis/data/temperature.txt");
  save3VectorData(radius, density, densityError,
		  "/home/fuze/Github/fuzeDataAnalysis/data/density.txt");

  /*
   * Temperature in eV
   * density in m^-3
   * radius in m
   * Lp in m
   * tauPulse in seconds
   */
  double neutrons = getNeutronProduction(density, temperature, radius, maxIndex, Lp, tauPulse);

  printf("Neutron Production: %.2g neutrons/pulse\n", neutrons);
  printf("Lp: %.2g cm\n", Lp*1E2);
  printf("Pulse time width: %.1g usec\n", tauPulse*1E6);

  double nD, T, r, dr, sigmaV, neutronProduction = 0;
  gsl_vector *sigmaVV = gsl_vector_alloc(maxIndex+1);
  gsl_vector *kernel = gsl_vector_alloc(maxIndex+1);
  gsl_vector *sigmaVRadius = gsl_vector_alloc(maxIndex+1);
  for (ii = 0; ii < maxIndex; ii++) {
    nD = gsl_vector_get(density, ii);
    /* Converting to keV */
    T = gsl_vector_get(temperature, ii)*1E-3;
    r = fabs(gsl_vector_get(radius, ii));
    dr = gsl_vector_get(radius,ii+1)-gsl_vector_get(radius,ii);
    /* Converting to m^3/second */
    sigmaV = getSigmaVDDHNE(T)*1E-6;
    gsl_vector_set(sigmaVV, ii, sigmaV);
    gsl_vector_set(sigmaVRadius, ii, gsl_vector_get(radius, ii)-gsl_vector_get(radius, 0));
    gsl_vector_set(kernel, ii, nD*nD*sigmaV*r*Lp*tauPulse*M_PI);
    neutronProduction = neutronProduction+nD*nD*sigmaV*r*dr*Lp*tauPulse*M_PI;
  }

  save2VectorData(sigmaVRadius, sigmaVV,
		  "/home/fuze/Github/fuzeDataAnalysis/data/sigmaV.txt");
  save2VectorData(sigmaVRadius, kernel,
		  "/home/fuze/Github/fuzeDataAnalysis/data/kernel.txt");

  testPlot();
  return 0;

}


static int testPlot() {


  char *gnuPlotFile = "data/gnuplot.sh";
  int status;

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/Ddensity.png'\n");
  fprintf(fp, "set yrange[0:]\n");
  fprintf(fp, "set xrange[0:0.4]\n");
  //fprintf(fp, "set grid\n");
  //fprintf(fp, "set key right top\n");
  //fprintf(fp, "set grid\n");
  //fprintf(fp, "set title 'n_{D} (cm^{-3}) from fit data for #180215012' font '0,18'\n");
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'Deuterium density (cm^{-3})' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2+0.685):($2*1E-6):($3*0.2*1E-6)
  //pt 7 with errorbars title 'n_{D}'\n", "data/density.txt");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/kernel.png'\n");
  //fprintf(fp, "set yrange[0:]\n");
  //fprintf(fp, "set xrange[0:0.004]\n");
  //fprintf(fp, "set grid\n");
  //fprintf(fp, "set key right top\n");
  //fprintf(fp, "set grid\n");
  //fprintf(fp, "set title 'dR/dV for D(d,n)^{3}He data from #180215012' font '0,18'\n");
  //fprintf(fp, "set xlabel 'radius (m)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'dR/dV' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1):($2) pt 7 title 'dR/dV'\n", "data/kernel.txt");
  fprintf(fp, "set terminal pngcairo\n");
  fprintf(fp, "set output 'data/yeild.png'\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set title 'dY/dr for #180215012' font '0,18'\n");
  fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'dY/dr' font ',16' offset 0,0\n");
  fprintf(fp, "set label 'Y = 4 x 10^{5} Neutrons' at graph 0.40,0.5 font ', 20'\n");
  fprintf(fp, "set label '= area under curve' at graph 0.40,0.4 font ', 20'\n");
  fprintf(fp, "set label 'Lp: 15 cm' at graph 0.40,0.70 font ', 20'\n");
  fprintf(fp,
	  "set label '{/Symbol t}: 5 {/Symbol m}sec' at graph 0.40,0.60 font ', 20'\n");  
  fprintf(fp, "plot '%s' using ($1*100):($2) pt 7 title 'dY/dr'\n", "data/kernel.txt");
  fprintf(fp, "pause -1\n");


  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }



  return 1;
  
}
