#ifndef PLOT_H
#define PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "save.h"

int plotImageData (gsl_matrix *mInput, char *plotOptions);
int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, char *plotOptions);
int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		    char *plotOptions);
int plotImageDataFile(char *fileName, char *plotOptions);
int plot1DVectorData (gsl_vector *vecIn, char *plotOptions);
int plotColumnImageDataFile(char *fileName, int numRows, int numCols, int colNum, 
			    char *plotOptions);
int plotMatrixColDataFile (char *fileName, int colNum, char *plotOptions);
int plot2MatrixColDataFile (char *fileName1, int colNum1, char *fileName2, int colNum2, 
			    char *plotOptions);
int plotMatrixColVColDataFile (char *fileName, int colNumX, int colNumY, char *plotOptions);
int plot5VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		     gsl_vector *yVec3In, gsl_vector *yVec4In, gsl_vector *yVec5In,
		     char *y1Label, char *y2Label, char *y3Label, char *y4Label,
		     char *y5Label, char *plotOptions);
int plotMatrixColVColErrorDataFile (char *fileName, int colNumX, int colNumY, 
				    int colNumError, char *plotOptions);

#endif
