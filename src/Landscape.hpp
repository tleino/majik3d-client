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

#ifndef __LANDSCAPE_HPP__
#define __LANDSCAPE_HPP__

#include "Object.hpp"
#include "Detail.hpp"
#include <ssg.h>
#include <GL/glu.h>

double interpolate(double, double, double);

class Landscape
{
 public:
   Landscape();
   ~Landscape();
   
   void Landscape::init( ssgRoot*);
	 
   float getHOT(float, float);
 private:
   ssgBranch *createTileLOD (int, int, ssgSimpleState*, int, float);
   void createTile (ssgTransform*, int, int, ssgSimpleState*);
   ssgRoot scene_root;
   
};

extern Landscape *landscape;

#endif /* __LANDSCAPE_HPP__ */
