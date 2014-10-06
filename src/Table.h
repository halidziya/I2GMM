#pragma once
#include "Stut.h"
#include "Dish.h"
#include <list>
#include "Global.h"

using namespace std;

class Table : public Global
{
public:
	Stut dist;
	int npoints;
	double logprob; // Used in sampling
	double loglikelihood;
	int tableid;

	Matrix sampleScatter;
	Vector sampleMean;
	list<Table>::iterator copy; // To facilate copy


	Table(void);
	Table(int dim);
	Table(const Table& t);
	Table(list<Dish>::iterator d);
	~Table(void);

	list<Dish>::iterator dishp;
	void addPoint(Vector& v);
	void removePoint(Vector& v);
	void addInitPoint(Vector& v);

	void operator=(const Table& t);
	void calculateCov(); // Fill covariance sampleCov , again scatter actually
	void calculateDist();

	friend ostream& operator<<(ostream& os, const Table& t);
};

