#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <stdio.h>
#include "Majik.hpp"

class Matrix
{
 public:
   Matrix();
   ~Matrix();
   const void print(FILE *);
   Matrix id();
   Matrix rotate(const double,const double,const double);
   Matrix translate(const double,const double,const double);
   Matrix scale(const double,const double,const double);
   const Matrix operator+(const Matrix &);
   const Matrix operator-(const Matrix &);
   const Matrix operator*(const Matrix &);
   Matrix operator=(const Matrix &);
   Matrix operator+=(const Matrix &);
   Matrix operator-=(const Matrix &);
   Matrix operator*=(const Matrix &);
   double mdata[4][4];
};

#endif