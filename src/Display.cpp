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

#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Display::Display()
{
   width = 800;
   height = 600;
   bpp = 32;

   cursor = new Cursor;
   
   #ifdef DEBUG
	 debug->put("Display constructor");
   #endif
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;

   cursor = new Cursor;
   
   #ifdef DEBUG
	 debug->put("Display constructor: width=%d height=%d bpp=%d", width, height, bpp);
   #endif	 
}

Display::~Display()
{
   closeScreen();
   
   #ifdef DEBUG
	 debug->put("Display destructor");
   #endif
}

void
Display::openScreen()
{
   unsigned long flags;
   GLenum format;
   
   #ifdef DEBUG
	 debug->put("Opening screen...");
   #endif
   
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("majik");
   glutDisplayFunc(updateScreen);
   glutKeyboardFunc(input->keyDown);
   glutSpecialFunc(input->specialDown);
   glutPassiveMotionFunc(input->mousePassiveMotion);
   glutIdleFunc(idle);

   landscape->init();
   cursor->init();
   
   #ifdef DEBUG
	 debug->put("Screen opened.");
   #endif
}

void 
Display::closeScreen()
{
   #ifdef DEBUG
	 debug->put("Closing screen...");
   #endif
      
   #ifdef DEBUG
	 debug->put("Screen closed.");
   #endif
}

void
Display::idle()
{
//   landscape->angle += 2.0;
   glutPostRedisplay();
}

void 
Display::updateScreen()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   landscape->drawLandscape();
   
   /* INSERT HERE: Draw menus etc */
   
   display->cursor->draw();
   
   glutSwapBuffers();
}
