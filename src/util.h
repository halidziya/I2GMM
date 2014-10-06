#pragma once
#include <math.h>
#include <new>
#include "Vector.h"
// From Tom Minka's LightSpeed package
#define INFINITY my_infinity()
#define mem(X,n) (X*) malloc(sizeof(X)*(n));

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif


double gammaln(double x);
double my_infinity(void);
double urand();
double harmean(double x,double y);


extern Vector gl_pc;
void precomputeGammaLn(int size);