#include "DHI/forceBalance.h"


/******************************************************************************
 *
 * This file includes functions that will use a radial force balance, and
 * uniform plasma drift velocity assumptions to calculate the Azimuthal
 * magnetic field, and temperature as a function of radius, given
 * a radial density profile, and total plasma current
 *
 ******************************************************************************/


/******************************************************************************
 * Function: azimuthBFieldForceBalance
 * Inputs: gsl_vector *, gsl_vector *, gsl_vector *, double
 * Returns: int
 * Description: This function will calculate the azimuthal magnetic field
 * from the radial density profile, and total current Ip. The calculating 
 * is based on the calculation form M. Ross's thesis. The units of the B-field
 * are Tesla
 ******************************************************************************/

int azimuthBFieldForceBalance(gsl_vector *densityProfile, gsl_vector *azimuthalBField, 
			      double Ip, double dr) {
				
  int ii,
    numRows = densityProfile->size;

  double vd,
    QE = 1.60218E-19,
    PI = 3.1416,
    denInt = 0,
    MU_0 = 1.2566E-6,
    thetaInt;


  
  /* Calculating the drift velocity */
  for (ii = 0; ii < numRows; ii++) {
    denInt = denInt + dr*gsl_vector_get(densityProfile, ii)*(dr*ii);
  }

  vd = Ip / (2*PI*QE*denInt);



  /* Calulating the azimuthal magnetic field */

  denInt = 0;
  for (ii = 0; ii < numRows; ii++) {
    denInt = denInt + dr*gsl_vector_get(densityProfile, ii)*(dr*ii);
    thetaInt = 2*PI*QE*vd*denInt; // Total current in amperian loop
    thetaInt = MU_0*thetaInt/(2*PI*(dr*ii));  
    gsl_vector_set(azimuthalBField, ii, thetaInt);

  }


  
  return 0;

}


/******************************************************************************
 * Function: temperatureForceBalance
 * Inputs: gsl_vector *, gsl_vector *, gsl_vector *, gsl_vector *, double
 * Returns: int
 * Description: This function will calculate the radial temperature profile
 * from the radial density profile, azimuthal B-field and total current Ip. 
 * The calculating is based on the calculation form M. Ross's thesis. The temperature
 * is returned in degrees Kelvin. To convert to eV, multiple by 8.618E-5 or 1/11604
 ******************************************************************************/

int temperatureForceBalance(gsl_vector *densityProfile, gsl_vector *azimuthalBField, 
			    gsl_vector *temperature, double Ip, double dr) {
				
  int ii,
    numRows = densityProfile->size,
    edge = numRows - 1;

  double vd,
    QE = 1.60218E-19,
    PI = 3.1416,
    denInt = 0,
    maxDen = gsl_vector_max(densityProfile),
    tempValue = 0,
    tempInt = 0,
    KB = 1.38065E-23;


  /* Finding edge value */
  for (ii = 0; ii < numRows; ii++) {
    if ( fabs(gsl_vector_get(densityProfile, ii)/maxDen) < 0.1) {
      edge = ii;
      break;
    }
  }

  
  /* Calculating the drift velocity */
  for (ii = 0; ii < numRows; ii++) {
    denInt = denInt + dr*gsl_vector_get(densityProfile, ii)*(dr*ii);
  }

  vd = Ip / (2*PI*QE*denInt);




  
  /* Calulating the temperature */

  for (ii = edge; ii >= 0; ii--) {
    tempInt = tempInt + 
      dr*gsl_vector_get(densityProfile, ii)*gsl_vector_get(azimuthalBField, ii);
    tempValue = tempInt * QE * vd/(2*gsl_vector_get(densityProfile, ii)*KB);
    gsl_vector_set(temperature, ii, tempValue);

  }

  
  return 0;

}





/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/


