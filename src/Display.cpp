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
#include <sys/time.h>
#include <pu.h>

#include "Menu.hpp"
#include "Landscape.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "Display.hpp"

puText *status_text;
time_t t = time(NULL);
int frames = 0;
int mouse_x, mouse_y;
double start_time;

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
   width = 800;
   height = 600;
   bpp = 32;

   // cursor = new Cursor;
   
   DEBUG("Display constructor");
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;

   // cursor = new Cursor;
   
   DEBUG (debug->string("Display constructor: width=%d height=%d bpp=%d", width, height, bpp));
}

Display::~Display()
{
   closeScreen();
   
   DEBUG ("Display destructor");
}

void
Display::openScreen()
{
   unsigned long flags;
   GLenum format;
   
   DEBUG ("Opening screen...");
   mousetrap = 1;
   
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
   
   glutWarpPointer (width/2, height/2);
   
   /* PLIB: Picoscopic User Interface */
   puInit();
   puShowCursor();
   
   /* PLIB: Font Library */
   fntTexFont *fnt = new fntTexFont;
   // fnt->load ("gfx/sorority.txf");
   // puFont sorority (fnt, 12);
   // puSetDefaultFonts (sorority, PUFONT_8_BY_13);
   puSetDefaultStyle (PUSTYLE_SMALL_SHADED);
   puSetDefaultColourScheme (1.0, 1.0, 1.0, 0.6f);
   
   status_text = new puText (5, 10);
   status_text->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
   
   menu->init();   
   landscape->init();
   
   DEBUG ("Screen opened.");
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
   DEBUG ("Closing screen...");
   DEBUG ("Screen closed.");
}

void 
Display::updateScreen()
{
   int t2 = (int) (time(NULL) - t), warp = 0;
   frames++;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   landscape->draw();
   
   /* Trap the mouse */
      
   if (mouse_x < 1 && display->mousetrap) {
	  mouse_x = 2; warp = TRUE;
   } else if (mouse_x > display->width-1 && display->mousetrap) {
	  mouse_x = display->width-2; warp = TRUE;
   }
   if (mouse_y < 1 && display->mousetrap) {
	  mouse_y = 2; warp = TRUE;
   } else if (mouse_y > display->height-1 && display->mousetrap) {
	  mouse_y = display->height-2; warp = TRUE;
   }

   if (warp) {
	  glutWarpPointer (mouse_x, mouse_y);
	  warp = 0;
   }
	  
   /* Draw menus etc using PUI (part of PLIB) */
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glAlphaFunc(GL_GREATER,0.1f);
   
   status_text->setLabel ((char *) debug->string("fps: %.1f angle: %.1f tilt: %.1f distance: %.1f",
										(float) 1/(read_time_of_day() - start_time),
										landscape->angle, landscape->tilt, landscape->distance));
   start_time = read_time_of_day();
   
   puDisplay();
   glDisable(GL_BLEND);
   
   /* Display the cursor. FIXME: Should be displayed only if using hardware
      acceleration which doesn't provide it's own cursor */
   // display->cursor->draw();
   
   glutSwapBuffers();
   glutPostRedisplay();
}

void
Display::resizeScreen(int w,int h)
{
   display->width = w;
   display->height = h;
   
   landscape->setViewport(0,0,w,h);
   glViewport(0,0,w,h);
   glutPostRedisplay();
}
