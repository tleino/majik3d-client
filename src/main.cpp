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
#include <getopt.h>
#include "Majik.hpp"

int Object::lastId = 0;
bool quit = false;
Error *error = NULL;
Debug *debug = NULL;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;
Input *input = NULL;
Config *config = NULL;

void
main(int argc, char **argv)
{
   int c;

   // Initialize the necessary global variables as proper objects
   error = new Error;
   debug = new Debug;
   sock = new Socket;
   display = new Display;
   landscape = new Landscape;
   input = new Input;
   config = new Config;
   
   glutInit(&argc, argv);
   
   // Read majikrc
   config->readOptions("majikrc");
   
   // Read the command line arguments 
   while(1)
	 {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		
		static struct option long_options[] = {
			 { "version", 0, 0, 'V' },
			 { "help", 0, 0, 'h' },
			 { "host", 0, 0, 'H' },
			 { "port", 0, 0, 'p' },
			 { 0, 0, 0, 0 },
		};
		
		c = getopt_long(argc, argv, "hVH:p:", long_options, &option_index);
		if(c == -1)
		  break;
		
		switch(c) 
		  {
		   case 'V':
			 printf("%s-%s.%s\n", PACKAGE, VERSION, CPU_VENDOR_OS);
			 exit(0);
		   case 'H':
			 printf ("HOST = %s\n", optarg);
			 break;
		   case 'p':
			 printf ("PORT = %s\n", optarg);
			 break;
		   case '?':
		   case 'h':
			 printf("Usage: %s [option(s)]\n\n  -V, --version  display version information\n  -h, --help     display this text\n", argv[0]);
			 exit(0);
		  }
	 }

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
   exit(0);
}
