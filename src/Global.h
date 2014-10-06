#pragma once
#include "Matrix.h"
class Global
{
public:
	Global(void);
	~Global(void);
	static int d; // dimensions
	static int m;
	static int n;
	static int eta;
	static double kappa;
	static double kappai;
	static double alpha;
	static double gamma;
	static Matrix Psi;
	static Vector mu0;
};

