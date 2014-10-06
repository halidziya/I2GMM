#include "ThreadPool.h"

thread_local int thread_id;

ThreadPool::ThreadPool(int numthreads)
{
	int i;
	this->numthreads = numthreads;// thread::hardware_concurrency();
	stop = false;
	idle = 0;
	thread_id = 0;


	for(i=0;i<numthreads;i++)
		workers.push_back(thread(&ThreadPool::thread_loop,this,i+1)); // Threads starts from 1 , parent is 0
}


ThreadPool::~ThreadPool(void)
{
	std::unique_lock<std::mutex> lock(queue_mutex);
	stop = true;
	lock.unlock();

	condition.notify_all();

	for(unsigned int i=0;i<workers.size();i++)
	{
		workers[i].join();
		pri(workers[i].get_id());
	}
}

void ThreadPool::thread_loop(int id)
{
	// int completed = 0;	
	thread_id = id;
	srand(time(NULL)+thread_id);
	Task* task;
    while(true)
    {
		
        unique_lock<mutex>  lock(queue_mutex);
        while(!stop && tasks.empty())
        {
			idle++;
			if (idle == numthreads)
				empty.notify_one();
            condition.wait(lock); // Wait for a job
			idle--;
        }
 
        if(stop)
            return;
 
        task = tasks.front();
        tasks.pop_front();
		lock.unlock();
        task->run(thread_id);
		// completed++;
	}	
}


void ThreadPool::submit(Task& t)
{
	unique_lock<mutex>  lock(queue_mutex);
	tasks.push_back(&t);
	lock.unlock();
	condition.notify_one();
}

void ThreadPool::waitAll()
{
	
	unique_lock<mutex>  lock(queue_mutex);
	while (!tasks.empty() || idle != numthreads)
	{
		empty.wait(lock);
	}
	lock.unlock();
}