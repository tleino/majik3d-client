/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik3d.org>
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

#include "mcDisplay.hpp"
#include "mcDebug.hpp"
#include "mcMenu.hpp"
#include "mcConfig.hpp"

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
  "Toggle Fullscreen",
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
  menu->fullscreenCB,
  NULL 
};

puCallback majikSubmenuCB[] = {
  menu->exitCB,
  NULL 
};

puMenuBar *menuBar;
puDialogBox *dialogBox;
puText *dialogBoxMessage;
puOneShot *dialogBoxButton;

Menu::Menu()
{
  debug->put("Menu constructor");
}

Menu::~Menu()
{
  debug->put("Menu destructor");
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
  
  if (!config->testFlag(mcConfig::MENU))
    menuBar->hide();
}

/* Menu callbacks */

void
Menu::fullscreenCB(puObject *)
{
	if (config->testFlag(mcConfig::FULLSCREEN))
    {
      glutReshapeWindow(display->getInitialWidth(), display->getInitialHeight());
      glutPositionWindow(0, 0);
      config->setFlag ( mcConfig::FULLSCREEN, 0);
    }
  else
    {
      glutFullScreen();
      config->setFlag ( mcConfig::FULLSCREEN, 1);
    }
}

void
Menu::textureCB(puObject *)
{
  ssgOverrideTexture (0);
   config->setFlag(mcConfig::TEXTURED, 0);
}

void
Menu::noTextureCB(puObject *)
{
  ssgOverrideTexture (1);
  config->setFlag(mcConfig::TEXTURED, 1);
}

void
Menu::mkDialogCB(puObject *)
{
  delete dialogBox;
  dialogBox = NULL;
}

void
Menu::mkDialog(char *str)
{
  dialogBox = new puDialogBox (150, 50);
  {
    new puFrame (0, 0, 400, 100);
    dialogBoxMessage = new puText (10, 70);
    dialogBoxMessage->setLabel (str);
    dialogBoxButton = new puOneShot (180, 10, "OK");
    dialogBoxButton->makeReturnDefault (TRUE);
    dialogBoxButton->setCallback (mkDialogCB);
  }
  dialogBox->close();
  dialogBox->reveal();
}

void
Menu::aboutCB(puObject *)
{
  // mkDialog("Majik 3D 0.00/M3");
}

void
Menu::mouseCB(puObject *)
{
  config->setFlag ( mcConfig::MOUSE, 0);
  puShowCursor();
}

void
Menu::noMouseCB(puObject *)
{
  config->setFlag ( mcConfig::MOUSE, 1);
  puHideCursor();
}

void
Menu::mousetrapCB(puObject *)
{
	config->setFlag (mcConfig::MOUSE_TRAP, 1);
//  config->mousetrap = 1;
}

void
Menu::noMousetrapCB(puObject *)
{
	config->setFlag (mcConfig::MOUSE_TRAP, 0);
//  config->nomousetrap = 1;
  //config->mousetrap = 0;
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
  config->setFlag(mcConfig::WIREFRAME, 1);
}

void
Menu::noWireframeCB(puObject *)
{
  glClearColor (0.4f, 0.7f, 1.0f, 1.0f);
  glEnable(GL_LIGHTING);
  glEnable(GL_FOG);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  config->setFlag(mcConfig::WIREFRAME, 0);
}

void
Menu::fogCB(puObject *)
{
  glEnable(GL_FOG);
}

void
Menu::noFogCB(puObject *)
{
	config->setFlag(mcConfig::FOG, 0);

  // FIXME: Crashes on Mesa-3.1 for an unknown reason.
  // glDisable(GL_FOG);
}

void
Menu::flatCB(puObject *)
{
  config->setFlag(mcConfig::SMOOTH, 0);
  glShadeModel(GL_FLAT);
}

void
Menu::smoothCB(puObject *)
{
  config->setFlag(mcConfig::SMOOTH, 1);
  glShadeModel(GL_SMOOTH);
}

void
Menu::exitCB(puObject *)
{
  debug->put("Exit selected from menu");
  exit(0);
}
