#include "Table.h"


Table::Table(void)
{
	npoints = 0;
}


Table::Table(list<Dish>::iterator d)
{
	dishp = d;
	npoints = 0;
	sampleScatter = Matrix(d->dist.mu.n,d->dist.mu.n);
	sampleMean	  = Vector(d->dist.mu.n);
	sampleScatter.zero();
	sampleMean.zero();
	new (&dist) Stut(d->dist.mu.n);
}

Table::Table(int d)
{
	npoints = 0;
	sampleScatter = Matrix(d,d);
	sampleMean	  = Vector(d);
	sampleScatter.zero();
	sampleMean.zero();
	new (&dist) Stut(d);
}

Table::~Table(void)
{
}

void Table::addInitPoint(Vector& v)
{
	sampleMean = ((sampleMean * npoints ) + v ) / (npoints + 1.0); 
	sampleScatter  =  ( sampleScatter + (v>>v) ) ; 
	npoints++;
}

void Table::addPoint(Vector& v)
{
	npoints++;  // npoint is current number of points in the table
	Vector& diff = (v - sampleMean); // Get abstract of the output buffer
	sampleScatter  =  ( sampleScatter + (diff>>diff)*((npoints-1.0)/(npoints))) ; 
	sampleMean = ((sampleMean * (npoints-1) ) + v ) / (npoints); 
	calculateDist();
}


void Table::removePoint(Vector& v)
{
	if (npoints<=0)
		npoints = 0;


	if (npoints > 1)
	{
		Vector& diff = (v - sampleMean); // Get abstract of the output buffer
		sampleScatter  =  ( sampleScatter - (diff>>diff)*(npoints/(npoints-1.0))) ; 
		sampleMean = ((sampleMean * (npoints/(npoints - 1.0))) - v *(1.0/(npoints - 1.0)))  ; 
		npoints--;
		calculateDist();
	}
	else
	{
		sampleScatter.zero();
		sampleMean.zero();
		npoints--;
	}
	


	// Table updates
}

// Vector assignment allocates memory if does not have any or uses previous one 
// So this avoids extra allocations
Table::Table(const Table& t)
{
	npoints = t.npoints;
	dist = t.dist;
	sampleScatter = t.sampleScatter;
	sampleMean = t.sampleMean;
	dishp = t.dishp;
	tableid = t.tableid;
}

void Table::operator=(const Table& t)
{
	npoints = t.npoints;
	dist = t.dist;
	sampleScatter = t.sampleScatter;
	sampleMean = t.sampleMean;
	dishp = t.dishp;
}

void Table::calculateCov()
{
	sampleScatter = sampleScatter - ((sampleMean>>sampleMean)*npoints);
	// sampleCov = sampleScatter / (npoints - 1);
}


void Table::calculateDist()
{
	double s1=dishp->kap + npoints;
	dist.eta = m + 1 + dishp->nsamples + this->npoints - d - dishp->ntables ; 
	dist.mu = (sampleMean*(npoints/s1) + dishp->dist.mu * (dishp->kap/s1) ) ;
	Vector& diff = dishp->dist.mu - sampleMean;
	dist.cholsigma = 
		((Psi + dishp->sampleScatter + sampleScatter +
		((diff)>>(diff))
		*(npoints*dishp->kap /(s1)))
		*((s1+1)/(s1*dist.eta))).chol();
	dist.calculateNormalizer();
}

ostream& operator<<(ostream& os, const Table& t)
{

	// os should be binary file
	int dishid = t.dishp->dishid;
	os.write((char*) &t.tableid,sizeof(int)); 
	os.write((char*) &dishid,sizeof(int)); 
	os.write((char*) &t.npoints,sizeof(int)); 
	// os.write((char*) &t.logprob,sizeof(double)); 
	os.write((char*) &t.loglikelihood,sizeof(double)); 
	os << t.sampleScatter;
	os << t.sampleMean;
	os << t.dist;
	return os;
}
