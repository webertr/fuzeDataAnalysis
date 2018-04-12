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
  

  
