#pragma once
#include "Stut.h"
#include "Global.h"
#include <list>
#include <fstream>

using namespace std;

class Table;

class Dish 
{
public:
	Stut dist;
	Dish(Stut t);
	Dish(int d); 
	Dish();

	int dishid;
	int ntables;
	int nsamples;  // Maybe it is not necessary
	double kap;
	double kapi;
	double logprob; // Used in sampling
	Matrix sampleScatter;
	Vector sampleMean;
	list<Dish>::iterator copy; // To facilate copy

	void addCluster(Table& cluster);
	void removeCluster(Table& cluster);
	void calculateDist();
	void reset();

	~Dish(void);

	friend ostream& operator<<(ostream& os, const Dish& d);
};

