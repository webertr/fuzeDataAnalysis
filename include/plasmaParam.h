#ifndef PLASMAPARAM_H
#define PLASMAPARAM_H


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_math.h>

int gyroFrequencies(double bField);
double carbonIonTemperature(double centerWL, double widthWL);

#endif
