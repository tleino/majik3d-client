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
#ifdef WIN32
# include <windows.h>
# include <time.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif
#include <pu.h>
#include <ssg.h>

#include "Menu.hpp"
//#include "Landscape.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "Display.hpp"
#include "Scene.hpp"
#include "Socket.hpp"
#include "Protocol.hpp"
#include "Config.hpp"
#include "Overlay.hpp"

time_t t = time(NULL);
int frames = 0;
int mouse_x, mouse_y;
double start_time;
extern Object *tuxi;

bool Display::sceneVisible = false;

double read_time_of_day();

double read_time_of_day ()
{
#ifdef WIN32
   _int64 u, v ;
   QueryPerformanceCounter   ((LARGE_INTEGER*) &u ) ;
   QueryPerformanceFrequency ((LARGE_INTEGER*) &v ) ;
   return (double)u / (double)v ;
#else
   timeval tv ;
   gettimeofday ( &tv, NULL ) ;
   
   return (double) tv . tv_sec + (double) tv . tv_usec / 1000000.0 ;
#endif
}

Display::Display()
{
   width = 640;
   height = 480;
   initialWidth = width;
   initialHeight = height;
   bpp = 32;

   // cursor = new Cursor;
   
   debug->put("Display constructor");
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   initialWidth = width;
   initialHeight = height;
   
   bpp = b;

   // cursor = new Cursor;
   
   debug->put("Display constructor: width=%d height=%d bpp=%d", width, height, bpp);
}

Display::~Display()
{
   closeScreen();
   
   debug->put("Display destructor");
}

void
Display::openScreen()
{
   //unsigned long flags;
   //GLenum format;
   
   debug->put("Opening screen...");
   if (config->nomousetrap != 0)
	 config->mousetrap = 1;
   
   display->state = 0; /* PRELOAD */
   config->camera = 0;
   display->pitch = 0;
   
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("majik");
   
   glutReshapeFunc(resizeScreen);
   glutDisplayFunc(updateScreen);
   glutKeyboardFunc(input->keyDown);
   glutSpecialFunc(input->specialDown);
   glutMouseFunc(input->mouseDown);
   glutPassiveMotionFunc(mouseMotion);
   glutMotionFunc(mouseMotion);
   //glutIdleFunc(idle);
      
   if (config->nomousetrap != 1)
	 glutWarpPointer (width/2, height/2);
      
   /* PLIB: Simple Scene Graph */
   debug->put("ssgInit");
   ssgInit();
   // initMaterials();
   
   overlay->init();
   scene->init();
   
   glEnable ( GL_DEPTH_TEST);
   
   if (config->nofog)
	 glDisable (GL_FOG);
   if (config->nosmooth)
	 glShadeModel (GL_FLAT);
   else
	 glShadeModel (GL_SMOOTH);
   
   debug->put("Screen opened.");
}

void
Display::mouseMotion(int x, int y)
{
   mouse_x = x; mouse_y = y;
   puMouse (x, y);
   glutPostRedisplay();
}

void 
Display::closeScreen()
{
   debug->put("Closing screen...");
   debug->put("Screen closed.");
}

void
Display::updateScreen()
{
   char *tmp;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   tmp = sock->readPacket();
   
   while (tmp != NULL)
	 {
		protocol->parseCommand(tmp);
		tmp = sock->readPacket();
	 }
   
//   if (mousetrap)
//	 mousetrap();
//   if (Display::sceneVisible)
	 scene->draw();
   
   overlay->draw();

   glutSwapBuffers();
   glutPostRedisplay();
}

void
Display::resizeScreen(int w,int h)
{
   display->width = w;
   display->height = h;
   glViewport(0, 0, w, h);
   overlay->inp->setSize(display->width-5, 5+20 ) ;
   //glutPostRedisplay();

}
