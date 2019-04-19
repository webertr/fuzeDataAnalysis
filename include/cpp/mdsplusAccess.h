#ifndef CPP_MDSPLUSACCESS_H
#define CPP_MDSPLUSACCESS_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <mdsobjects.h>

gsl_vector *readMDSplusVector(int shotNumber, std::string nodeName, std::string treeName);
gsl_vector *readMDSplusVectorDim(int shotNumber, std::string nodeName, std::string treeName);
int getCurrentPulseNumber();

#endif
