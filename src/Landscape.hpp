#ifndef __LANDSCAPE_H__
#define __LANDSCAPE_H__

#include "Object.hpp"
#include "Detail.hpp"

class Landscape
{
 public:
   Landscape();
   ~Landscape();
   void drawLandscape();
   void setViewport(int, int, int, int);
   void addObject(int);
   void removeObject(int);
 public:
   unsigned long detail;
   char *map_1;   /* Terrains for map parts                          */
   char *map_2;   /* Map_1 is the area nearest to player and has the */
   char *map_3;   /* smallest polygons                               */
   char *map_4;
   float  *zmap_1;  /* Height maps */
   float  *zmap_2;
   float  *zmap_3;
   float  *zmap_4;
   Object *first;
 private:
   void makeMap_1();    /* These construct the display lists for maps */
   void makeMap_2();
   void makeMap_3();
   void makeMap_4();
 private:
   int viewport_x;
   int viewport_y;
   int viewport_w;
   int viewport_h;
   float viewport_ratio;
   
   int listId_1;    /* Display list ID for map_1 */
   
   float angle;
};

#endif
