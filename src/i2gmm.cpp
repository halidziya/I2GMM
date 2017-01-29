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


// Default Label Sampling values
int MAX_SWEEP=1500;
int BURNIN=1000;
int SAMPLE=20;
int STEP  = (MAX_SWEEP - BURNIN)/SAMPLE;
char* result_dir = "./";
double kep;


class TotalLikelihood : public Task
{
public:
	atomic<int> taskid;
	int ntable;
	int nchunks;
	vector<Customer>& customers;
	atomic<double> totalsum;
	TotalLikelihood(vector<Customer>& customers,int nchunks) : nchunks(nchunks), customers(customers){
	}
	void reset() {
		totalsum = 0;
		taskid = 0;
	}
	void run(int id) {
		// Use thread specific buffer
		SETUP_ID()
		int taskid = this->taskid++; // Oth thread is the main process
		auto range = trange(n, nchunks, taskid); // 2xNumber of Threads chunks			
		double logsum = 0;
		for (auto i = range[0]; i< range[1]; i++) // Operates on its own chunk
		{
			logsum += customers[i].table->dist.likelihood(customers[i].data);
		}
		totalsum = totalsum + logsum;
	}
};


void updateTableDish(list<Dish>& franchise,vector<Restaurant>& Restaurants)
{
	for (auto dit = franchise.begin(); dit != franchise.end(); dit++)
		dit->reset();
	for (int i = 0; i < Restaurants.size(); i++) // Update stats
	{
		//Each table
		for (auto tit = Restaurants[i].tables.begin(); tit != Restaurants[i].tables.end(); tit++)
		{
			tit->dishp->addCluster(*tit);
		}
	}

	for (auto dit = franchise.begin(); dit != franchise.end(); dit++)
		dit->calculateDist();
	for (int i = 0; i < Restaurants.size(); i++)
	{
		// Each table
		for (auto tit = Restaurants[i].tables.begin(); tit != Restaurants[i].tables.end(); tit++)
		{
			tit->calculateDist();
		}
	}
}

bool compare_clusters(Dish& c1, Dish& c2)
{
	return c1.nsamples > c2.nsamples;
}


