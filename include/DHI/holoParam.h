#ifndef HOLOPARAM_H
#define HOLOPARAM_H

/* 
 * A structure to pass around the necessary parameters for hologram reconstruction and inversion
 */
struct holographyparameters { 
  double res;                   // Resolution of Camera CCD Pixels (3.85E-6 m)
  double lambda;                // Wavelength of laser (532E-9 m)
  double d;                     // Reconstruction distance (0.5 m)
  double deltaN;                // Density offset delta (5E18)
  int numRows;                  // Number of rows of image (6016)
  int numCols;                  // Number of columns of image (4000)
  double e0;                    // The permitivity of free space
  double q;                     // The electron Charge
  double c;                     // The speed of light
  double me;                    // The electro mass
  int xLL;                      // X/Column number of far left pixel defining first order image
                                // to extract from the holographic reconstruction
  int yLL;                      // Y/Row number of bottom most pixel defining first order image
                                // to extract from the holographic reconstruction
  int xUR;                      // X/Column number of far right pixel defining first order image
                                // to extract from the holographic reconstruction
  int yUR;                      // Y/Row number of top most pixel defining first order image
                                // to extract from the holographic reconstruction
  int hyperbolicWin;            // The fitting parameter for the hyperbolic smooth parameters (8)
  int sampleInterval;           // Sampling interval of line-integrated density (10)
  int centroidNum;              // number of centroids to vary +/- around maximum (10)
  int offsetIter;               // Number of offset iterations (15)
  int boxCarSmoothWidth;        // Box car smoothing width to apply to unwrapped phase (8)
  double unwrapThresh;          // Threshold to trigger the phase unwrapping
  int signTwin;                 // Sign to density conversion +/-1. Depends on laser setup (-1)
  double deltaX;                // The distance between column pixels for the current 2d image
  double deltaY;                // The distance between row pixels for the current 2d image
  int saveHologram;             // 1 means to save the hologram before extracting twin image
  char fileHologram[100];       // The name of the file for the hologram data
  int invertImage;              // 1 means to perform an inversion
  int saveLineInt;              // 1 means to save the line integrated data
  int saveWrappedPhase;         // 1 means to save the file before it's unwrapped
  char fileWrappedPhase[100];   // The name of the file to save the wrapped phase of the hologram
  char fileLineIntPos[100];     // The name of file for the line integrated data with positon info
  char fileLineInt[100];        // The name of the file for the line integrated data
  char fileLineIntText[100];    // The name of the file for the line integrated data in Ascii form
  char fileLeftInvert[100];     // The name of the file for the left abel inverted data
  char fileRightInvert[100];    // The name of the file for the right abel inverted data
  char fileFullInvert[100];     // The name of the file for the full abel inverted data
  char fileFullInvertPos[100];  // The name of file for the full abel inverted data with pos info
  char fileFullInvertText[100]; // The name of the file for the full abel inverted data text file
  char fileCentroid[100];       // The name of the file for the centroid file
  char fileBTheta[100];         // The name of the file for the centroid file
  char fileTemperature[100];    // The name of the file for the centroid file
  double R_electrode;           // value of the radius of the inner electrode
  int rotateImage;              // 1 means to rotate the image by 90 degrees CW
  int flipImageRows;            // 1 means to flip the image rows
  int flipImageCols;            // 1 means to flip the images cols
  int refSubtract;              // 1 means to subtract the reference image
  double zPosition;             // The z position of the hologram at the center of the image
  char filePlasma[100];         // The name of the baseline image
  char fileRef[100];            // The name of the plasma image
  int axialCorrection;          // If 1, will correct for axial phase variation of hologram
  int convertDensity;           // If 1, this will convert from the phase to density
  int Z;                        // Atomic number of main ion population (1 = H, 2 = He)
  char fileError[100];         // The name of the error bar file
};

typedef struct holographyparameters holographyParameters;

#endif
