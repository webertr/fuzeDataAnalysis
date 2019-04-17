#ifndef CPP_PLOT_H
#define CPP_PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>


int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, const char *yLabel,
		   const char *plotOptions, const char *tempDataFile, const char *tempScriptFile);
int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, const char *y1Label,
		    gsl_vector *yVec2In, const char *y2Label, const char *plotOptions, 
		    const char *tempDataFile, const char *tempScriptFile);

#endif
