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
//#include <getopt.h>

#include "Menu.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "Socket.hpp"
#include "Display.hpp"
#include "Landscape.hpp"
#include "Input.hpp"
#include "Config.hpp"

int Object::lastId = 0;
bool quit = false;
Error *error = NULL;
Debug *debug = NULL;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;
Input *input = NULL;
Config *config = NULL;
Menu *menu = NULL;

int
main(int argc, char **argv)
{
   // int c;
      
   // Initialize the necessary global variables as proper objects
   error = new Error;
   debug = new Debug;
   sock = new Socket;
   display = new Display;
   landscape = new Landscape;
   input = new Input;
   config = new Config;
   menu = new Menu;
   
   glutInit(&argc, argv);
   config->readConfig(argc, argv);
/*
   // Read the command line arguments 
   while(1)
	 {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		
		static struct option long_options[] = {
			 { "version", 0, 0, 'V' },
			 { "help", 0, 0, '?' },
			 { "host", 0, 0, 'H' },
			 { "port", 0, 0, 'p' },
		     { "width", 0, 0, 'w' },
		     { "height", 0, 0, 'h' },
		     { "bpp", 0, 0, 'b' },
			 { 0, 0, 0, 0 },
		};
		
		c = getopt_long(argc, argv, "?VH:p:w:h:b:", long_options, &option_index);
		if(c == -1)
		  break;
		
		switch(c) 
		  {
		   case 'V':
			 printf("%s-%s.%s\n", PACKAGE, VERSION, CPU_VENDOR_OS);
			 exit(0);
		   case 'H':
			 printf("HOST = %s\n", optarg);
			 strcpy(config->server_ip, optarg); 
			 break;
		   case 'p':
			 printf("PORT = %s\n", optarg);
			 config->server_port = atoi(optarg);
			 break;
		   case 'w':
			 printf("WIDTH = %s\n", optarg);
			 config->screen_width = atoi(optarg);
			 break;
		   case 'h':
			 printf("HEIGHT = %s\n", optarg);
			 config->screen_height = atoi(optarg);
			 break;
		   case 'b':
			 printf("BPP = %s\n", optarg);
			 config->bpp = atoi(optarg);
			 break;
		   case '?':
			 printf("Usage: %s [option(s)]\n\n  -V, --version  display version information\n  -?, --help  display this text\n", argv[0]);
			 exit(0);
		  }
	 }
*/
   // Assign proper width / height 
   if(config->screen_width && config->screen_height && config->bpp)
	 {
		display->width = config->screen_width;
		display->height = config->screen_height;
		display->bpp = config->bpp;
	 }
   
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
   
   // Exit program
   return 0;
}
