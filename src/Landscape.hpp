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
#include "P3D.hpp"
#include <glu.h>


double interpolate(double, double, double);

class Landscape
{
 public:
   Landscape();
   ~Landscape();
   void init();
   void draw();
   double getHeight(int, int);
   void setMap_1(char *);
   void setMap_2(char *);
   void setMap_3(char *);
   void shiftMap_1(int);
   void shiftMap_2(int);
   void shiftMap_3(int);
 public:
   unsigned long detail;
   char *map_1;   /* Terrains for map parts                          */
   char *map_2;   /* Map_1 is the area nearest to player and has the */
   char *map_3;   /* smallest polygons                               */
   char *map_4;
   double *zmap_1;  /* Height maps */
   double *zmap_2;
   double *zmap_3;
   double *zmap_4;
   int map1_x;
   int map1_y;
   int map2_x;
   int map2_y;
   int map3_x;
   int map3_y;
   int map1_shift_x;
   int map1_shift_y;
   int map2_shift_x;
   int map2_shift_y;
   Object *first;
 public:
   void initMap_1Mesh();
   void initMap_2Mesh();
   void initMap_3Mesh();
   void initMap_4Mesh();
   void makeMap_1();    /* These construct the display lists for maps */
   void makeMap_2();
   void makeMap_3();
   void makeMap_4();
   void makeHeightMaps();
 private:
   void drawMap_1();
   
   
   struct mesh {
	  int numVertices;
	  double *vertices;   
	  P3D *face_normals;
	  double *normals;
   };
   
   struct mesh map_1Mesh, map_2Mesh, map_3Mesh, map4Mesh;

   ssgBranch *map1_branch;
   ssgBranch *map2_branch;
   ssgBranch *map3_branch;
   
   int listId_1;    /* Display list ID for map_1 */
   int listId_2;
   int listId_3;
   int listId_4;



};

extern Landscape *landscape;

#endif /* __LANDSCAPE_HPP__ */
