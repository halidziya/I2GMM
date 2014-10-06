#pragma once
#include "Matrix.h"
#include "Global.h"
#include "Stut.h"

class DataSet : public Global
{
public:
	DataSet(char* datafile,char* priorfile,char* configfile);
	Matrix data;
	Matrix prior;
	~DataSet(void);
};

