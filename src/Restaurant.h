#pragma once
#include <list>
#include "Table.h"
#include "Customer.h"
#include "ThreadPool.h"

using namespace std;

class Restaurant : public virtual Task// Collection of clusters and data points
{
public:

	int Restaurantid;
	double likelihood;


	list<Table> tables;
	vector<Customer> customers;


	void addTable(Table& t);
	Restaurant(void);
	
	~Restaurant(void);
	void run(int id);

	void operator=(Restaurant r);
	Restaurant(const Restaurant& r); // Copy constructor
	
	
	friend ostream& operator<<(ostream& os, Restaurant& v);
};

