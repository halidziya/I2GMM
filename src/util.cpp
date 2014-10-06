/* C implementations of useful functions.
 * Written by Tom Minka (unless otherwise noted).
 */

#define _USE_MATH_DEFINES 1
#include <stdlib.h>
#include <float.h>
#include "util.h"
#include <math.h>

#ifdef _MSC_VER
#define finite _finite
#define isnan _isnan
#endif

#ifdef	 __USE_ISOC99
/* INFINITY and NAN are defined by the ISO C99 standard */
#else
double my_infinity(void) {
  double zero = 0;
  return 1.0/zero;
}
double my_nan(void) {
  double zero = 0;
  return zero/zero;
}
#define INFINITY my_infinity()
#define NAN my_nan()
#endif


double gammaln(double x)
{
  #define M_lnSqrt2PI 0.91893853320467274178
  static double gamma_series[] = {
    76.18009172947146,
    -86.50532032941677,
    24.01409824083091,
    -1.231739572450155,
    0.1208650973866179e-2,
    -0.5395239384953e-5
  };
  int i;
  double denom, x1, series;
  if(x < 0) return NAN;
  if(x == 0) return INFINITY;
  if(!finite(x)) return x;
  /* Lanczos method */
  denom = x+1;
  x1 = x + 5.5;
  series = 1.000000000190015;
  for(i = 0; i < 6; i++) {
    series += gamma_series[i] / denom;
    denom += 1.0;
  }
  return( M_lnSqrt2PI + (x+0.5)*log(x1) - x1 + log(series/x) );
}

// x/2 is actual gammaln parameter

double urand()
{
	return rand() / ((double) (RAND_MAX+1.0));
}




Vector gl_pc;
/* With 0.5 increments */
void precomputeGammaLn(int size) 
{
	int i;
	new (&gl_pc) Vector(size);
	for (i=0;i<gl_pc.n;i++)
		gl_pc[i] = gammaln(i/2.0);
}


double harmean(double x,double y)
{
	return 1.0/((1.0/x)+(1.0/y));
}