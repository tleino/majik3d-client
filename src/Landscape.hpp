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

#include <ssg.h>
#include <GL/glu.h>

double interpolate(double, double, double);

/** A landscape class. This class creates the landscape. */

class Landscape
{
 public:
   Landscape();
   ~Landscape();

   ///
   ssgTransform   *terrain;
   /// Initializes landscape.
   void Landscape::init(ssgRoot *);
	 
   /// Get height of terrain at the point given as an argument.
   float getHOT(float x, float y);
   /// 0 = not initialized, 1 = running through init(), 2 = done with init().
   int initialized;
   ///
   ssgBranch *createTileLOD (int level, int x, int y, int ntris, char *terrain_map);
   ///
   void createTile (ssgTransform *tile, int x, int y, ssgSimpleState *state);
   /// A pointer to SSG scene's root.
   ssgRoot *scene_root;
   
};

extern Landscape *landscape;

#endif /* __LANDSCAPE_HPP__ */
