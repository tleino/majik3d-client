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

#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__

#include <pu.h>

/** A display class. This class handles all the things to do with the
    display, opening window, reshaping, updating and so on. */

class Display
{
 public:
   Display();
   /// Init the display using width, height and bpp as arguments.
   Display(int width, int height, int bpp);
   ~Display();
   ///
   void openScreen();
   ///
   void closeScreen();
   ///
   static void updateScreen();
   ///
   static void resizeScreen(int width, int height);
   ///
   static void mouseMotion(int x,int y);
   puText *status_text;
 private:
   static void idle();
 public: 
   ///
   int width;
   ///
   int height;
   ///
   int bpp;
   ///
   int initialWidth;
   ///
   int initialHeight;
   ///
   int mousetrap;
   ///
   int nomousetrap;
   ///
   int noTexture;
   ///
   int wireframe;
   ///
   int nomouse;
   ///
   int nofog;
   ///
   int nosmooth;
   ///
   int nomenu;
   ///
   int camera;
   ///
   int fullscreen;
   ///
   int state;
   ///
   float pitch;
   ///
   int inp_command;
   ///
   puInput *inp;
   ///
   float sun_x, sun_y, sun_z;
   char stxt[8192*3];

};

extern Display *display;

#endif /* __DISPLAY_HPP__ */
