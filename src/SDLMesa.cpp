#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

SDLMesaContext::SDLMesaContext()
{
   surface = NULL;
   context = NULL;
}

SDLMesaContext::SDLMesaContext(SDL_Surface *buffer)
{
   makeContext(buffer);
}

SDLMesaContext::~SDLMesaContext()
{
   if(surface != NULL)
	 free(surface);
   if(context != NULL)
	 OSMesaDestroyContext(context);
}

void SDLMesaContext::makeContext(SDL_Surface *buffer)
{
   GLenum format;
   
   if(SDL_MUSTLOCK(buffer))
	 {
		Error(ERROR_FATAL, "Surface is not safe for writes (%s)",
			  "surface requires locking");
	 }
      
   format = -1;
   switch(buffer->format->BytesPerPixel)
	 {
	  case 1:
		format = OSMESA_COLOR_INDEX;
		break;
	  case 4:
		if((buffer->format->Rmask == 0x00FF0000) &&
		   (buffer->format->Gmask == 0x0000FF00) &&
		   (buffer->format->Bmask == 0x000000FF))
		  {
			 format = OSMESA_RGBA;
		  }
		else if((buffer->format->Rmask == 0xFF000000) &&
				(buffer->format->Gmask == 0x00FF0000) &&
				(buffer->format->Bmask == 0x0000FF00))
		  {
			 format = OSMESA_RGBA;
		  }
		else if((buffer->format->Bmask == 0xFF000000) &&
				(buffer->format->Gmask == 0x00FF0000) &&
				(buffer->format->Rmask == 0x0000FF00))
		  {
			 format = OSMESA_BGRA;
		  }
		break;
	 }
      
   if(format == -1)
	 {
		Error(ERROR_FATAL, "Unknown surface pixel format");
	 }
   
   surface = buffer;
   context = OSMesaCreateContext(format, NULL);
   if(context == NULL)
	 {
		Error(ERROR_FATAL, "OSMesaCreateContext() failed");
	 }
}

void SDLMesaContext::makeCurrent()
{
   if(!OSMesaMakeCurrent(context, surface->pixels, GL_UNSIGNED_BYTE, 
						 surface->w, surface->h))
	 {
		Error(ERROR_FATAL, "OSMesaMakeCurrent() failed");
	 }

   OSMesaPixelStore(OSMESA_Y_UP, 0);
}
