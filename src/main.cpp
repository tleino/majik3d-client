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

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */
#include <iostream.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "mcMenu.hpp"
#include "mcError.hpp"
#include "mcDebug.hpp"
#include "mcSocket.hpp"
#include "mcDisplay.hpp"
#include "mcLandscape.hpp"
#include "mcInput.hpp"
#include "mcConfig.hpp"
#include "mcScene.hpp"
#include "mcOverlay.hpp"
#include "mcProtocol.hpp"
#include "mcPerlin.hpp"
#include "mcMapquad.hpp"
//#include "mcSky.hpp"

bool quit = false;
mcError *error = NULL;
mcConfig *config = NULL;
Debug *debug = NULL;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;
Input *input = NULL;
Menu *menu = NULL;
Scene *scene = NULL;
Protocol *protocol = NULL;
Perlin *perlin = NULL;
Overlay *overlay = NULL;
Mapquad *Mapquad::root_map = NULL;
//mcSky *mc_sky = NULL;


int
main(int argc, char **argv)
{
#ifndef WIN32
  printf ("Majik 3D client " VERSION " / " CPU_VENDOR_OS " ("__DATE__")\n");
  printf ("Copyright (C) 1999 Majik Development Team <majik@majik3d.org>\n\n");
#endif /* WIN32 */

  // Initialize the necessary global variables as proper objects
  error = new mcError;
  config = new mcConfig;
  config->readConfig();
  debug = new Debug;
  display = new Display;
  landscape = new Landscape;
  input = new Input;
  menu = new Menu;
  scene = new Scene;
  protocol = new Protocol;
  perlin = new Perlin;
  overlay = new Overlay;
//  mc_sky = new mcSky(16, 8);
  Mapquad::root_map = new Mapquad (NULL, 0, 0, 0);


  // Check the config object for ip / port settings and create the socket
  // accordingly.
 if(config->getServerIP() != NULL || config->getServerPort())
   sock = new Socket(config->getServerIP(), config->getServerPort());
  else
   sock = new Socket("195.74.0.76", 4000);
   
  glutInit(&argc, argv);
  
  // Assign proper width / height 
  if(config->getScreenWidth() && config->getScreenHeight() && config->getScreenBpp())
    {
	  display->setScreenProperties(	config->getScreenWidth(),
									config->getScreenHeight(),
									config->getScreenBpp() );
    }
  
#ifndef WIN32
  sock->connectServer();
#endif   
  
  // Open the screen
  display->openScreen();
  
  glutMainLoop();
  
  // Call ending functions
  display->closeScreen();
  
  // Clean up
  delete error;
  delete debug;
  delete sock;
  delete display;
  delete landscape;
  delete input;
  delete config;
  delete menu;
  delete scene;
  delete perlin;
  
  // Exit program
  return 0;
}
