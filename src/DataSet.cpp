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
		cout << kappa << " " << kappa1 << " " << alpha << " " << gamma;
	}
	else
	{
		m = d + 2; 
		kappa = 0.05;
		kappa1 = 0.5;
		alpha = 1;
		gamma = 1;
	}
	init_buffer(nthd, d);
	if (fexists(priorfile))
	{
		prior.readBin(priorfile);
		hypersample = 1; // 0 actual value would be changed in further versions
		Psi = prior;
		Psi.r = d; // Last row is shadowed
		Psi.n = d*d;
		mu0 = prior(d).copy();
	}
	else
	{
		Psi = eye(d)*(m - d - 1);
		mu0 = data.mean();
	}

}


DataSet::~DataSet(void)
{
}


