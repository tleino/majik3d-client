#include <stdio.h>
#include <math.h>
#include "Majik.hpp"

P3D::P3D(double x1, double y1, double z1)
{
   x = x1;
   y = y1;
   z = z1;
}

P3D::P3D()
{
   x = y = z = 0.0;
}

P3D::~P3D()
{
}

const void 
P3D::print(FILE *fp)
{
   fprintf(fp, "%f %f %f\n\n", x, y, z);
}

P3D 
P3D::negate()
{
   x = -x;
   y = -y;
   z = -z;
   return *this;
}

P3D 
P3D::normalize()
{
   return *this /= magnitude();
}

const double 
P3D::magnitude()
{
   return sqrt(x*x + y*y + z*z);
}

const double 
P3D::dotProduct(const P3D &tmp)
{
   return (x*tmp.x + y*tmp.y + z*tmp.z);
}

const P3D 
P3D::crossProduct(const P3D &tmp)
{
   P3D retp;
   retp.x = y*tmp.z - z*tmp.y;
   retp.y = z*tmp.x - x*tmp.z;
   retp.z = x*tmp.y - y*tmp.x;
   return retp;
}

P3D 
P3D::set(const double x1, const double y1, const double z1)
{
   x = x1;
   y = y1;
   z = z1;
   return *this;
}

const P3D 
P3D::operator+(const P3D &tmp)
{
   P3D retp;
   retp.x = x + tmp.x;
   retp.y = y + tmp.y;
   retp.z = z + tmp.z;
   return retp;
}

const P3D 
P3D::operator-(const P3D &tmp)
{
   P3D retp;
   retp.x = x - tmp.x;
   retp.y = y - tmp.y;
   retp.z = z - tmp.z;
   return retp;
}

const P3D 
P3D::operator*(const P3D &tmp)
{
   P3D retp;
   retp.x = x * tmp.x;
   retp.y = y * tmp.y;
   retp.z = z * tmp.z;
   return retp;
}

const P3D 
P3D::operator*(const Matrix &tmp)
{
   P3D retp;
   retp.x = tmp.mdata[0][0] * x + tmp.mdata[1][0] * y + tmp.mdata[2][0] * z + tmp.mdata[3][0];
   retp.y = tmp.mdata[0][1] * x + tmp.mdata[1][1] * y + tmp.mdata[2][1] * z + tmp.mdata[3][1];
   retp.z = tmp.mdata[0][2] * x + tmp.mdata[1][2] * y + tmp.mdata[2][2] * z + tmp.mdata[3][2];
   return retp;
}

const P3D 
P3D::operator*(const double f)
{
   P3D retp;
   retp.x *= f;
   retp.y *= f;
   retp.z *= f;
   return retp;
}

const P3D 
P3D::operator/(const P3D &tmp)
{
   P3D retp;
   retp.x = x / tmp.x;
   retp.y = y / tmp.y;
   retp.z = z / tmp.z;
   return retp;
}

const P3D 
P3D::operator/(const double f)
{
   P3D retp;
   retp.x = x / f;
   retp.y = y / f;
   retp.z = z / f;
   return retp;
}

P3D 
P3D::operator+=(const P3D &tmp)
{
   x += tmp.x;
   y += tmp.y;
   z += tmp.z;
   return *this;
}

P3D 
P3D::operator-=(const P3D &tmp)
{
   x -= tmp.x;
   y -= tmp.y;
   z -= tmp.z;
   return *this;
}

P3D 
P3D::operator*=(const P3D &tmp)
{
   x *= tmp.x;
   y *= tmp.y;
   z *= tmp.z;
   return *this;
}

P3D 
P3D::operator*=(const Matrix &tmp)
{
   x = tmp.mdata[0][0] * x + tmp.mdata[1][0] * y + tmp.mdata[2][0] * z + tmp.mdata[3][0];
   y = tmp.mdata[0][1] * x + tmp.mdata[1][1] * y + tmp.mdata[2][1] * z + tmp.mdata[3][1];
   z = tmp.mdata[0][2] * x + tmp.mdata[1][2] * y + tmp.mdata[2][2] * z + tmp.mdata[3][2];
   return *this;
}

P3D 
P3D::operator*=(const double f)
{
   x *= f;
   y *= f;
   z *= f;
   return *this;
}

P3D 
P3D::operator/=(const P3D &tmp)
{
   x /= tmp.x;
   y /= tmp.y;
   z /= tmp.z;
   return *this;
}

P3D 
P3D::operator/=(const double f)
{
   x /= f;
   y /= f;
   z /= f;
   return *this;
}

P3D 
P3D::operator=(const P3D &tmp)
{
   x = tmp.x;
   y = tmp.y;
   z = tmp.z;
   return *this;
}

const bool 
P3D::operator==(const P3D &tmp)
{
   if(x != tmp.x || y != tmp.y || z != tmp.z)
	 return false;
   else
	 return true;
}

const bool 
P3D::operator!=(const P3D &tmp)
{
   if(x != tmp.x || y != tmp.y || z != tmp.z)
	 return true;
   else
	 return false;
}