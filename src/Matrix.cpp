#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

Matrix::Matrix(void)
{
	triangle = false;
	r = 0;
	m = 0;
}



Matrix::~Matrix(void)
{
	// Vector destructor takes care of it
}

Matrix::Matrix(int r,int m,int real) : Vector(r*m,real)
{
	this->r = r;
	this->m = m;
	triangle = false;
}


Matrix::Matrix(int r) : Vector(r*r)
{
	this->r = r;
	this->m = r;
	triangle = false;
}




void Matrix::zero()
{
	memset(data,0,r*m*sizeof(double));
}

void Matrix::readMatrix(char* filename)
{
	ifstream file(filename);
	int i,j;
	double val;
	if (!file.is_open())
		printf("Could not operfile...\n");
	else
	{
		printf("Reading %s...\n",filename);
		file >> r >> m;
		data = (double*) malloc(sizeof(double)*r*m);
		type = 1;
		for(i=0;i<r;i++)
			for(j=0;j<m;j++)
				file >> data[i*m+j];
		file.close();
	}
}

void Matrix::writeMatrix(char* filename)
{
	ofstream file(filename);
	int i,j;
	file << r << " " << m << endl;
	for (i=0;i<r;i++)
	{
		for(j=0;j<m;j++)
			file << data[i*m+j] << " ";
		file << endl;
	}
	file.close();
}


void Matrix::readBin(char* filename)
{
	FILE* file;
	fopen_s(&file,filename,"rb");
	int i,j;
	double val;
	if (file==NULL)
		printf("Could not open file...\n");
	else
	{
		printf("Reading %s...\n",filename);
		fread(&r,sizeof(int),1,file);
		fread(&m,sizeof(int),1,file);
		n = r*m;
		type = 1;
		data  = (double*) malloc(sizeof(double)*n);
		fread(data,sizeof(double),n,file);
		fclose(file);
	}
}

void Matrix::writeBin(char* filename)
{
	FILE* file;
	fopen_s(&file,filename,"wb");
	int i,j;
	fwrite(&r,sizeof(int),1,file);
	fwrite(&m,sizeof(int),1,file);

	n = r*m;

	fwrite(data,sizeof(double),n,file);
	fclose(file);
}


/* Get Row */
Vector Matrix::operator()(int i){ 
	absbuffer.get().data = data + m*i;
	return absbuffer.next();
}

double Matrix::sumlogdiag()
{
	int d = r<m?r:m; //Smaller
	int i;
	double sum=0;
	for(i=0;i<d;i++)
		sum += log(data[i+m*i]);
	return sum;
}


/* Cholesky decomposition of square matrix */
Matrix Matrix::chol()
{
	double s = 0;
	int i,j,k;
	Matrix& mat = matbuffer.get();
	// double debug;
	mat.zero();
    for ( i = 0; i < m ; i++)
	{
        for ( j = 0; j < i; j++) {
            for (s=0,k = 0; k < j; k++)
                s += mat.data[i* m  + k] * mat.data[j* m  + k];
            mat.data[i* m  + j] = ( (data[i* m  + j] - s) / mat.data[j* m  + j] ) ;
//		if (mat.data[i*m + j]!=mat.data[i*m + j])
//			printf("ERROR Matrix is singular\n");
        }

	for (s=0,k = 0; k < i; k++)
		s += mat.data[i* m  + k] * mat.data[i* m  + k];
	mat.data[i*m + i] = sqrt(data[i* m  + i] - s);


	}


	mat.triangle = true;
	return matbuffer.next();
}


Matrix Matrix::operator*(double scalar)
{
	int i;
	Matrix& mati = matbuffer.get();
	for(i=0;i<n;i++)
		mati.data[i] = data[i]*scalar;
	return matbuffer.next();
}

Matrix Matrix::operator/(double scalar)
{
	int i;
	Matrix& mati = matbuffer.get();
	double divone = 1.0/scalar;
	for(i=0;i<n;i++)
		mati.data[i] = data[i]*divone;
	return matbuffer.next();
}

Matrix Matrix::operator+(Matrix& mat)
{
	int i;
	Matrix& mati = matbuffer.get();
	for(i=0;i<n;i++)
		mati.data[i] = data[i]+mat.data[i];
	return matbuffer.next();
}

Matrix Matrix::operator-(Matrix& mat)
{
	int i;
	Matrix& mati = matbuffer.get();
	for(i=0;i<n;i++)
		mati.data[i] = data[i]-mat.data[i];
	return matbuffer.next();
}

void  Matrix::operator=(const Matrix& mat)
{
	Vector::operator=(mat);
	triangle = mat.triangle;
	m = mat.m;
	r = mat.r;
}



Matrix::Matrix(Matrix&& mat) : Vector(mat)
{
}

Matrix::Matrix(const Matrix& mat) : Vector(mat) , r(mat.r) , m(mat.m) , triangle(mat.triangle)
{
}


ostream& operator<<(ostream& os, const Matrix& v)
{   // It does not save type , it save as real vector always
	os.write((char*) &v.r,sizeof(int)); 
	os.write((char*) &v.m,sizeof(int)); 
	os.write((char*) v.data,sizeof(double)*v.n); 
	// os.write((char*) &v.triangle,sizeof(int));  // Not used greatly
	return os;
}