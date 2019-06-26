#ifndef CPP_PSQLACCESS_H
#define CPP_PSQLACCESS_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <pqxx/pqxx>
#include <iostream>

#include "mdsplusAccess.h"

gsl_vector_long *getRecentShotNumbers(int shotNums);
gsl_vector_long *getShotNumbersBeforePulse(int shotNums, long shotNumber);
int testPsqlAccess();

#endif
