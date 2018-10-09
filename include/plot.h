#ifndef PLOT_H
#define PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "save.h"

int plotImageData (gsl_matrix *mInput, double dx, double dy, char *plotOptions);
int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, char *plotOptions);
int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, char *plotOptions, char *tempDataFile, 
		     char *tempScriptFile);
int plotImageDataFile(char *fileName, double xScale, double yScale, double zScale,
		      char *plotOptions);
int plot1DVectorData (gsl_vector *vecIn, char *plotOptions);
int plotColumnImageDataFile(char *fileName, int numRows, int numCols, int colNum, 
			    char *plotOptions);
int plotMatrixColDataFile (char *fileName, int colNum, char *plotOptions);
int plot2MatrixColDataFile (char *fileName1, int colNum1, char *fileName2, int colNum2, 
			    char *plotOptions);
int plotMatrixColVColDataFile (char *fileName, int colNumX, int colNumY, char *plotOptions);
int plot3VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, char *plotOptions, char *tempDataFile, 
		     char *tempScriptFile);
int plot5VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, char *plotOptions,
		     char *tempDataFile, char *tempScriptFile);
int plotMatrixColVColErrorDataFile (char *fileName, int colNumX, int colNumY, 
				    int colNumError, double xScale, double yScale, 
				    double errorScale, char *plotOptions);
int plotMatrixColVColErrorData2Files2Axes(char *fileName1, int colNumX1, int colNumY1, 
					  int colNumError1, char *label1, double xScale1,
					  double yScale1, double errorScale1,
					  char *fileName2, int colNumX2, int colNumY2, 
					  int colNumError2, double xScale2, double yScale2, 
					  double errorScale2, char *label2, char *plotOptions);
int plotVectorDataWithError(gsl_vector *xIn, gsl_vector *yIn, char *yLabel, gsl_vector *yError,
			    char *plotOptions, char *tempDataFile, char *tempScriptFile);

#endif
