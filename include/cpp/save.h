#ifndef CPP_SAVE_H
#define CPP_SAVE_H

#include <stdio.h>
#include <string.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <string>
#include <iostream>
#include <fstream>

int saveVectorData(gsl_vector *vecIn, std::string fileName);
int save2VectorData(gsl_vector *vec1In, gsl_vector *vec2In, std::string fileName);
gsl_vector *readVectorData(std::string fileName);
int testSave();

#endif
