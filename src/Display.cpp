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
#include <pu.h>
#include "Majik.hpp"

puText *fps_text;
puMenuBar *main_menu_bar;
time_t t = time(NULL);
int frames = 0;

void exit_cb(puObject *);
void flat_cb(puObject *);
void smooth_cb(puObject *);
void lighting_cb(puObject *);
void nolighting_cb(puObject *);
void fog_cb(puObject *);
void nofog_cb(puObject *);

char *majik_submenu[] = { "Exit", "========", "Smooth", "Flat", "Lighting", "No Lighting", "Fog", "No Fog", NULL };
puCallback majik_submenu_cb[] = { exit_cb, NULL, smooth_cb, flat_cb, lighting_cb, nolighting_cb, fog_cb, nofog_cb, NULL };

void
lighting_cb(puObject *)
{
   glEnable(GL_LIGHTING);
}

void
nolighting_cb(puObject *)
{
   glDisable(GL_LIGHTING);
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
   
   #ifdef DEBUG
	 debug->put("Display constructor");
   #endif
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;

   // cursor = new Cursor;
   
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
   glutReshapeFunc(resizeScreen);
   glutDisplayFunc(updateScreen);
   glutKeyboardFunc(input->keyDown);
   glutSpecialFunc(input->specialDown);
   glutMouseFunc(input->mouseDown);
   glutPassiveMotionFunc(mouseMotion);
   glutMotionFunc(mouseMotion);
   //glutIdleFunc(idle);
   
   /* PLIB: Picoscopic User Interface */
   puInit();
   puShowCursor();
   
   /* PLIB: Font Library */
   fntTexFont *fnt = new fntTexFont;
   fnt->load ("gfx/sorority.txf");
   puFont sorority (fnt, 12);
   puSetDefaultFonts (sorority, PUFONT_8_BY_13);
   puSetDefaultStyle (PUSTYLE_SMALL_SHADED);
   puSetDefaultColourScheme (1.0, 1.0, 1.0, 0.6f);
   
   fps_text = new puText (5, 10);
   fps_text->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
   
   main_menu_bar = new puMenuBar();
     {
	main_menu_bar->add_submenu ("Majik", majik_submenu, majik_submenu_cb);
     }
   main_menu_bar->close();
   
   landscape->init();
   // cursor->init();
   
   #ifdef DEBUG
	 debug->put("Screen opened.");
   #endif
}

void
Display::mouseMotion(int x, int y)
{
   puMouse (x, y);
   glutPostRedisplay();
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

/*void
Display::idle()
{
//   landscape->angle += 2.0;
   puDisplay();
   glutPostRedisplay();
}*/

void 
Display::updateScreen()
{
   int t2 = (int) (time(NULL) - t);
   frames++;
   char fps[80];
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   landscape->drawLandscape();
   
   /* Draw menus etc using PUI (part of PLIB) */
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glAlphaFunc(GL_GREATER,0.1f);
   sprintf (fps, "fps: %f", (float) frames/t2);
   fps_text->setLabel (fps);
   puDisplay();

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
