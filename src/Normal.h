#pragma once
#include "Matrix.h"

class Normal
{
public:
	Vector mu;
	Matrix cholsigma;
	int d;
	Normal(void);
	Normal(int d); // dimensions
	~Normal(void);
	double likelihood();
};

