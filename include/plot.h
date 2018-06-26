#ifndef PLOT_H
#define PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "save.h"

int plotImageData (gsl_matrix *mInput, char *plotOptions);
int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, char *plotOptions);
int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		    char *plotOptions);
int plotImageDataFile(char *fileName, double xScale, double yScale, double zScale,
		      char *plotOptions);
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
				    int colNumError, double xScale, double yScale, 
				    double errorScale, char *plotOptions);
int plotMatrixColVColErrorData2Files2Axes(char *fileName1, int colNumX1, int colNumY1, 
					  int colNumError1, char *label1, double xScale1,
					  double yScale1, double errorScale1,
					  char *fileName2, int colNumX2, int colNumY2, 
					  int colNumError2, double xScale2, double yScale2, 
					  double errorScale2, char *label2, char *plotOptions);

#endif
