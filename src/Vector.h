#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ostream>
#include "DebugUtils.h"
#include <thread>
#include "Buffer.h"
#define EPS 1e-10 // Floating point equality

class Matrix;
using namespace std;


/*	Vector Types :
	TYPE 0 : Abstract	vector , just a pointer
	TYPE 1 : Standart	vector , allocates deallocates memory automatically
	TYPE 2 : Buffer		vector , calculations are used on top.

*/

class Vector{
public:
	double* data;
	int n;
	int type;
	Vector(int size,int real=1);
	~Vector(void);
	
	Vector (const Vector& v);
	Vector (Vector&& v);
	Vector();
	

	void print();
	void zero();
	void resize(int size);
	Vector copy();		// Returns real vector
	Vector unique();	// Returns unique values 


	inline double& operator[](const int i);
	int		operator()(int i);	 // Return integer
	double operator*(Vector& v); // Inner product
	Vector operator-(Vector& v); // Subtraction
	Vector operator/(Matrix& mat); // Cholesky matrix division
	void operator=(const Vector& v); // Assignment 
	Vector operator*(double scalar); // Scaling
	Vector operator+(Vector& v);	// Summation
	Vector operator/(double scalar); // Scaling
	Matrix operator>>(Vector& v);	// Outer product
	double maximum();

	friend ostream& operator<<(ostream& os, const Vector& v);
};


// BE CAREFUL ONCE BUFFER IS INITIALIZED IT OPERATES IN 'd' DIMENSIONS
#define BUFF_SIZE 10
extern MultiBuffer<Vector> buffer;
extern MultiBuffer<Vector> absbuffer;
extern MultiBuffer<Matrix> matbuffer;

void init_buffer(int nthreads,int d);