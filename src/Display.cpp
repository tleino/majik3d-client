#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Display::Display()
{
   width = 640;
   height = 480;
   bpp = 32;
   ctx = NULL;
   #ifdef DEBUG
	 Debug("Display constructor");
   #endif
}

Display::Display(int w, int h, int b)
{
   width = w;
   height = h;
   bpp = b;
   ctx = NULL;
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
   
   ctx = new SDLMesaContext(screen);
   ctx->makeCurrent();
}

void Display::closeScreen()
{
   if(ctx != NULL)
	 delete ctx;
}
