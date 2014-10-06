#pragma once
#include "Vector.h"
class Matrix :
	public Vector
{
public:
	int m;
	int r;
	int triangle;


	Matrix(void);
	Matrix(int d);
	Matrix(int r,int m,int real=1);			// refcount
	Matrix(Matrix&& mat);			// Move
	Matrix(const Matrix& mat);			// Copy
									// 1 is standart , 2  is persistent , 0 is abstract
	~Matrix(void);
	
	void readMatrix(char* filename);
	void writeMatrix(char* filename);
	void readBin(char* filename);
	void writeBin(char* filename);
	

	Vector operator()(int i);   // Get row
	Matrix operator*(double scalar);
	void   operator=(const Matrix& m);
	Matrix operator+(Matrix& v);	// Summation
	Matrix operator-(Matrix& v);	// Subtract
	Matrix operator/(double scalar); // Scaling
	Matrix operator<=(const Matrix& mat); //Point by Abstract Matrix
	void zero();
	double sumlogdiag();
	Matrix chol();

	friend ostream& operator<<(ostream& os, const Matrix& v);
};

