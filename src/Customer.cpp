#include "Customer.h"

Customer::Customer(Vector& data,double loglik) : data(data) , loglik0(loglik)
{
}

Customer::Customer(Vector& data,double loglik,list<Table>::iterator table,int id) : data(data) , loglik0(loglik) , table(table),id(id)
{
}

Customer::Customer()
{
}

Customer::~Customer(void)
{
		
}


void Customer::operator=(Customer c) 
{
	loglik0=c.loglik0;
	table=c.table;
	id=c.id;
	new (&data) Vector(c.data); // Do not copy actual vector 
}

ostream& operator<<(ostream& os, const Customer& c)
{
	os.write((char*) &c.id,sizeof(int));
	os.write((char*) &c.loglik0,sizeof(double));
	os.write((char*) &c.table->tableid,sizeof(int));
	os << c.data;
	return os;
}
