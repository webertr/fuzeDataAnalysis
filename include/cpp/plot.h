#ifndef CPP_PLOT_H
#define CPP_PLOT_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>


int plotVectorData(gsl_vector *xVecIn, gsl_vector *yVecIn, std::string yLabel,
		   std::string plotOptions, std::string tempDataFile, std::string tempScriptFile);
int plot2VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, std::string plotOptions, 
		    std::string tempDataFile, std::string tempScriptFile);
int plot3VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile);
int plot4VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    gsl_vector *yVec4In, std::string y4Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile);
int plot5VectorData(gsl_vector *xVecIn, gsl_vector *yVec1In, std::string y1Label,
		    gsl_vector *yVec2In, std::string y2Label, 
		    gsl_vector *yVec3In, std::string y3Label,
		    gsl_vector *yVec4In, std::string y4Label,
		    gsl_vector *yVec5In, std::string y5Label,
		    std::string plotOptions, std::string tempDataFile, std::string tempScriptFile);
int plotImageData(gsl_matrix *mInput, double dx, double dy, std::string plotOptions,
		  std::string tempDataFile, std::string tempScriptFile);

#endif
