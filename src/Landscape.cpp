#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Landscape::Landscape()
{
   viewport_x = 0;
   viewport_y = 0;
   viewport_w = display->width;
   viewport_h = display->height;
   
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
   float h;
   
   h = (float) viewport_h / (float) viewport_w;
   
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -h, h, 5.0, 1000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glPushMatrix();
   
   glBegin(GL_QUADS);
   
    glVertex3f(-1,-1, -10);
    glVertex3f(-1, 1, -10);
    glVertex3f(1,  1, -10);
    glVertex3f(1, -1, -10);
   
   glEnd();

   glPopMatrix();
   
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
