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

#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

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

#endif /* __MATRIX_HPP__ */
