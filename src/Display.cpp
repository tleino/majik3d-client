#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Display::Display()
{
   width = 640;
   height = 480;
   bpp = 32;
   sdlmesactx = NULL;
   
   #ifdef DEBUG
	 debug->put("Display constructor");
   #endif
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;
   sdlmesactx = NULL;

   #ifdef DEBUG
	 debug->put("Display constructor: width=%d height=%d bpp=%d", width, height, bpp);
   #endif
}

Display::~Display()
{
   if(sdlmesactx != NULL)
	 closeScreen();
   
   #ifdef DEBUG
	 debug->put("Display destructor");
   #endif
}

void
Display::openScreen()
{
   unsigned long flags;
   GLenum format;
   SDL_Surface *screen;
   
   #ifdef DEBUG
	 debug->put("Opening screen...");
   #endif
   
   flags = SDL_SWSURFACE;
   flags != SDL_FULLSCREEN;
   flags != SDL_ANYFORMAT;
   if(bpp <= 8) 
	 {
		flags |= SDL_HWPALETTE;
	 }
   
   screen = SDL_SetVideoMode(width, height, bpp, flags);
   if(screen == NULL) 
	 {
		error->put(ERROR_FATAL, "Unable to set video mode");
	 }
   
   sdlmesactx = new SDLMesaContext(screen);
   sdlmesactx->makeCurrent();
   
   #ifdef DEBUG
	 debug->put("Screen opened.");
   #endif
}

void 
Display::closeScreen()
{
   #ifdef DEBUG
	 debug->put("Closing screen...");
   #endif
	 
   if(sdlmesactx != NULL)
	 {
		delete sdlmesactx;
		sdlmesactx = NULL;
	 }
      
   #ifdef DEBUG
	 debug->put("Screen closed.");
   #endif
}

void 
Display::updateScreen()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   landscape->drawLandscape();
   /* INSERT HERE: Draw menus etc */
   
   SDL_Flip(sdlmesactx->surface);
}
