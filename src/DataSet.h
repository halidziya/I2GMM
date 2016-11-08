#pragma once
#include "Matrix.h"
#include "Global.h"
#include "Stut.h"

extern int hypersample;
class DataSet 
{
public:
	DataSet(char* datafile,char* priorfile,char* configfile);
	Matrix data;
	Matrix prior;
	~DataSet(void);
};

