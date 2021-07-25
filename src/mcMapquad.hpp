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

#ifndef __MAPQUAD_HPP__
#define __MAPQUAD_HPP__

#include <ssg.h>

#include "mcCamera.hpp"

class TerrainBlock;

/// Size of one map chunk.
#define   MAP_SIZE    16
#define   NUM_VISIBLE_LEVELS 5

/** A map chunk. */


class Mapquad
{
public:
	enum Treesize
	{
		MAX_LEVEL = 12
	};

  struct Map_data
  {
    char *terrain;
    double *height;
  };

  ~Mapquad();
  
  ssgTransform *getTrans() { return trans; }

  Mapquad *getMapquad(int level, int x, int y);
  Mapquad *tryMapquad(int level, int x, int y);
  
  
  char *getMap ();
  void setMap (Map_data);
  

  bool isMapReceived();

  static Mapquad& getRootMap() { return Mapquad::root_map; }

  static void draw(sgVec2 tri[3]);
  static void triangulate(int x, int y);
  static void clean(int x, int y);
  
  //   static int stats[13];
  // static void printStats();
private:
  static Mapquad root_map;

  Mapquad(Mapquad *parent, int level, int top_x, int top_y);

  void drawRecursive(sgVec2 tri[3]);
  void cleanRecursive(int x, int y);

  void resetBlocks();
  void triangulateBlocks();
  void exchangeBorders();
  void selectLOD(int, int, int );
  void selectLOD(int);

  Mapquad *parent;
  Mapquad *child1;
  Mapquad *child2;
  Mapquad *child3;
  Mapquad *child4;

  Map_data submap1;
  Map_data submap2;
  Map_data submap3;
  Map_data submap4;

  int *lod_indices;

  TerrainBlock *block;
  ssgTransform *trans;
  ssgSelector  *lod_switch;

  int level;
  int top_x;
  int top_y;
  int mid_x;
  int mid_y;

  int reference_count;
  ///
  int map_requested;
  ///
  Map_data map_data[NUM_VISIBLE_LEVELS];
  ///
  int  incRef  ();
  ///
  int  decRef  ();
  /// Clean up the map from the memory when it is no longer needed.
  void cleanUp ();
  ///
  Mapquad *getChild1();
  Mapquad *getChild2();
  Mapquad *getChild3();
  Mapquad *getChild4();

   bool mapReceived;
   int current_lod;
   
   /* kludge */
//	static ssgSimpleState *state;

};


#endif // __MAPQUAD_HPP__
