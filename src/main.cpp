#include <iostream.h>
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
Keyboard *keyboard = NULL;

void
main(int argc, char **argv)
{
   int c;
   
   glutInit(&argc, argv);
      
   // Read the command line arguments 
   while(1)
	 {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
	  
		static struct option long_options[] = {
			   { "version", 0, 0, 'V' },
			   { "help", 0,0, 'h' },
			   { 0, 0, 0, 0 },
		  };
		
		c = getopt_long(argc, argv, "hV", long_options, &option_index);
		if(c == -1)
		  break;
		
		switch(c) 
		  {
		   case 'V':
			 printf("%s-%s.%s\n", PACKAGE, VERSION, CPU_VENDOR_OS);
			 exit(0);
		   case '?':
		   case 'h':
			 printf("Usage: %s [option(s)]\n\n  -V, --version  display version information\n  -h, --help     display this text\n", argv[0]);
			 exit(0);
		  }
	 }
      
   // Initialize the necessary global variables as proper objects
   error = new Error;
   debug = new Debug;
   sock = new Socket;
   display = new Display;
   landscape = new Landscape;
   keyboard = new Keyboard;
         
   // Open the screen
   display->openScreen();

   landscape->init();
   
   glutMainLoop();
   
   // Call ending functions
   display->closeScreen();
   
   // Clean up
   delete error;
   delete debug;
   delete sock;
   delete display;
   delete landscape;
   delete keyboard;
   
   // Exit program
   exit(0);
}
