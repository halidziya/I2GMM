#include "Normal.h"


Normal::Normal(void)
{
}


Normal::~Normal(void)
{
}


Normal::Normal(int d)
{
	mu = Vector(d);  
	cholsigma = Matrix(d,d);
	this->d = d;
}

double Normal::likelihood()
{
	// Not yet implemented

	return 0;
}