#pragma once
#include <deque>
#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <condition_variable>
#include "DebugUtils.h"
#define thread_local  __declspec( thread )

using namespace std;

class Task{
public:
	virtual void run(int id)=0;
};

thread_local extern int thread_id;

class ThreadPool
{
public:
	deque<Task*> tasks; // Maybe there could be better way to implement this instead of pointers
	vector<thread> workers;
	mutex queue_mutex;
	condition_variable empty;
	int idle;
	bool stop;
	void submit(Task& t);
	int numthreads;
	condition_variable condition;
	ThreadPool(int numthreads);
	~ThreadPool(void);
	void thread_loop(int id);
	void waitAll();
};

