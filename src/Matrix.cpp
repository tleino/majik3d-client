#include <stdio.h>
#include <math.h>
#include "Majik.hpp"

Matrix::Matrix()
{
   id();
}

Matrix::~Matrix()
{
}

const void 
Matrix::print(FILE *fp)
{
   fprintf(fp, "%d %d %d %d\n", mdata[0][0], mdata[0][1], mdata[0][2], mdata[0][3]);
   fprintf(fp, "%d %d %d %d\n", mdata[1][0], mdata[1][1], mdata[1][2], mdata[1][3]);
   fprintf(fp, "%d %d %d %d\n", mdata[2][0], mdata[2][1], mdata[2][2], mdata[2][3]);
   fprintf(fp, "%d %d %d %d\n\n", mdata[3][0], mdata[3][1], mdata[3][2], mdata[3][3]);
}

Matrix 
Matrix::id()
{
   mdata[0][0] = 1.0; mdata[1][0] = 0.0; mdata[2][0] = 0.0; mdata[3][0] = 0.0;
   mdata[0][1] = 0.0; mdata[1][1] = 1.0; mdata[2][1] = 0.0; mdata[3][1] = 0.0;
   mdata[0][2] = 0.0; mdata[1][2] = 0.0; mdata[2][2] = 1.0; mdata[3][2] = 0.0;
   mdata[0][3] = 0.0; mdata[1][3] = 0.0; mdata[2][3] = 0.0; mdata[3][3] = 1.0;
   return *this;
}

Matrix 
Matrix::rotate(const double x, const double y, const double z)
{
   double cx, cy, cz, sx, sy, sz;
   Matrix tmat;
   Matrix rmat;
   
   // Z rotation
   if(z)
	 {
		cz = cos(z); sz = sin(z);
		tmat.id();
		tmat.mdata[0][0] = cz; tmat.mdata[0][1] = sz;
		tmat.mdata[1][0] = -sz; tmat.mdata[1][1] = cz;
		rmat *= tmat;
	 }
   
   // X rotation
   if(x)
	 {
		cx = cos(x); sx = sin(x);
		tmat.id();
		tmat.mdata[1][1] = cx; tmat.mdata[1][2] = sx;
		tmat.mdata[2][1] = -sx; tmat.mdata[2][2] = cx;
		rmat *= tmat;
	 }
      
   // Y rotation
   if(y)
	 {
		cy = cos(y); sy = sin(y);
		tmat.id();
		tmat.mdata[0][0] = cy; tmat.mdata[0][2] = -sy;
		tmat.mdata[2][0] = sy; tmat.mdata[2][2] = cy;
		rmat *= tmat;
	 }
   return *this *= rmat;
}

Matrix 
Matrix::translate(const double x, const double y, const double z)
{
   Matrix tmat;
   tmat.mdata[3][0] = x; 
   tmat.mdata[3][1] = y; 
   tmat.mdata[3][2] = z;
   return *this *= tmat;
}

Matrix 
Matrix::scale(const double x, const double y, const double z)
{
   Matrix smat;
   smat.mdata[0][0] = x;
   smat.mdata[1][1] = y;
   smat.mdata[2][2] = z;
   return *this *= smat;
}

const Matrix 
Matrix::operator+(const Matrix &tmp)
{
   int i, j;
   Matrix retm;
   retm.id();
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   retm.mdata[i][j] += tmp.mdata[i][j];
   return retm;
}

const Matrix 
Matrix::operator-(const Matrix &tmp)
{
   int i, j;
   Matrix retm;
   retm.id();
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   retm.mdata[i][j] -= tmp.mdata[i][j];
   return retm;
}

const Matrix 
Matrix::operator*(const Matrix &tmp)
{
   int i, j;
   Matrix retm;
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   retm.mdata[i][j] = mdata[i][0] * tmp.mdata[0][j] +
	                      mdata[i][1] * tmp.mdata[1][j] +
	                      mdata[i][2] * tmp.mdata[2][j] +
	 					  mdata[i][3] * tmp.mdata[3][j];
   return retm;
}

Matrix 
Matrix::operator=(const Matrix &tmp)
{
   int i, j;
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   mdata[i][j] = tmp.mdata[i][j];
   return *this;
}

Matrix 
Matrix::operator+=(const Matrix &tmp)
{
   int i, j;
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   mdata[i][j] += tmp.mdata[i][j];
   return *this;
}

Matrix 
Matrix::operator-=(const Matrix &tmp)
{
   int i, j;
   for(i=0;i<4;i++)
	 for(j=0;j<4;j++)
	   mdata[i][j] -= tmp.mdata[i][j];
   return *this;
}

Matrix 
Matrix::operator*=(const Matrix &tmp)
{
   return *this = *this * tmp;
}