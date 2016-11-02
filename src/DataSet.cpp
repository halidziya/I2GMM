#include "DataSet.h"
#include <iostream>
#include <fstream>

using namespace std;

DataSet::DataSet(char* datafile,char* priorfile,char* configfile)
{
	Matrix conf;
	Matrix labels;
	data.readBin(datafile);
	prior.readBin(priorfile);
	conf.readBin(configfile);
	n = data.r;
	d =  conf(0)(0);
	m =  conf(0)[1];
	kappa  = conf(0)[2];
	kappa1 = conf(0)[3];
	alpha  = conf(0)[4];
	gamma  = conf(0)[5];

}


DataSet::~DataSet(void)
{
}


