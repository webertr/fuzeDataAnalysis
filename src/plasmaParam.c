#include "plasmaParam.h"

/******************************************************************************
 *
 * This is here to calculate some basic plasma parameters and print them out
 *
 ******************************************************************************/

/* Define physical constants in CGS */

//static const double E_0  = 1;              // Permitivity of free space in m^-3 kg^-1 s4 A^2
//static const double MU_0 = 1;              // Permeability of free space in m kg s^-2 A^-2
//static const double KB   = 1.3807E-16;     // Boltznman constant in erg/deg(K)
//static const double QE   = -4.8032E-10;    // Charge of an electron in stat-columbs
static const double QP   = 4.8032E-10;     // Charge of a proton in stat-columbs
static const double C    = 2.998E10;       // Speed of light in cm/sec
static const double MP   = 1.673E-24;      // Mass of proton in g
static const double ME   = 9.109e-28;      // Mass of electron in g
static const double PI   = 3.1416;         // Value of pi


/******************************************************************************
 * Function: gyroFrequencies
 * Inputs: double
 * Returns: int
 * Description: You pass it a magnetic field in CGS units, or Gauss
 * 1E4 Gauss = 1 Tesla. If you pass this 1E4 Gauss = 1 Tesla, you should get
 * Proton Gyro Frequency: 1.52413e+07 Hz at 1 Tesla
 * Electron Gyro Frequency: 2.79929e+10 Hz at 1 Tesla
 ******************************************************************************/

int gyroFrequencies(double bField) {

  double elecGyroFreq = QP*bField/(ME*C)/(2.0*PI);
  double ionGyroFreq = QP*bField/(MP*C)/(2.0*PI);
  double bFieldTesla = bField/(1.0E4);

  printf("\n**************************************************\n");
  printf("\nElectron Gyro Frequency: %g Hz at %g Tesla\n", elecGyroFreq, bFieldTesla);
  printf("\n**************************************************\n");
  printf("\nProton Gyro Frequency: %g Hz at %g Tesla\n", ionGyroFreq, bFieldTesla);
  printf("\n**************************************************\n");

  return 0;

}
  
/******************************************************************************
 * Function: carbonIonTemperature
 * Inputs: double
 * Returns: double, double
 * Description: You pass it a center wavelength, and a wavelength half width,
 * and this will return the temperature assuming thermal doppler broadening. 
 * 
 ******************************************************************************/

double carbonIonTemperature(double centerWL, double widthWL) {

  double ionTemp;
  double carbonMassAMU = 12.01;
  ionTemp = gsl_pow_2(widthWL/centerWL)/gsl_pow_2(1.46E-3)*carbonMassAMU;

  return ionTemp;

}


/******************************************************************************
 * Function: gyroRadius
 * Inputs: double
 * Returns: double, double
 * Description: This will calculate the gyro radius for a proton and electron.
 * Temperature is in eV, magnetic field is in Gauss.
 * For a bField of 1E4 Gauss and a temperature of 100 eV, you should get:
 * Electron Gyro Radius = 0.00238 cm
 * Proton Gyro Radius = 0.102 cm 
 ******************************************************************************/

int gyroRadius(double bField, double temperature) {

  double ionGyroRadius = 1.02E2*sqrt(temperature)/bField;
  double elecGyroRadius = 2.38*sqrt(temperature)/bField;
  double bFieldTesla = bField/1E4;

  printf("\n**************************************************\n");
  printf("\nElectron Gyro Radius: %g cm at %g Tesla\n", elecGyroRadius, bFieldTesla);
  printf("\n**************************************************\n");
  printf("\nProton Gyro Radius: %g cm at %g Tesla\n", ionGyroRadius, bFieldTesla);
  printf("\n**************************************************\n");


  return 1;

}


/******************************************************************************
 * Function: alfvenVelocity
 * Inputs: double
 * Returns: double, double
 * Description: This will calculate the plasma alfven velocity for a given
 * plasma with some magnetic field B, in Gauss, and ion density, ni, in
 * particles per cm^-3. Mu is the mi / mp mass ratio.
 * Should see 6.9E6 for B= 1 Tesla, n= 1E17 cm-3, and mu = 1 (hydrogen plasmas)
 ******************************************************************************/

int alfvenVelocity(double bField, double ionDensity, double mu) {

  double alfvenVelocity = 2.18E11/sqrt(mu)/sqrt(ionDensity)*bField;
  double bFieldTesla = bField/1E4;

  printf("\n**************************************************\n");
  printf("\nAlven Velocity: %g cm/sec at %g Tesla\n", alfvenVelocity, bFieldTesla);
  printf("\n**************************************************\n");

  return 1;

}


/******************************************************************************
 * Function: debyeLength
 * Inputs: double
 * Returns: int
 * Description: This will calculate the plasma Debye Length for a given
 * temperature, T (eV), and density, n (cm^-3). lambda_{D} = 7.43E2*T^(1/2)/n^(1/2)
 * for T in eV, and n in cm^-3.
 * Should see 7.44e-5 cm for T = 1 keV, n= 1E17 cm-3
 ******************************************************************************/

int debyeLength(double temperature, double density) {

  double debyeLength = 7.43E2*sqrt(temperature) / (sqrt(density));

  printf("\n**************************************************\n");
  printf("\nPlasma Debye Length: %g cm\n", debyeLength);
  printf("\n**************************************************\n");

  return 1;

}


  
