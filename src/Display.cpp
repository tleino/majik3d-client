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
   
   glutSwapBuffers();
}
