#ifndef __P3D_H__
#define __P3D_H__

#include <stdio.h>
#include "Majik.hpp"

class P3D
{
 public:
   P3D(double, double, double);
   P3D();
   ~P3D();
   const void print(FILE *fp);
   P3D negate();
   P3D normalize();
   const double magnitude();
   const double dotProduct(const P3D &);
   const P3D crossProduct(const P3D &);
   P3D set(const double, const double, const double);
   const P3D operator+(const P3D &);
   const P3D operator-(const P3D &);
   const P3D operator*(const P3D &);
   const P3D operator*(const Matrix &);
   const P3D operator*(const double);
   const P3D operator/(const P3D &);
   const P3D operator/(const double);
   P3D operator+=(const P3D &);
   P3D operator-=(const P3D &);
   P3D operator*=(const P3D &);
   P3D operator*=(const Matrix &);
   P3D operator*=(const double);
   P3D operator/=(const P3D &);
   P3D operator/=(const double);
   P3D operator=(const P3D &);
   const bool operator==(const P3D &);
   const bool operator!=(const P3D &);
   double x, y, z;
};

#endif