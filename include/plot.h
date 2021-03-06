#ifndef PLOT_H
#define PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "save.h"

int plotImageData (gsl_matrix *mInput, double dx, double dy, char *plotOptions,
		   char *tempDataFile, char *tempScriptFile);
int plotVectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		    char *plotOptions, char *tempDataFile, char *tempScriptFile);
int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, char *plotOptions, char *tempDataFile, 
		     char *tempScriptFile);
int plotImageDataFile(char *fileName, double xScale, double yScale, double zScale,
		      char *plotOptions, char *tempScriptFile);
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
int plot4VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     char *plotOptions, char *tempDataFile, char *tempScriptFile);
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
int plotImageDimensions(gsl_matrix *mInput, gsl_vector *yVec, gsl_vector *xVec, char *plotOptions,
			char *tempDataFile, char *tempScriptFile);
int plot6PlotsVectorData (gsl_vector *xVec1In, gsl_vector *yVec1In, char *y1Label,
			  gsl_vector *xVec2In, gsl_vector *yVec2In, char *y2Label, 
			  gsl_vector *xVec3In, gsl_vector *yVec3In, char *y3Label, 
			  gsl_vector *xVec4In, gsl_vector *yVec4In, char *y4Label, 
			  gsl_vector *xVec5In, gsl_vector *yVec5In, char *y5Label,
			  gsl_vector *xVec6In, gsl_vector *yVec6In, char *y6Label, 
			  char *plotOptions, char *tempDataFile, 
			  char *tempScriptFile);
int plot3PlotsVectorData (gsl_vector *xVec1In, gsl_vector *yVec1In, char *y1Label, char *y1Key,
			  gsl_vector *xVec2In, gsl_vector *yVec2In, char *y2Label, char *y2Key,
			  gsl_vector *xVec3In, gsl_vector *yVec3In, char *y3Label, char *y3Key,
			  char *plotOptions, char *tempDataFile, 
			  char *tempScriptFile);
int plot8VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, gsl_vector *yVec6In, char *y6Label,
		     gsl_vector *yVec7In, char *y7Label, gsl_vector *yVec8In, char *y8Label, 
		     char *plotOptions, char *tempDataFile, char *tempScriptFile);
int plot7VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, char *y1Label,
		     gsl_vector *yVec2In, char *y2Label, gsl_vector *yVec3In, 
		     char *y3Label, gsl_vector *yVec4In, char *y4Label, 
		     gsl_vector *yVec5In, char *y5Label, gsl_vector *yVec6In, char *y6Label,
		     gsl_vector *yVec7In, char *y7Label,
		     char *plotOptions, char *tempDataFile, char *tempScriptFile);

#endif
