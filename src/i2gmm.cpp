#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "DebugUtils.h"
#include "DataSet.h"
#include "util.h"
#include "Dish.h"
#include "Restaurant.h"
#include "Customer.h"
#include <thread>  
#include <iostream>
#include <algorithm>


using namespace std;


int MAX_SWEEP=500;
int BURNIN=100;
int SAMPLE=(MAX_SWEEP-BURNIN)/10; // Default value is 10 sample + 1 post burnin
char* result_dir = "./";

// Variables
double kep,eta;


PILL_DEBUG
int main(int argc,char** argv)
{
	debugMode(1);
	char* datafile = argv[1];
	char* priorfile = argv[2];
	char* configfile = argv[3];
	string ss(result_dir);
	ofstream nsampleslog(ss.append("nsamples.log"));
	srand(time(NULL));
	// Default values , 1000 , 100  , out
	if (argc>4)
		MAX_SWEEP = atoi(argv[4]);
	if (argc>5)
		BURNIN = atoi(argv[5]);
	if (argc > 6)
		result_dir = argv[6];
	SAMPLE = (MAX_SWEEP-BURNIN)/10; // Default value
	if (argc>7)
		SAMPLE = atoi(argv[7]);
	
	step();
	// Computing buffer

	printf("Reading...\n");
	DataSet ds(datafile,priorfile,configfile);
	kep = kappa*kappa1/(kappa+ kappa1);
	eta = m - d + 2;

	precomputeGammaLn(2*(n+d)+1);  // With 0.5 increments
	init_buffer(thread::hardware_concurrency(),d);	
	
	Vector priormean(d); 
	
	Matrix priorvariance(d,d);
	
	Psi = ds.prior;
	Psi.r = d; // Last row is shadowed
	Psi.n = d*d;
	mu0 =  ds.prior(d).copy();
	
	eta = eta;
	priorvariance = Psi*((kep+1)/((kep)*eta));
	priormean = mu0;
	
	Stut stt(priormean,priorvariance,eta); 
	Vector loglik0;
	
	Dish emptyDish(stt);
	vector<Restaurant> Restaurants;
	vector<Restaurant> beststate;
	vector<vector<Restaurant>::iterator> Restaurantit; // Hash table
	list<Dish> franchise;
	list<Dish> bestdishes;
	Matrix     sampledLabels((MAX_SWEEP-BURNIN)/SAMPLE + 1,n);
	Table besttable;
	Customer bestcustomer;
	int i,j,k;
	
	// INITIALIZATION
	printf("Number of Cores : %d\n",thread::hardware_concurrency());

	step();
	loglik0		= emptyDish.dist.likelihood(ds.data);
	// customers.reserve(ds.n);
	step();
	
	// One cluster for each object initially
	franchise.push_back(Dish(d));
	list<Dish>::iterator firstDish = franchise.begin();
	i=0;
	Restaurants.reserve(200);
	Restaurants.resize(1);
	Restaurants[i].Restaurantid = 0;
	Table t(firstDish); // First dish
	Restaurants[i].addTable(t); 
	
	// Create customers
	vector<Customer> allcust;
	for(i=0;i<n;i++)
	{
		Restaurants[0].tables.begin()->addInitPoint(ds.data(i));
		Restaurants[0].customers.emplace_back(ds.data(i),loglik0[i],Restaurants[0].tables.begin(),i+1);
	}
	allcust = Restaurants[0].customers;
	i=0;
	Restaurants[i].tables.front().calculateCov();
	firstDish->addCluster(Restaurants[i].tables.front());

	step();
	firstDish->calculateDist();
	Restaurants[i].tables.front().calculateDist();

	//END INITIALIZATION

	// GIBBS SAMPLER


	// NUMBER OF THREADS
	// =================
	ThreadPool tpool(thread::hardware_concurrency());
	// ==================



	list<Dish>::iterator dit,ditc;
	list<Table>::iterator tit;
	vector<Customer>::iterator cit;
	list<vector<Customer>::iterator> intable;
	list<vector<Customer>::iterator>::iterator points;

	double newdishprob,maxdishprob,sumprob,val,logprob,gibbs_score,best_score;
	int kal=1,id;
	gibbs_score = -INFINITE;
	best_score = -INFINITE;

	Vector score(MAX_SWEEP+1);
	
	vector<Restaurant> dishrestaurants;
	for (int num_sweep = 0;num_sweep <= MAX_SWEEP ; num_sweep++)
	{


		//Create restaurants for each dish
		dishrestaurants.resize(0); // Remove all
		dishrestaurants.resize(franchise.size());
		for (i=0,dit=franchise.begin();dit!=franchise.end();dit++,i++)
		{
			id = dit->dishid = i;
			dishrestaurants[id].Restaurantid = id;
			dishrestaurants[id].customers.reserve(dit->nsamples);
			dishrestaurants[id].likelihood=0;
		}

		// Copy tables
		for (i=0;i<Restaurants.size();i++) // Old Restaurants
			for (auto& table : Restaurants[i].tables)
			{
				id = table.dishp->dishid;
				dishrestaurants[id].tables.push_back(table);
				table.copy = (--dishrestaurants[id].tables.end()); //Last item copied
			}

		// Add customers
		for (i=0;i<Restaurants.size();i++)
		{
			for (auto& customer : Restaurants[i].customers)
			{
				allcust[ customer.id - 1].table = customer.table;
			}

		}

		for (i=0;i<n;i++)
		{
			id = allcust[i].table->dishp->dishid;
			allcust[i].table = allcust[i].table->copy;
			dishrestaurants[id].customers.push_back(allcust[i]);
		}

		// Submit Jobs
		// 1st Loop
		//

		for (dit=franchise.begin();dit!=franchise.end();dit++)
		{
			dit->copy = dit;
		}
		Restaurants.resize(0);
		Restaurants = dishrestaurants;
		random_shuffle(Restaurants.begin(),Restaurants.end());

		printf("Iter %d nDish %d nRests %d Score %.1f\n",num_sweep,franchise.size(),Restaurants.size(),gibbs_score);	
			
		for (i=0;i<Restaurants.size();i++)
			tpool.submit(Restaurants[i]);	
		tpool.waitAll(); // Wait for finishing all jobs


		for(dit=franchise.begin();dit!=franchise.end();dit++) // For each dish
		{
			dit->reset(); // To start recalculation
		}

		for (i=0;i<Restaurants.size();i++)
		{
			for(tit=Restaurants[i].tables.begin();tit!=Restaurants[i].tables.end();tit++)
			{
				tit->dishp->addCluster(*tit);
			}
		}

		for(dit=franchise.begin();dit!=franchise.end();dit++) // For each dish
		{
			dit->calculateDist();
		}
		

		// 3rd Loop
		for(dit=franchise.begin();dit!=franchise.end();dit++)
		{
			dit->logprob = 0;
			nsampleslog << dit->nsamples << " ";
		}
		nsampleslog << endl;

		for (i=0,kal=1;i<Restaurants.size();i++)
		{
			// Each table
			for(tit=Restaurants[i].tables.begin();tit!=Restaurants[i].tables.end();tit++,kal++)
			{
				tit->dishp->removeCluster(*tit);
				
				if (tit->dishp->ntables==0) // Remove dish 
					franchise.erase(tit->dishp);
				else
					tit->dishp->calculateDist();
				if (intable.size()>0)
				{
					int sss = intable.size();
					intable.clear();
				}
				

				// Create list of customers
				for(cit=Restaurants[i].customers.begin();cit!=Restaurants[i].customers.end();cit++)
				{
					if (cit->table == tit)
						intable.push_back(cit);
				}

				newdishprob = tit->npoints * log(gamma);
				for(points=intable.begin();points!=intable.end();points++)
					newdishprob += (*points)->loglik0;

				maxdishprob = newdishprob;

				for(dit=franchise.begin();dit!=franchise.end();dit++) 
				{
					logprob=0;
					for(points=intable.begin();points!=intable.end();points++)
					{
						// Here is computationally time consuming Under 4 for loops matrix division !!!!!!
						logprob+=dit->dist.likelihood((*points)->data);
					}
					dit->logprob = logprob + tit->npoints * log(dit->ntables); //Prior


					if (maxdishprob<dit->logprob)
						maxdishprob = dit->logprob;
				}

				sumprob=0;
				for(dit=franchise.begin();dit!=franchise.end();dit++) 
				{
					dit->logprob = exp(dit->logprob - maxdishprob);
					sumprob += dit->logprob;
				}

				sumprob += exp(newdishprob - maxdishprob);

				double rrr = urand();
				val = rrr*sumprob;

				for(dit=franchise.begin();dit!=franchise.end();dit++) 
				{
					if ((dit->logprob)>=val)
						break;
					else
						val -= dit->logprob;
				}

				if (dit==franchise.end()) // Create new dish
				{
					franchise.emplace_back(d);
					dit = franchise.end();
					dit--; // Point to actual dish
				}

				tit->dishp = dit;
				dit->addCluster(*tit);
				dit->calculateDist();
			}
		}

		for (kappa = 0.5; kappa < 2; kappa += 0.5)
		{
			// 4th loop 
			for (i = 0; i < Restaurants.size(); i++)
			{
				// Each table
				for (tit = Restaurants[i].tables.begin(); tit != Restaurants[i].tables.end(); tit++)
				{
					tit->calculateDist();
				}
			}

			logprob = 0;
			for (dit = franchise.begin(); dit != franchise.end(); dit++)
			{
				for (points = intable.begin(); points != intable.end(); points++)
				{
					// Here is computationally time consuming Under 4 for loops matrix division !!!!!!
					logprob += dit->dist.likelihood((*points)->data);
				}
				//logprob = logprob + tit->npoints * log(dit->ntables); //Prior
			}
			cout << logprob << endl;
		}
		
		// Calculate Gibbs Score
		gibbs_score = 0;
		for (i=0;i<Restaurants.size();i++)
			gibbs_score += Restaurants[i].likelihood;
		score[num_sweep] = gibbs_score;
		if ( (best_score < gibbs_score && num_sweep >= BURNIN) || num_sweep == 0)
		{
			best_score = gibbs_score ;
			bestdishes = franchise;
			for (dit=franchise.begin(),ditc=bestdishes.begin();dit!=franchise.end();dit++,ditc++)
			{
				dit->copy = ditc ;
			}
			beststate = Restaurants;
		}

		if  (((num_sweep-BURNIN)%SAMPLE)==0 && num_sweep >= BURNIN)
		{
			for (dit=franchise.begin(),i=0;dit!=franchise.end();dit++,i++)
				dit->dishid = i;
			int error=0;
			
			// Update tables
			for (i=0;i<Restaurants.size();i++)
			{
			for (auto& customer : Restaurants[i].customers)
			{
				allcust[ customer.id - 1].table = customer.table;
			}
			}


			for(i=0;i<n;i++)
			{
				sampledLabels((num_sweep-BURNIN)/SAMPLE)[allcust[i].id-1] = allcust[i].table->dishp->dishid;
				if (allcust[i].table->dishp->dishid<0 || allcust[i].table->dishp->dishid>franchise.size())
				{
					error = 1;
					printf("Something is wrong in C code : dishid %d table %d customer %d\n",allcust[i].table->dishp->dishid,allcust[i].table->tableid,allcust[i].id-1);
				}
			}
			if (error)
				pause();
		}
		
		flush(cout);
		// 2nd Loop
	}
	step();


	franchise = bestdishes;
	for (dit=franchise.begin(),ditc=bestdishes.begin();dit!=franchise.end();dit++,ditc++)
				ditc->copy = dit;
	Restaurants = beststate;
	

	string s(result_dir);
	ofstream dishfile( s.append("Dish.dish"),ios::out | ios::binary); // Be careful result_dir should include '\'
	s.assign(result_dir);
	ofstream restfile( s.append("Restaurant.rest"),ios::out | ios::binary);
	s.assign(result_dir);
	ofstream likefile( s.append("Likelihood.matrix"),ios::out | ios::binary);
	
	s.assign(result_dir);
	ofstream labelfile( s.append("Labels.matrix"),ios::out | ios::binary);

	int ndish = franchise.size();
	int nrest = Restaurants.size();
	dishfile.write((char*)& ndish,sizeof(int));
	restfile.write((char*)& nrest,sizeof(int));

	for(i=0,dit=franchise.begin();dit!=franchise.end();i++,dit++) 
	{
		dit->dishid = i+1;
		dishfile <<  *dit;
	}
	dishfile.close();


	for (i=0;i<nrest;i++)
	{
		restfile << Restaurants[i];
	}

	restfile.close();


	likefile << score;
	likefile.close();

	labelfile << sampledLabels;
	labelfile.close();

	nsampleslog.close();
}
