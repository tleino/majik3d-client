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

class Object;
class mcCamera;
class mcTerrainHeightGen;

#include <ssg.h>
#include <GL/glu.h>

double interpolate(double, double, double);

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
  
  void init(ssgRoot *);

  void draw(mcCamera *);

  float getHOT		(float x, float y)const;
  float getRealHOT	(float x, float y)const;

  void getNormal(sgVec3& nrm, float x, float y)const;

  GLuint getTextureHandle ( int level, int x, int y) const;

  ssgTransform	*getTerrain()const	{ return terrain; }
  
private:
  ssgRoot				*scene_root;
  ssgTransform			*terrain;
  ssgSimpleState		*m_terrainState;
  mcTerrainHeightGen	*m_terraingen;
};

#endif /* __LANDSCAPE_HPP__ */
