#ifndef IMAGEANALYSISDHI_H
#define IMAGEANALYSISDHI_H

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <string.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_fft_complex.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf_exp.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_wavelet.h>


/* 
 * A structure to pass around the necessary parameters
 * for hologram reconstruction and inversion
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
  .rotateImage = 0
};

gsl_matrix *readJPEGImage(char *fileName);
int saveJPEGImage(gsl_matrix *mInput, char *fileName);
gsl_matrix_complex *fresnel (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix *phaseDiffHolo (gsl_matrix_complex *mInput1, 
			   gsl_matrix_complex *mInput2);
gsl_matrix *extractTwinImage (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix* boxCarSmooth (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix* smoothUnwrapPhase (gsl_matrix *mInput, holographyParameters* param);
gsl_vector *getImageXVectorHol(gsl_matrix *mInput, holographyParameters* param);
gsl_vector *getImageYVectorHol(gsl_matrix *mInput, holographyParameters* param);
int saveHologramImageBinary(gsl_matrix *mInput, gsl_vector* xVec, gsl_vector* yVec,
			    char *fileName);
int convertPhaseDensity(gsl_matrix *mInput, holographyParameters* param);
gsl_matrix *readRawFileHol (char * filename, int numRows, int numCols);
gsl_matrix* binMatrix (gsl_matrix *mInput, int width);
gsl_vector* binVector (gsl_vector *vInput, int width);
gsl_matrix* getMatlabImage (char *fileName, int numRows, int numCols);
int saveHologramImageBinaryOne(gsl_matrix *mInput, char *fileName);
gsl_matrix *matrixReduceElements(gsl_matrix *mInput, holographyParameters* param);
int hologramAnalysis(char* baseFileName, char *plasmaFileName,
		     holographyParameters* param,
		     char* fileLeftProfile, char* fileRightProfile,
		     char* fileCentroidLocation);
int lineIntegratedCenterLine(gsl_matrix *mInput, char *fileNameImage, char* fileNameCentroid);
int saveRadialProfileWithPosition(char *fileLeftMatrix, char *fileRightMatrix,
				  int numRows, int numCols,
				  gsl_vector* yVec, int colNum, char *fileLeftProfile,
				  char *fileRightProfile);
int saveLineIntegratedSlice(gsl_matrix *mInput, int colNumber, 
			    char *fileSaveName);
int invertImage(gsl_matrix* imageM, char *fileLeftProfile, char* fileRightProfile,
		char* fileCentroid, holographyParameters* param);
int getRadialDensityProfile(gsl_vector* leftCrossSection, gsl_vector* rightCrossSection, 
			    gsl_vector* crossSection, gsl_vector* centroidLocation,
			    gsl_matrix* projectMatrix,
			    int centroidIterations,  int centroidIndexTest,
			    int colNumber, holographyParameters* param);
gsl_matrix* getInvertedMatrixLeft(int sizeM, double res);
gsl_matrix* getInvertedMatrixRight(int sizeM, double res);
int findDensityOffset(gsl_vector* smallCrossSection, gsl_vector* largeCrossSection,
		      holographyParameters* param);
int solveRightSystemLinearEq(gsl_matrix* mInput, gsl_vector* vInput,
			     gsl_vector* vOutput);
int solveLeftSystemLinearEq(gsl_matrix* mInput, gsl_vector* vInput,
			    gsl_vector* vOutput);
gsl_matrix* getProjectMatrix(int sizeM, double res);
int lineIntegratedSave(gsl_matrix *mInput, char *fileNameImage);
int saveLineIntegratedRow(gsl_matrix *mInput, int rowNumber, 
			  char *fileSaveName);
gsl_matrix *readGSLMatrix(char *fileNameMatrix);
int saveGSLMatrix(gsl_matrix *mInput, char *fileName);
int axialVariationCorrection(gsl_matrix *leftDensityProfile, gsl_matrix *rightDensityProfile,
			     gsl_matrix *imageM, gsl_vector *centroidLocation,
			     holographyParameters* param);
int unwrapCols(gsl_matrix *mInput, holographyParameters* param);
int unwrapRows(gsl_matrix *mInput, holographyParameters* param);
gsl_matrix *rotateImage90CW(gsl_matrix *imagePlasma);
int hologramMain();

#endif
