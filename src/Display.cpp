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

#include "Landscape.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "Display.hpp"

puText *fps_text;
puMenuBar *main_menu_bar;
time_t t = time(NULL);
int frames = 0;
int mouse_x, mouse_y;
int mousetrap = 1;
double start_time;

void exit_cb(puObject *);
void flat_cb(puObject *);
void smooth_cb(puObject *);
void wireframe_cb(puObject *);
void nowireframe_cb(puObject *);
void fog_cb(puObject *);
void nofog_cb(puObject *);
void mousetrap_cb(puObject *);
void nomousetrap_cb(puObject *);
void mouse_cb(puObject *);
void nomouse_cb(puObject *);
void about_cb(puObject *);

char *help_submenu[] = { "About...", NULL };
char *settings_submenu[] = { "Smooth", "Flat", "Fog", "No Fog", "Wireframe", "No Wireframe", "Mousetrap", "No Mousetrap", "Mouse", "No Mouse", NULL };
char *majik_submenu[] = { "Exit", NULL };
puCallback help_submenu_cb[] = { about_cb, NULL };
puCallback settings_submenu_cb[] = { smooth_cb, flat_cb, fog_cb, nofog_cb, wireframe_cb, nowireframe_cb, mousetrap_cb, nomousetrap_cb, mouse_cb, nomouse_cb, NULL };
puCallback majik_submenu_cb[] = { exit_cb, NULL };

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

void
about_cb(puObject *)
{
}

void
mouse_cb(puObject *)
{
   puShowCursor();
}

void
nomouse_cb(puObject *)
{
   puHideCursor();
}

void
mousetrap_cb(puObject *)
{
  mousetrap = 1;
}
   
void
nomousetrap_cb(puObject *)
{
   mousetrap = 0;
}

void
wireframe_cb(puObject *)
{
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glDisable(GL_LIGHTING);
   glDisable(GL_FOG);
   glDisable(GL_LIGHT0);
   glShadeModel(GL_FLAT);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void
nowireframe_cb(puObject *)
{
   glClearColor (0.0, 0.0, 1.0, 0.0);
   glEnable(GL_LIGHTING);
   glEnable(GL_FOG);
   glEnable(GL_LIGHT0);
   glShadeModel(GL_SMOOTH);
   glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void
fog_cb(puObject *)
{
   glEnable(GL_FOG);
}

void
nofog_cb(puObject *)
{
   glDisable(GL_FOG);
}

void
flat_cb(puObject *)
{
   glShadeModel(GL_FLAT);
}

void
smooth_cb(puObject *)
{
   glShadeModel(GL_SMOOTH);
}

void
exit_cb(puObject *)
{
   printf ("exit!\n");
   exit(0);
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
   
   fps_text = new puText (5, 10);
   fps_text->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
   
   main_menu_bar = new puMenuBar();
     {
	main_menu_bar->add_submenu ("Majik", majik_submenu, majik_submenu_cb);
	main_menu_bar->add_submenu ("Settings", settings_submenu, settings_submenu_cb);
		main_menu_bar->add_submenu ("Help", help_submenu, help_submenu_cb);
     }
   main_menu_bar->close();
   
   landscape->init();
   // cursor->init();
   
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
   char fps[80];
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   landscape->draw();
   
   /* Trap the mouse */
      
   if (mouse_x < 1 && mousetrap) {
	  mouse_x = 2; warp = TRUE;
   } else if (mouse_x > display->width-1 && mousetrap) {
	  mouse_x = display->width-2; warp = TRUE;
   }
   if (mouse_y < 1 && mousetrap) {
	  mouse_y = 2; warp = TRUE;
   } else if (mouse_y > display->height-1 && mousetrap) {
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

   sprintf (fps, "fps: %f", (float) 1/(read_time_of_day() - start_time));
   fps_text->setLabel (fps);
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