int testForceBalance() {

  // Diameter = 1.65 cm
  // Line integrated density = 2.2E17 cmE-2

  int numRows = 300,
    numCols = 300,
    shotNumber = 180516014;

  double val,
    deltaY = 0.00803917;

  holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT_FORCE;
  param.numRows = numRows;
  param.numCols = numCols;
  param.deltaY = deltaY;

  gsl_matrix *densityProfile = gsl_matrix_alloc(numRows, numCols);
  
  int ii, jj;
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      
      if ( (ii < 5) || (ii > 294)) {
	val = 0.0;
      } else if ( (ii < 80) ) {
	val = 2.2E17*(1-gsl_pow_2((ii-80)/75.0));
      } else if ( (ii > 220) ) {
	val = 2.2E17*(1-gsl_pow_2((ii-220)/75.0));
      } else if ( ii == 150) {
	val = 2.2E17*1.01;
      } else {
	val = 2.2E17;
      }
    
      gsl_matrix_set(densityProfile, ii, jj, val);

    }
  }  
  
  saveMatrixData(densityProfile, param.fileLineIntText);
  saveImageData(densityProfile, param.fileLineInt);

  param.deltaN = 0.02;
  param.centroidNum = 10;
  param.offsetIter = 10;

  //plotImageData(densityProfile, "set title 'Line integrated data'\n");

  gsl_matrix *invertedImage = invertImageDHI(densityProfile, &param);
  saveImageData(invertedImage, param.fileFullInvert);

  gsl_matrix *mSave = readMatrixTextFile(param.fileLeftInvert);

  gsl_vector* xSave = gsl_vector_alloc(numRows);
  gsl_vector* ySave = gsl_vector_alloc(numRows);

  for (ii = 0; ii < numRows; ii++) {
    gsl_vector_set(xSave, ii, ii*param.deltaY);
    gsl_vector_set(ySave, ii, gsl_matrix_get(mSave, ii, 145));
  }

  save2VectorData(xSave, ySave, "data/fitRadialProfile180516014.txt");


  gsl_matrix *radialProfile = readMatrixTextFile("data/fitRadialProfile180516014.txt");

  gsl_vector *xVec = gsl_vector_alloc(numRows),
    *yVec = gsl_vector_alloc(numRows);

  for (ii = 0; ii < numRows; ii++) {
    gsl_vector_set(xVec, ii, gsl_matrix_get(radialProfile, ii, 0));
    gsl_vector_set(yVec, ii, gsl_matrix_get(radialProfile, ii, 1));
  }

  // Removing bad data point
  gsl_vector_set(yVec, 0, gsl_vector_get(yVec, 1));


  //Converting to meters
  gsl_vector_scale(xVec, 1E-2);
  gsl_vector_scale(yVec, 1E2*1E2*1E2);

  gsl_vector *tempProfile = gsl_vector_alloc(numRows);
  gsl_vector *Btheta = gsl_vector_alloc(numRows);

  azimuthBFieldForceBalance(yVec, Btheta, 70E3, param.deltaY);
    
  temperatureForceBalance(yVec, Btheta, tempProfile, 70E3, param.deltaY);

  
  save2VectorData(xVec, Btheta, "data/fitBTheta180516014.txt");
  save2VectorData(xVec, tempProfile, "data/fitTemp180516014.txt");


  /* // Calculate the drift velocity */
  /* double vd, */
  /*   Ip = 70E3, */
  /*   QE = 1.60218E-19, */
  /*   PI = 3.1416, */
  /*   denInt = 0, */
  /*   dr = gsl_vector_get(xVec,1) - gsl_vector_get(xVec,0); */

  /* for (ii = 0; ii < numRows; ii++) { */
  /*   denInt = denInt + dr*gsl_vector_get(yVec, ii)*gsl_vector_get(xVec, ii); */
  /* } */

  /* vd = Ip / (2*PI*QE*denInt); */


  /* // Calculate the theta component of the magnetic field */
  /* // 70 kA, should produce a peak field at 1.1 cm of 1.36 Tesla */
  /* double MU_0 = 1.2566E-6, */
  /*   thetaInt; */

  //gsl_vector *Btheta = gsl_vector_alloc(numRows);

  /* denInt = 0; */
  /* for (ii = 0; ii < numRows; ii++) { */
  /*   denInt = denInt + dr*gsl_vector_get(yVec, ii)*gsl_vector_get(xVec, ii); */
  /*   thetaInt = 2*PI*QE*vd*denInt; // Total current in amperian loop */
  /*   thetaInt = MU_0*thetaInt/(2*PI*gsl_vector_get(xVec, ii));   */
  /*   gsl_vector_set(Btheta, ii, thetaInt); */

  /* } */

  //save2VectorData(xVec, Btheta, "data/fitBTheta180516014.txt");




  /* // Calculating the temperature */
  /* double KB = 1.38065E-23, */
  /*   tempValue = 0, */
  /*   tempInt = 0; */
  /* int edge = 146; */
  
  /* gsl_vector *tempProfile = gsl_vector_alloc(numRows); */

  /* for (ii = edge; ii >= 0; ii--) { */
  /*   tempInt = tempInt +  */
  /*     dr*gsl_vector_get(yVec, ii)*gsl_vector_get(Btheta, ii); */
  /*   tempValue = tempInt * QE * vd/(2*gsl_vector_get(yVec, ii)*KB); */
  /*   gsl_vector_set(tempProfile, ii, tempValue); */

  /* } */

  /* //gsl_vector_scale(tempProfile, 1/11604.0); */

  //save2VectorData(xVec, tempProfile, "data/fitTemp180516014.txt");




  // Calculating pressure
  double MP = 1.673E-27,
    press = 0,
    bPressF,
    KB = 1.38065E-23,
    MU_0 = 1.2566E-6,
    dr = gsl_vector_get(xVec,1) - gsl_vector_get(xVec,0);

  (void) MP;

  gsl_vector *pressure = gsl_vector_alloc(numRows);

  for (ii = 0; ii < numRows; ii++) {
    press = 2*gsl_vector_get(yVec, ii)*KB*gsl_vector_get(tempProfile, ii);
    gsl_vector_set(pressure, ii, press);
  }				

  save2VectorData(xVec, pressure, "data/fitPressure180516014.txt");



  // Calculating the force term from the magnetic field

  gsl_vector *bPressureForce = gsl_vector_alloc(numRows);

  for (ii = 0; ii < (numRows-1); ii++) {
    press = gsl_vector_get(Btheta, ii)/(MU_0*gsl_vector_get(xVec, ii));
    bPressF = press * (gsl_vector_get(Btheta, ii+1)*gsl_vector_get(xVec, ii+1) 
		       - gsl_vector_get(Btheta,ii)*gsl_vector_get(xVec, ii))/dr;
    gsl_vector_set(bPressureForce, ii, bPressF);
  }				

  save2VectorData(xVec, bPressureForce, "data/fitBPressureForce180516014.txt");



  // Calculating the pressure force term from the pressure

  gsl_vector *pressureForce = gsl_vector_alloc(numRows);

  for (ii = 0; ii < (numRows-1); ii++) {
    press = (gsl_vector_get(pressure,ii+1) - gsl_vector_get(pressure,ii))/dr;
    gsl_vector_set(pressureForce, ii, press);
  }				

  save2VectorData(xVec, pressureForce, "data/fitPressureForce180516014.txt");



  // Summing the two force terms

  gsl_vector *totalForce = gsl_vector_alloc(numRows);

  gsl_vector_add(totalForce, pressureForce);
  gsl_vector_add(totalForce, bPressureForce);

  save2VectorData(xVec, totalForce, "data/fitTotalForce180516014.txt");



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
  fprintf(fp, "set yrange[0:]\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  //fprintf(fp, "set title 'T (eV) from fit data for #%d' font '0,18'\n", shotNumber);
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'T (eV)' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2):($2/11604.0) with points ls 2 title 'T'\n", 
  // 	  "data/fitTemp180516014.txt");
  //fprintf(fp, "set title 'B_{/Symbol q} (Tesla) from fit data for #%d' font '0,18'\n", shotNumber);
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2):($2) with points ls 2 title 'B_{/Symbol q}'\n", 
  //	  "data/fitBTheta180516014.txt");
  //fprintf(fp, "set title 'Pressure (atm) from fit data for #%d' font '0,18'\n", shotNumber);
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'Pressure (atm)' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2):($2*9.87E-6) with points ls 2 title 'Pressure'\n", 
  //	  "data/fitPressure180516014.txt");
  //fprintf(fp, "set title 'B_{/Symbol q} force from fit data for #%d' font '0,16'\n", shotNumber);
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'Force (Newtons?)' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2):($2) with points ls 2 title 'Force'\n", 
  //	  "data/fitBPressureForce180516014.txt");
  fprintf(fp, "set title 'Radial force balance from fit to #%d' font '0,16'\n", shotNumber);
  fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'Force (Newtons/m^{3})' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using ($1*1E2):($2*-1) with points ls 2 title 'Pressure force x -1',\\\n", 
  	  "data/fitPressureForce180516014.txt");
  fprintf(fp, "     '%s' using ($1*1E2):($2) with points ls 1 title 'B_{/Symbol q} force'\n", 
   	  "data/fitBPressureForce180516014.txt");
  //fprintf(fp, "set title 'Radial force balance from fit data for #%d' font '0,18'\n", shotNumber);
  //fprintf(fp, "set xlabel 'radius (cm)' font ',16' offset 0,0\n");
  //fprintf(fp, "set ylabel 'Force (Newtons?)' font ',16' offset 0,0\n");
  //fprintf(fp, "plot '%s' using ($1*1E2):($2) with points ls 2 title 'Force'\n", 
  // 	  "data/fitTotalForce180516014.txt");
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

  //plotVectorData(xVec, Btheta, "");



  return 0;


}
