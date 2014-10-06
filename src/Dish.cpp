#include "Dish.h"
#include "Table.h"
#include "util.h"

Dish::Dish(Stut t)
{
	dist = t;
	sampleScatter = Matrix(t.mu.n,t.mu.n);
	sampleMean = Vector(t.mu.n);
	sampleScatter.zero();
	sampleMean.zero();
	kap = 0;
	kapi=0;
	nsamples =0;
	ntables = 0;
}

Dish::Dish()
{
	nsamples = 0;
	ntables = 0;
	kapi=0;
}




Dish::Dish(int d)
{
	dist = Stut(d);
	sampleScatter = Matrix(d,d);
	sampleMean = Vector(d);
	sampleScatter.zero();
	sampleMean.zero();
	kap = 0;
	kapi=0;
	nsamples = 0;
	ntables = 0;
}

void Dish::reset()
{
	sampleMean.zero();
	sampleScatter.zero();
	nsamples = 0;
	ntables = 0;
	kapi=0;
}


Dish::~Dish(void)
{
}


void Dish::addCluster(Table& cluster)
{
	double coef1 = cluster.npoints*kappai / (cluster.npoints+kappai);
	sampleMean = (sampleMean * (kapi/(kapi+coef1)) + (cluster.sampleMean*(coef1/(kapi+coef1))));
	kapi = kapi + coef1;
	sampleScatter = sampleScatter + cluster.sampleScatter ; 
	this->ntables++;
	this->nsamples += cluster.npoints;	
}


void Dish::removeCluster(Table& cluster)
{
	
	double coef1 = cluster.npoints*kappai / (cluster.npoints+kappai);
 	sampleScatter = sampleScatter - cluster.sampleScatter ; 
	kapi = kapi - coef1;
	sampleMean = (sampleMean * ((kapi+coef1)/(kapi)) - (cluster.sampleMean*((coef1)/kapi)));
 	this->ntables--;
 	this->nsamples -= cluster.npoints;	
}


void Dish::calculateDist()
{
	kap = harmean(kapi+kappa,kappai);
	dist.eta = m + 2 + nsamples - d - ntables ; //dpar0
	dist.mu =( sampleMean*kapi + mu0*kappa )/(kapi + kappa); // dpar1
	dist.cholsigma = (( Psi + sampleScatter)*((kap + 1)/(kap*dist.eta))).chol(); // dpar2
	// mu_ti is deleted because of it is same with dpar0
	dist.calculateNormalizer();
}

ostream& operator<<(ostream& os, const Dish& d)
{
	// os should be binary file
	os.write((char*) &d.dishid,sizeof(int)); 
	os.write((char*) &d.ntables,sizeof(int)); 
	os.write((char*) &d.nsamples,sizeof(int)); 
	os.write((char*) &d.kap,sizeof(double)); 
	os.write((char*) &d.kapi,sizeof(double)); 
	os.write((char*) &d.logprob,sizeof(double)); 
	os << d.sampleScatter;
	os << d.sampleMean;
	os << d.dist;
	return os;
}
