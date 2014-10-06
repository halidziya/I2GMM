#pragma once
#include "Normal.h"
#include "util.h"
#include "DebugUtils.h"

class Stut : public Normal
{
public:
	double eta;		// Degree of Freedom
	double normalizer;
	double coef1;
	// mu
	// sigma

	Stut(Vector mean,Matrix cov,int dof);
	Stut();
	Stut(int d);
	~Stut(void);

	double calculateNormalizer();
	double likelihood(Vector& x);
	Vector likelihood(Matrix& mat);

	friend ostream& operator<<(ostream& os, const Stut& t);
};

