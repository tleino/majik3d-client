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
	 Debug("Display constructor");
   #endif
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;
   sdlmesactx = NULL;

   #ifdef DEBUG
	 Debug("Display constructor: width=%d height=%d bpp=%d", width, height, bpp);
   #endif
}

Display::~Display()
{
   #ifdef DEBUG
	 Debug("Display destructor");
   #endif
}

void Display::openScreen()
{
   unsigned long flags;
   GLenum format;
   SDL_Surface *screen;
   
   #ifdef DEBUG
	 Debug("Opening screen...");
   #endif
   
   flags = SDL_SWSURFACE;
   flags |= SDL_FULLSCREEN;
   flags != SDL_ANYFORMAT;
   if(bpp <= 8) 
	 {
		flags |= SDL_HWPALETTE;
	 }
   
   screen = SDL_SetVideoMode(width, height, bpp, flags);
   if(screen == NULL) 
	 {
		Error(ERROR_FATAL, "Unable to set video mode");
	 }
   
   sdlmesactx = new SDLMesaContext(screen);
   sdlmesactx->makeCurrent();
   
   #ifdef DEBUG
	 Debug("Screen opened.");
   #endif
}

void Display::closeScreen()
{
   #ifdef DEBUG
	 Debug("Closing screen...");
   #endif
	 
   if(sdlmesactx != NULL)
	 {
		delete sdlmesactx;
		sdlmesactx = NULL;
	 }
      
   #ifdef DEBUG
	 Debug("Screen closed.");
   #endif
}

void Display::updateScreen()
{
   landscape->drawLandscape();
   SDL_Flip(sdlmesactx->surface);
}
