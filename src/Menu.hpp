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

#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <pu.h>

class Menu {
 public:
   Menu();
   ~Menu();
   void init();
   
   static void exitCB(puObject *);
   static void flatCB(puObject *);
   static void smoothCB(puObject *);
   static void wireframeCB(puObject *);
   static void noWireframeCB(puObject *);
   static void fogCB(puObject *);
   static void noFogCB(puObject *);
   static void mousetrapCB(puObject *);
   static void noMousetrapCB(puObject *);
   static void mouseCB(puObject *);
   static void noMouseCB(puObject *);
   static void aboutCB(puObject *);
   static void textureCB(puObject *);
   static void noTextureCB(puObject *);
   static void fullscreenCB(puObject *);
   
   static void mkDialog(char *);
   static void mkDialogCB(puObject *);
   
   puMenuBar *menuBar;
};

extern Menu *menu;

#endif /* __MENU_HPP__ */
