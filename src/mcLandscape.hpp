/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik3d.org>
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

#include "mcObject.hpp"

#include <ssg.h>
#include <GL/glu.h>

double interpolate(double, double, double);

/** A landscape class. This class creates the landscape. */

class Landscape
{
public:

	enum BlockSettings
	{
		TERRAIN_RESOLUTION = 16,
		BLOCK_WIDTH = 512
	};


  Landscape();
  ~Landscape();
  
  void Landscape::init(ssgRoot *);

  float getHOT(float x, float y);

  void getNormal(sgVec3& nrm, float x, float y);

  GLuint getTextureHandle ( int level, int x, int y);

//  ssgBranch *createTileLOD (int level, int x, int y, int ntris,
//			    char *terrain_map);

 // ssgBranch *createBlock(int x, int y);

  ssgTransform	*getTerrain()	{ return terrain; }
  
private:
  ssgRoot *scene_root;
  ssgTransform   *terrain;
};

extern Landscape *landscape;

#endif /* __LANDSCAPE_HPP__ */
