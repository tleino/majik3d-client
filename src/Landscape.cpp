#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Landscape::Landscape()
{
   first = NULL;
   #ifdef DEBUG
	 Debug("Landscape constructor");
   #endif
}

Landscape::~Landscape()
{
   if(first != NULL)
	 {
		delete first;
		first = NULL;
	 }
   #ifdef DEBUG
	 Debug("Landscape destructor");
   #endif
}

void Landscape::drawLandscape()
{
}

void Landscape::setViewport(int x, int y, int w, int h)
{
   viewport_x = x;
   viewport_y = y;
   viewport_w = w;
   viewport_h = h;
}

void Landscape::addObject(int oid)
{
}

void Landscape::removeObject(int oid)
{
}
