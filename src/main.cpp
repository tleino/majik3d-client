#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include "Majik.hpp"

int Object::lastId = 0;
bool quit = false;
SDL_Event event;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;
Keyboard *keyboard = NULL;

int
FilterEvents(const SDL_Event *event)
{
   #ifdef DEBUG
	 switch(event->type)
	 {
	  case SDL_QUIT:
		Debug("Quit request sent, closing down...");
		break;
	  case SDL_KEYDOWN:
		Debug("Key pressed: %c", SDL_SymToASCII(&event->key.keysym, NULL));
		break;
	  case SDL_MOUSEMOTION:
		Debug("Mouse moved to: %dx %dy", event->motion.x, event->motion.y);
		break;
	 }
   #endif
   return 1;
}

void
main(int argc, char **argv)
{
   int c;
      
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
   sock = new Socket;
   display = new Display;
   landscape = new Landscape;
   keyboard = new Keyboard;
   
   // Initialize the SDL library
   if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	 Error(ERROR_FATAL, "Unable to init SDL");
	   
   // Automate the SDL_Quit -call when ending the program
   atexit(SDL_Quit);
      
   // Open the screen
   display->openScreen();
   
   // Set event filter and event states
   SDL_SetEventFilter(FilterEvents);
   SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
   SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
   
   // Start main loop
   while(quit == false)
	 {
		// Poll events
		while(SDL_PollEvent(&event))
		  {
			 // Act according to event type
			 switch(event.type)
			   {
				case SDL_QUIT:
				  quit = true;
				  break;
				case SDL_KEYDOWN:
				  keyboard->keyPressed(SDL_SymToASCII(&event.key.keysym, NULL));
				  break;
				default:
				  break;
			   }
		  }
		
		// End event poll
		display->updateScreen();
	 }
   // End main loop
   
   // Call ending functions
   display->closeScreen();
   
   // Clean up
   delete sock;
   delete display;
   delete landscape;
   delete keyboard;
   
   // Exit program
   exit(0);
}
