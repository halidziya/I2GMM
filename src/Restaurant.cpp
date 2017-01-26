#include "Restaurant.h"
#include "Table.h"

Restaurant::Restaurant(void)
{
}


void Restaurant::operator=(Restaurant r)
{
	tables = r.tables;
	customers = r.customers;
	Restaurantid = r.Restaurantid;
	list<Table>::iterator it,tit;
	vector<Customer>::iterator cit,rit;
	int i;
	for(it=tables.begin(),tit=r.tables.begin();tit!=r.tables.end();tit++,it++)
	{
		tit->copy = it;
		it->dishp = tit->dishp->copy;
	}
	for(cit=customers.begin(),rit=r.customers.begin();cit!=customers.end();cit++,rit++)
	{
		cit->table = rit->table->copy; // Point to copied object
	}

}


Restaurant::Restaurant(Restaurant&  r)
{
	tables = r.tables;
	customers = r.customers;
	Restaurantid = r.Restaurantid;
	list<Table>::iterator it, tit;
	vector<Customer>::iterator cit, rit;
	int i;
	for (it = tables.begin(), tit = r.tables.begin(); tit != r.tables.end(); tit++, it++)
	{
		tit->copy = it;
		it->dishp = tit->dishp->copy;
	}
	for (cit = customers.begin(), rit = r.customers.begin(); cit != customers.end(); cit++, rit++)
	{
		cit->table = rit->table->copy; // Point to copied object
	}
}

Restaurant::~Restaurant(void)
{
}


void Restaurant::addTable(Table& t)
{
	tables.push_back(t);
}

void Restaurant::run(int id)
{
	// Use thread specific buffer
	threadid = id;

	int i,n,ti,npts;
	double sum,val,max,subrest,newclust; // Likelihood sum for normalization
	n = customers.size();
	list<Table>::iterator tit,oldtable; // Table iterator
	list<Dish>::iterator ki;
	Table copy;
	likelihood=0;
	int removed = 0;
	for (i=0;i< n;i++)
	{
		Vector& x = customers[i].data;
		oldtable = customers[i].table;
		copy = *oldtable;
		ki = customers[i].table->dishp;
		removed = 0;
		customers[i].table->removePoint(x);
		// printf("%d\n",customers[i].table->npoints);
		sum = 0;
		
		
		npts = customers[i].table->npoints;

		if (customers[i].table->npoints == 0)
		{
			removed = 1;
			tables.erase(customers[i].table);
		}
		/*for(tit=tables.begin();tit!=tables.end();tit++)
			if (ki == tit->dishp)
				subrest+= tit->npoints;*/
		
		newclust = customers[i].loglik0 + log(alpha); ///(subrest+alpha)
		max = newclust;

		for(tit=tables.begin();tit!=tables.end();tit++)
		{

				tit->loglikelihood = tit->dist.likelihood(x);
				tit->logprob = tit->loglikelihood  + log(tit->npoints); ///(subrest+alpha)

			if (tit->logprob > max)
				max = tit->logprob;
		}


		for(tit=tables.begin();tit!=tables.end();tit++)
		{
				tit->logprob = exp(tit->logprob-max); // No longer in logarithm actually
				sum += tit->logprob;
		}

		sum += exp(newclust - max); // New class


		val = urand()*sum;
		for(tit=tables.begin();tit!=tables.end();tit++)
		{
			if ( (tit->logprob) >= val )
			{
				break; // Find it
			}
			val -= (tit->logprob);
		}

		if (tit==tables.end()) // Not in current tables add new one
		{
			tables.emplace_front(ki); // New empty table
			tit = tables.begin();
			tit->loglikelihood = customers[i].loglik0;
		}

		customers[i].table = tit;

		likelihood += tit->loglikelihood;
		if (!removed && tit==oldtable)
			*tit = copy;
		else
			tit->addPoint(x); // Add point to selected one

	}
}


ostream& operator<<(ostream& os, Restaurant& r)
{
	os.write((char*) &r.Restaurantid,sizeof(int));
	os.write((char*) &r.likelihood,sizeof(double));
	int ncustomers = r.customers.size();
	int i;

	int ntables = r.tables.size();
	os.write((char*) &ntables,sizeof(int));
	list<Table>::iterator tit;
	for(i=0,tit=r.tables.begin();tit!=r.tables.end();tit++,i++)
	{
		tit->tableid = i+1;
		os << *tit;
	}

	os.write((char*) &ncustomers,sizeof(int));
	for (i=0;i<ncustomers;i++)
	{
	/*	if (r.customers[i].table->tableid>10)
		{
			printf("Tables\n");
			for(tit=r.tables.begin();tit!=r.tables.end();tit++)
			{
				printf("%p %d %d\n",tit._Ptr,tit->tableid,tit->npoints);
			}
			printf("Customer\n");
			printf("%p\n",r.customers[i].table._Ptr);
			printf("%d\n",r.customers[i].table->tableid);
			printf("%d\n",r.customers[i].table->npoints);
		}*/
		os << r.customers[i];
	}


	return os;
}
