#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "Majik.hpp"

int Object::lastId = 0;
Socket *sock = NULL;
Display *display = NULL;
Landscape *landscape = NULL;

void
main(int argc, char *argv[])
{
   sock = new Socket;
   display = new Display;
   landscape = new Landscape;
   
   if(SDL_Init(SDL_INIT_VIDEO) < 0)
	 {
		Error(ERROR_FATAL, "Unable to init SDL");
	 }
   atexit(SDL_Quit);
      
   display->openScreen();
     
   delete socket;
   delete display;
   delete landscape;
}
