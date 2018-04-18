#ifndef HOLOPARAM_H
#define HOLOPARAM_H


/* 
 * A structure to pass around the necessary parameters for hologram reconstruction and inversion
 */
struct holographyparameters { 
  double res;            // Resolution of Camera CCD Pixels (3.85E-6 m)
  double lambda;         // Wavelength of laser (532E-9 m)
  double d;              // Reconstruction distance (0.5 m)
  double deltaN;         // Density offset delta (5E18)
  int numRows;           // Number of rows of image (6016)
  int numCols;           // Number of columns of image (4000)
  int xLL;               // X/Column number of far left pixel defining first order image
                         // to extract from the holographic reconstruction
  int yLL;               // Y/Row number of bottom most pixel defining first order image
                         // to extract from the holographic reconstruction
  int xUR;               // X/Column number of far right pixel defining first order image
                         // to extract from the holographic reconstruction
  int yUR;               // Y/Row number of top most pixel defining first order image
                         // to extract from the holographic reconstruction
  int hyperbolicWin;     // The fitting parameter for the hyperbolic smooth parameters (8)
  int sampleInterval;    // Sampling interval of line-integrated density (10)
  int centroidNum;       // number of centroids to vary +/- around maximum (10)
  int offsetIter;        // Number of offset iterations (15)
  int boxCarSmoothWidth; // Box car smoothing width to apply to unwrapped phase (8)
  double unwrapThresh;   // Threshold to trigger the phase unwrapping
  int signTwin;          // Sign to density conversion +/-1. Depends on laser setup (-1)
  double deltaX;         // The distance between column pixels for the current 2d image
  double deltaY;         // The distance between row pixels for the current 2d image
  int debugPhase;        // 1 means save and plot a col profile of the phase and unwrapped phase
  int debugPhaseColNum;  // Col number to save for the phase and unwrapped phase plot
  int debugPhaseRowNum;  // Row number to save for the phase and unwrapped phase plot
  int hologramPreview;   // 1 means to preview the hologram before extracting twin image
  int plotRadialProfile; // 1 means to plot the inverted radial profile and slice through
                         // the line integrated image (at plotColNum)
  int invertImage;       // 1 means to perform an inversion
  int plotColNum;        // The column number to plot for the inverted radial profile and
                         // slice of the line integrated image.
  int plotLineIntegrated;// 1 means to plot the line integrated data with centroid location
  int plotRawHologram;   // 1 means it will plot a raw hologram
  int plotRawHologramRow;// 1 means it will plot a raw hologram row
  int plotRawHologramCol;// 1 means it will plot a raw hologram column
  int plotTwinImage;     // 1 means it will plot this twin image
  double R_electrode;    // value of the radius of the inner electrode
  int rotateImage;       // 1 means to rotate the image by 90 degrees CW
  int flipImageRows;     // 1 means to flip the image rows
  int flipImageCols;     // 1 means to flip the images cols
  int refSubtract;       // 1 means to subtract the reference image
  double zPosition;      // The z position of the hologram at the center of the image
};

typedef struct holographyparameters holographyParameters;

/*
 * This is the struct that will hold the default values,
 * static means it should only be visible to the source file
 * that includes it, so the compiler won't complain when linking, I think?
 */
static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT = {
  .res = 3.85E-6, 
  .lambda = 532E-9,
  .d = 0.5,
  .deltaN = 5E21,
  .numRows = 4000,
  .numCols = 6016,
  .xLL = 700,
  .yLL = 2400,
  .xUR = 1400,
  .yUR = 3650,
  .hyperbolicWin = 8,
  .sampleInterval = 10,
  .centroidNum = 10,
  .offsetIter = 15,
  .boxCarSmoothWidth = 8,
  .unwrapThresh = 1.0 * M_PI,
  .signTwin = -1,
  .deltaX = 0.000115,
  .deltaY = 0.000115,
  .debugPhase = 0,
  .debugPhaseColNum = 10,
  .debugPhaseRowNum = 10,
  .hologramPreview = 0,
  .plotRadialProfile = 1,
  .invertImage = 1,
  .plotColNum = 5,
  .plotLineIntegrated = 0,
  .plotRawHologram = 0,
  .plotRawHologramRow = 20,
  .plotRawHologramCol = 20,
  .plotTwinImage = 1,
  .R_electrode = 0.100838,
  .rotateImage = 0,
  .flipImageRows = 0,
  .flipImageCols = 0,
  .refSubtract = 1,
  .zPosition = 0.0
};


#endif
