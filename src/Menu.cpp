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

#include <pu.h>
#include <ssg.h>

#include "Display.hpp"
#include "Debug.hpp"
#include "Menu.hpp"

char *helpSubmenu[] = {
   "About...",
   NULL 
};
char *settingsSubmenu[] = {
   "Smooth",
   "Flat",
   "Fog",
   "No Fog",
   "Wireframe",
   "No Wireframe",
   "Mousetrap",
   "No Mousetrap",
   "Mouse",
   "No Mouse",
   "Textures",
   "No Textures",
   NULL 
};
char *majikSubmenu[] = {
   "Exit",
   NULL 
};
puCallback helpSubmenuCB[] = {
   menu->aboutCB,
   NULL
};
puCallback settingsSubmenuCB[] = {
   menu->smoothCB,
   menu->flatCB,
   menu->fogCB,
   menu->noFogCB,
   menu->wireframeCB,
   menu->noWireframeCB,
   menu->mousetrapCB,
   menu->noMousetrapCB,
   menu->mouseCB,
   menu->noMouseCB,
   menu->textureCB,
   menu->noTextureCB,
   NULL 
};

puCallback majikSubmenuCB[] = {
   menu->exitCB,
   NULL 
};

puMenuBar *menuBar;

Menu::Menu()
{
   DEBUG("Menu constructor");
}

Menu::~Menu()
{
   DEBUG("Menu destructor");
}

void
Menu::init()
{
   menuBar = new puMenuBar();
	 {
		menuBar->add_submenu ("Majik", majikSubmenu, majikSubmenuCB);
		menuBar->add_submenu ("Settings", settingsSubmenu, settingsSubmenuCB);
		menuBar->add_submenu ("Help", helpSubmenu, helpSubmenuCB);
	 }
   menuBar->close();
}

/* Menu callbacks */

void
Menu::textureCB(puObject *)
{
   ssgOverrideTexture (0);
   display->noTexture = 0;
}

void
Menu::noTextureCB(puObject *)
{
   ssgOverrideTexture (1);
   display->noTexture = 1;
}

void
Menu::aboutCB(puObject *)
{
}

void
Menu::mouseCB(puObject *)
{
   puShowCursor();
}

void
Menu::noMouseCB(puObject *)
{
   puHideCursor();
}

void
Menu::mousetrapCB(puObject *)
{
   display->mousetrap = 1;
}

void
Menu::noMousetrapCB(puObject *)
{
   display->mousetrap = 0;
}

void
Menu::wireframeCB(puObject *)
{
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glDisable(GL_LIGHTING);
   glDisable(GL_FOG);
   glDisable(GL_LIGHT0);
   glShadeModel(GL_FLAT);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   display->wireframe = 1;
}

void
Menu::noWireframeCB(puObject *)
{
   glClearColor (0.0, 0.0, 1.0, 0.0);
   glEnable(GL_LIGHTING);
   glEnable(GL_FOG);
   glEnable(GL_LIGHT0);
   glShadeModel(GL_SMOOTH);
   glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
   display->wireframe = 0;
}

void
Menu::fogCB(puObject *)
{
   glEnable(GL_FOG);
}

void
Menu::noFogCB(puObject *)
{
   glDisable(GL_FOG);
}
void
Menu::flatCB(puObject *)
{
   glShadeModel(GL_FLAT);
}

void
Menu::smoothCB(puObject *)
{
   glShadeModel(GL_SMOOTH);
}

void
Menu::exitCB(puObject *)
{
   DEBUG ("Exit selected from menu");
   exit(0);
}
