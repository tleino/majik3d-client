#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Landscape::Landscape()
{
   viewport_x = 0;
   viewport_y = 0;
   viewport_w = display->width;
   viewport_h = display->height;
   
   listId_1 = -1;
   
   angle = 0;
   
   first = NULL;
   #ifdef DEBUG
	 debug->put("Landscape constructor");
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
	 debug->put("Landscape destructor");
   #endif
}

void 
Landscape::drawLandscape()
{
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -viewport_ratio, -viewport_ratio, 5.0, 1000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   /* Load camera matrix here            */


   /* Position sun                       */
   
   /* Draw sky                           */
   /* - Check for detail settings        */

   
   /* Draw terrain                       */
   /*  - Check for detail settings       */
   /*  - Cull map regions                */
   
   
   /* Draw objects                       */
   /*  - Check for 1st-person/3rd-person */
   /*  - Check for detail settings       */
}

void 
Landscape::setViewport(int x, int y, int w, int h)
{
   viewport_x = x;
   viewport_y = y;
   viewport_w = w;
   viewport_h = h;
   viewport_ratio = h/w;
}

void 
Landscape::makeMap_1()
{
   /* Check for already existing display list */
   if (listId_1 != -1)
	 glDeleteLists(listId_1, 1);
   
   /* Generate new list ID */
   listId_1 = glGenLists(1);
   glNewList(listId_1, GL_COMPILE);
   
   /* SOLUTION NEEDED: It is hardly wise to calculate the height and normals */
   /*                  every time. Somekind of buffer for calculated perlin  */
   /*                  values and vertex normals is needed. Same goes for    */
   /*                  blended textures.                                     */
                     
   glEndList();
}

void 
Landscape::makeMap_2()
{
}

void 
Landscape::makeMap_3()
{
}

void 
Landscape::makeMap_4()
{
}

void 
Landscape::addObject(int oid)
{
}

void 
Landscape::removeObject(int oid)
{
}
