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

 ///
  Mapquad(Mapquad *parent, int level, int top_x, int top_y);
  ~Mapquad();
  
  static Mapquad *root_map;
  
  
  Map_data submap1;
  Map_data submap2;
  Map_data submap3;
  Map_data submap4;

  ///
  ssgSelector  *m_lodSwitch;
  
  ///
  int *lod_indices;
  
  ///
  TerrainBlock *m_block;
  
  ///
  int m_mapRequested;
  ///
  Map_data map_data[NUM_VISIBLE_LEVELS];

  int  incRef  ();
  int  decRef  ();
  void cleanUp ();
  
  Mapquad *getChild1();
  Mapquad *getChild2();
  Mapquad *getChild3();
  Mapquad *getChild4();

  Mapquad *getMapquad(int level, int x, int y);
  Mapquad *tryMapquad(int level, int x, int y);
  ///
  char *getMap ();
  ///
  void setMap (Map_data);
  ///
//  void setSubMap (int, Map_data);
  ///
  void draw(); //sgVec3, float);
	// hello
  void resetBlocks();
  void triangulateBlocks();
  void exchangeBorders();
  void selectLOD(int, int, int );
  void selectLOD(int);
  ///
//  void divideMap(int, Map_data&, Map_data&, Map_data&, Map_data&, Map_data&);
  ///
//  void show();
  //void hide();
  ///
  bool isMapReceived();
  
  /*
    getMaps ( ... );
    getInventory ( ... );
    getObservers ( ... );
  */ 
  
  //   static int stats[13];
  
  // static void printStats();
  ssgTransform	*getTrans() { return m_trans; }

private:
	bool m_mapReceived;
	int m_currentLod;
	int m_refCount;

	Mapquad*	m_parent;
	Mapquad*	m_child1;
	Mapquad*	m_child2;
	Mapquad*	m_child3;
	Mapquad*	m_child4;

	ssgTransform *m_trans;

	int m_level;
	int m_top_x;
	int m_top_y;
	int m_mid_x;
	int m_mid_y;   
};


#endif // __MAPQUAD_HPP__
