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

#include <iostream.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Menu.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "Socket.hpp"
#include "Display.hpp"
#include "Landscape.hpp"
#include "Input.hpp"
#include "Config.hpp"
#include "Scene.hpp"

#include "Protocol.hpp"
#include "Perlin.hpp"
#include "Mapquad.hpp"


bool quit = false;
Error *error = NULL;
Debug *debug = NULL;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;
Input *input = NULL;
Config *config = NULL;
Menu *menu = NULL;
Scene *scene = NULL;
Protocol *protocol = NULL;
Perlin *perlin = NULL;
Mapquad *Mapquad::root_map = NULL;


int
main(int argc, char **argv)
{      
   // Initialize the necessary global variables as proper objects
   error = new Error;
   debug = new Debug;

   sock = new Socket("195.197.61.60", 4001);

   display = new Display;
   landscape = new Landscape;
   input = new Input;
   config = new Config;
   menu = new Menu;
   scene = new Scene;

   protocol = new Protocol;
   perlin = new Perlin;
   Mapquad::root_map = new Mapquad (NULL, 0, 0, 0);

   
   glutInit(&argc, argv);

   config->readConfig();
   
   // Assign proper width / height 
   if(config->screen_width && config->screen_height && config->bpp)
	 {
		display->width = config->screen_width;
		display->height = config->screen_height;
		display->bpp = config->bpp;
	 }
  
//   sock->writePacket("51\r\n"); /* Log in to the server */

  // sock->connectServer();
#ifndef WIN32
   sock->connectServer();
#endif   
      
   // Open the screen
   display->openScreen();
   
//   Mapquad::root_map->getMapquad(12,0,0);
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
   
   // Exit program
   return 0;
}