void reid(list<Dish>& clusters)
{
	clusters.sort(compare_clusters);
	int i = 0;
	for (auto& acul : clusters)
		acul.dishid = i++;
}

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
	else
	{
		string str(datafile);
		result_dir = (char*) str.substr(0,str.find_last_of("/\\")).c_str();
	}
	if (argc > 7)
	{
		SAMPLE = atoi(argv[7]);
	}
	STEP = (MAX_SWEEP - BURNIN) / SAMPLE;
	if (BURNIN >= MAX_SWEEP | STEP == 0 ) // Housekeeping
	{
		BURNIN = MAX_SWEEP - 2;
		SAMPLE = 1; STEP = 1;
	}
	
	step();
	// Computing buffer

	printf("Reading...\n");
	nthd = thread::hardware_concurrency();
	DataSet ds(argc,argv);

	kep = kappa*kappa1/(kappa + kappa1);
	
	precomputegamLn(2*(n+d)+1+200*d);  // With 0.5 increments
	Vector priormean(d); 
	Matrix priorvariance(d,d);
	priorvariance = Psi*((kep+1)/((kep)*(m-d+2)));
	priormean = mu0;
	Vector originalmean(d);
	originalmean  = mu0;
	
	Stut stt(priormean,priorvariance,m-d+2); 
	Vector loglik0;
	
	Dish emptyDish(stt);
	vector<Restaurant> Restaurants;
	vector<Restaurant> beststate;
	vector<vector<Restaurant>::iterator> Restaurantit; // Hash table
	list<Dish> franchise;
	list<Dish> bestdishes;
	Matrix     sampledLabels(SAMPLE,n);
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
	Restaurants.reserve(20);
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
	ThreadPool tpool(nthd);
	// ==================



	list<Dish>::iterator dit,ditc;
	list<Table>::iterator tit;
	vector<Customer>::iterator cit;
	list<vector<Customer>::iterator> intable;
	list<vector<Customer>::iterator>::iterator points;

	double newdishprob,maxdishprob,sumprob,val,logprob,gibbs_score,best_score;
	int kal=1,id;
	gibbs_score = -INFINITY;
	best_score = -INFINITY;

	Vector score(MAX_SWEEP+1);
	vector<Restaurant> dishrestaurants;
	TotalLikelihood tl(allcust, nthd);
	bool hypersample = false;
	for (int num_sweep = 0;num_sweep < MAX_SWEEP ; num_sweep++)
	{


		if (false)
		{
			// Update tables
			for (i = 0; i < Restaurants.size(); i++)
			{
				for (auto& customer : Restaurants[i].customers)
				{
					allcust[customer.id - 1].table = customer.table;
				}
			}

			Vector logprob(20);
			int idx;
			k = 0;
			double s = kappa1 / kappa;
			for (kappa = 0.025; kappa < 2; kappa += 0.1)
			{
				kappa1 = s * kappa;
				updateTableDish(franchise, Restaurants);
				tl.reset();
				for (i = 0; i<tl.nchunks; i++)
					tpool.submit(tl);
				tpool.waitAll(); // Wait for finishing all jobs
				logprob[k] = tl.totalsum / n;

				k++;
				//cout << logprob << endl;
			}
			idx = sampleFromLog(logprob);
			kappa = idx*0.1 + 0.025;
			kappa1 = s * kappa;
			//cout << kappa << endl;
			updateTableDish(franchise, Restaurants);

			k = 0;
			for (kappa1 = 1*kappa; kappa1 < (6-EPS)*kappa; kappa1 += 0.25*kappa)
			{
				updateTableDish(franchise, Restaurants);
				tl.reset();
				for (i = 0; i<tl.nchunks; i++)
					tpool.submit(tl);
				tpool.waitAll(); // Wait for finishing all jobs
								 //cout << tl.totalsum << ":";
				logprob[k] = tl.totalsum / n;

				k++;
				//cout << logprob << endl;
			}
			//logprob.print();
			idx = sampleFromLog(logprob);
			kappa1 = 0.25*kappa*idx + 1*kappa;
			updateTableDish(franchise, Restaurants);
			//cout << kappa1 << endl;

			for (int dim = 0; dim < d; dim++) {
				k = 0;
				for (double ps = 0.05; ps < (2.05-EPS); ps += 0.1)
				{
					Psi.data[dim*d + dim] = ps*(m - d - 1);
					updateTableDish(franchise, Restaurants);
					tl.reset();
					for (i = 0; i<tl.nchunks; i++)
						tpool.submit(tl);
					tpool.waitAll(); // Wait for finishing all jobs
					logprob[k] = tl.totalsum / n;
					k++;
				}
				idx = sampleFromLog(logprob);
				Psi.data[dim*d + dim] = (0.05 + (idx)*0.1) *(m - d - 1);
				updateTableDish(franchise, Restaurants);
			}

			k = 0;
			Matrix psi = (Psi / (m - d - 1)).copy();
			//cout << m << endl ;
			for (m = d + 2; m < (d + 2 + 100 * d - EPS); m += 5 * d)
			{
				Psi = psi*(m - d - 1);
				updateTableDish(franchise, Restaurants);
				tl.reset();
				for (i = 0; i < tl.nchunks; i++)
					tpool.submit(tl);
				tpool.waitAll(); // Wait for finishing all jobs
				logprob[k] = tl.totalsum / n;
				k++;
				//cout << logprob << endl;
			}
			//logprob.print();
			idx = sampleFromLog(logprob);
			m = d + 2 + (idx * 5 * d);
			//cout << m << endl;
			Psi = psi*(m - d - 1);



			//for (gam = 0.2; gam < 1.2; gam += 0.05)
			//{
			//	double at = 0;
			//	for (auto& f : franchise)
			//	{
			//		at += gamln(f.nsamples);
			//	}
			//	logprob[k] = log(gam)*franchise.size() + at - gamln(n + gam) + gamln(gam); // Partition likelihood
			//}
			//gam = sampleFromLog(logprob)*0.05 + 0.2;


			//for (alpha = 0.2; alpha < 1.2; alpha += 0.05)
			//{
			//	double at = 0;
			//	for (auto& r : Restaurants) // Summation of lower layer CRP partition likelihoods
			//	{
			//		at += log(alpha)*r.tables.size();
			//		for (auto t: r.tables)
			//			at += gamln(t.npoints);
			//		at += gamln(r.customers.size() + alpha) - gamln(alpha);
			//	}
			//	logprob[k] = at;
			//}
			//alpha = sampleFromLog(logprob)*0.05 + 0.2;



			
			////mu0.print();
			//logprob.resize(5);
			////priormean = mu0;
			//for (int dim = 0; dim < d; dim++) {
			//	k = 0;
			//	for (double add = -0.1; add < 0.1; add += 0.04)
			//	{
			//		mu0.data[dim] = originalmean.data[dim] + add;
			//		updateTableDish(franchise, Restaurants);
			//		tl.reset();
			//		for (i = 0; i<tl.nchunks; i++)
			//			tpool.submit(tl);
			//		tpool.waitAll(); // Wait for finishing all jobs
			//		logprob[k] = tl.totalsum / n;
			//		k++;
			//	}
			//	idx = sampleFromLog(logprob);
			//	mu0.data[dim] = originalmean.data[dim] + idx*0.04 - 0.1;
			//	updateTableDish(franchise, Restaurants);
			//}
			////mu0.print();
			////(Psi/(m-d-1)).print();
			//priormean = mu0;



			kep = kappa*kappa1 / (kappa + kappa1);
			priorvariance = Psi*((kep + 1) / ((kep)*(m - d + 2)));
			loglik0 = Stut(priormean, priorvariance, m - d + 2).likelihood(ds.data);

		}







		//Create restaurants for each dish
		reid(franchise);
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
		Restaurants.resize(dishrestaurants.size());
		for (i=0;i<Restaurants.size();i++)
			Restaurants[i].copyfrom(dishrestaurants[i]);
		//Restaurants.copy(dishrestaurants);
		//random_shuffle(Restaurants.begin(),Restaurants.end());

		if (num_sweep%5==0)
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

				newdishprob = tit->npoints * log(gam);//+ log(3/(franchise.size() + 1.0 )); // A possion factor added centered on 3 clusters
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

		// 4th loop 
		for (i = 0; i < Restaurants.size(); i++)
		{
			// Each table
			for (tit = Restaurants[i].tables.begin(); tit != Restaurants[i].tables.end(); tit++)
			{
				tit->calculateDist();
			}
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


		if  (((MAX_SWEEP-num_sweep - 1)%STEP)==0 && num_sweep >= BURNIN) // Later samples are better pick from last one
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

			int sampleno = (MAX_SWEEP - num_sweep - 1) / STEP;
			if (sampleno < SAMPLE)
				try {
				for (i = 0; i < n; i++)
				{
					sampledLabels(sampleno)[allcust[i].id - 1] = allcust[i].table->dishp->dishid;
				}
			}
			catch (exception& error)
			{
				cout << "Error in writing labels : " << error.what();
				pause();
			}
		}
		
		flush(cout);
		// 2nd Loop
	}
	step();


	franchise = bestdishes;
	for (dit=franchise.begin(),ditc=bestdishes.begin();dit!=franchise.end();dit++,ditc++)
				ditc->copy = dit;
	Restaurants = beststate;
	


	try {
		string s(result_dir);
		ofstream dishfile(s.append("Dish.dish"), ios::out | ios::binary); // Be careful result_dir should include '\'
		int ndish = franchise.size();
		int nrest = Restaurants.size();
		dishfile.write((char*)& ndish, sizeof(int));
		for (i = 0, dit = franchise.begin(); dit != franchise.end(); i++, dit++)
		{
			dit->dishid = i + 1;
			dishfile << *dit;
		}
		dishfile.close();

		s.assign(result_dir);
		ofstream restfile(s.append("Restaurant.rest"), ios::out | ios::binary);
		restfile.write((char*)& nrest, sizeof(int));
		for (i = 0; i < nrest; i++)
		{
			restfile << Restaurants[i];
		}
		restfile.close();

		s.assign(result_dir);
		ofstream likefile(s.append("Likelihood.matrix"), ios::out | ios::binary);
		likefile << score;
		likefile.close();

		s.assign(result_dir);
		ofstream labelfile(s.append("Labels.matrix"), ios::out | ios::binary);
		labelfile << sampledLabels;
		labelfile.close();

		nsampleslog.close();
	}
	catch (exception e)
	{
		cout << "Error in writing files " << endl;
		return -1;
	}
}
