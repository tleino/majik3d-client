#ifndef __LANDSCAPE_H__
#define __LANDSCAPE_H__

#include "Object.hpp"
#include "Detail.hpp"

class Landscape
{
 public:
   Landscape();
   ~Landscape();
   void addObject(int);
   void removeObject(int);
 public:
   unsigned long detail;
   Object *first;
};

#endif