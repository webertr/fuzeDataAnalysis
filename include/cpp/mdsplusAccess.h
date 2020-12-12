#ifndef CPP_MDSPLUSACCESS_H
#define CPP_MDSPLUSACCESS_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <mdsobjects.h>
#include <math.h>

gsl_vector *readMDSplusVector(int shotNumber, std::string nodeName, std::string treeName);
gsl_vector *readMDSplusVectorDim(int shotNumber, std::string nodeName, std::string treeName);
int getCurrentPulseNumber();
double readMDSplusDouble(int shotNumber, std::string nodeName, std::string treeName);
int testMDSplusAccess();
int writeMDSplusDouble(double valIn, int shotNumber, 
		       std::string nodeName, std::string treeName);
int writeMDSplusVector(gsl_vector *vecIn, int shotNumber, 
		       std::string nodeName, std::string treeName);
int writeMDSplusMatrix(gsl_matrix_ushort *matIn, int shotNumber, 
		       std::string nodeName, std::string treeName);
gsl_matrix_ushort *readMDSplusMatrix(int shotNumber, std::string nodeName, std::string treeName);
bool readMDSplusVectorWrite(int shotNumber, std::string nodeName,
			    std::string treeName, std::string fileName);
int writeMDSplusSignal(gsl_vector *vecX, gsl_vector *vecY, int shotNumber, 
		       std::string nodeName, std::string treeName);

#endif
