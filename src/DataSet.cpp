#include "DataSet.h"
#include <iostream>
#include <fstream>

using namespace std;
int hypersample = 1;
DataSet::DataSet(char* datafile,char* priorfile,char* configfile)
{
	Matrix conf;
	Matrix labels;
	data.readBin(datafile);
	n = data.r;
	d = data.m;

	if (fexists(priorfile))
	{
		conf.readBin(configfile);
		m = conf(0)[1];
		kappa = conf(0)[2];
		kappa1 = conf(0)[3];
		alpha = conf(0)[4];
		gamma = conf(0)[5];
	}
	else
	{
		m = d + 2;
		kappa = 1;
		kappa1 = 1;
		alpha = 1;
		gamma = 1;
	}

	if (fexists(priorfile))
	{
		prior.readBin(priorfile);
		hypersample = 0;
		Psi = prior;
		Psi.r = d; // Last row is shadowed
		Psi.n = d*d;
		mu0 = prior(d).copy();
	}
	else
	{
		init_buffer(nthd, d);
		Psi = eye(d)*(m - d - 1);
		mu0 = data.mean();
	}

}


DataSet::~DataSet(void)
{
}


