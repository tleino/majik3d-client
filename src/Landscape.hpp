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
   int viewport_x;
   int viewport_y;
   int viewport_w;
   int viewport_h;
   unsigned long detail;
   Object *first;
};

#endif
