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

#include "holoParam.h"
#include "abelInversionDHI.h"
#include "save.h"


gsl_matrix *readJPEGImage(char *fileName);
int saveJPEGImage(gsl_matrix *mInput, char *fileName);
gsl_matrix_complex *fresnel (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix *phaseDiffHolo (gsl_matrix_complex *mInput1, 
			   gsl_matrix_complex *mInput2);
gsl_matrix *extractTwinImage (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix* boxCarSmooth (gsl_matrix *mInput, holographyParameters* param);
gsl_matrix* smoothUnwrapPhase (gsl_matrix *mInput, holographyParameters* param);
gsl_vector *getImageXVectorHol(holographyParameters* param);
gsl_vector *getImageYVectorHol(holographyParameters* param);
int convertPhaseDensity(gsl_matrix *mInput, holographyParameters* param);
gsl_matrix* binMatrix (gsl_matrix *mInput, int width);
gsl_vector* binVector (gsl_vector *vInput, int width);
gsl_matrix *matrixReduceElements(gsl_matrix *mInput, holographyParameters* param);
int hologramMain(holographyParameters* param);
int unwrapCols(gsl_matrix *mInput, holographyParameters* param);
int unwrapRows(gsl_matrix *mInput, holographyParameters* param);
gsl_matrix *rotateImage90CW(gsl_matrix *imagePlasma);
gsl_matrix *rotateImage90CW(gsl_matrix *imagePlasma);
gsl_matrix *flipImageRows(gsl_matrix *imagePlasma);
gsl_matrix *flipImageCols(gsl_matrix *imagePlasma);

#endif
