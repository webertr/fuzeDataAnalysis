#include "cpp/analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

static int plotCompCurrent();

/******************************************************************************
 * Function: plotPostShotAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

int plotPostShotAnalysis() {

  int shotNumber,
    currShotNumber = getCurrentPulseNumber();
  
  printf("\nEnter Pulse Number> ");
  scanf("%d", &shotNumber);

  if (shotNumber <= 0) {
    shotNumber = currShotNumber+shotNumber;
  }

  getchar();
  
  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    plotCompCurrent();
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }

  if (0) {
    
  }

  return 0;

}


/******************************************************************************
 * Function: kiranaImageAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will analyze images from the kirana
 ******************************************************************************/

int kiranaImageAnalysis() {

  Kirana kirana("/home/fuze/webertrNAS/kiranaVideo/170915068.SVF");
  
  kirana.setCropRow(315, 410);
  //kirana.saveImage(0, "data/kiranaImage.dat");
  
  gsl_matrix *image = kirana.getImage(1);

  std::string keywords = "set palette gray\nset title 'corn'"; 
  plotImageData(image, 1, 1, keywords, "data/temp.dat", "data/temp.sh");

  return 0;

}


/******************************************************************************
 * Function: plotCompCurrent
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotCompCurrent() {

  return 0;

}
