/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __P3D_HPP__
#define __P3D_HPP__

#include "Matrix.hpp"

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

#endif /* __P3D_HPP__ */
