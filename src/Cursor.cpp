#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Cursor::Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor constructor");
   #endif
}

Cursor::~Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor destructor");
   #endif
}

void
Cursor::draw()
{
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0, display->width, 0, display->height, -1, 250);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glScalef(1.0f/640.0f, 1.0f/640.0f, 1.0f);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   
   
   
   glDisable(GL_BLEND);
}
