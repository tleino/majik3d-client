#ifndef __SDL_Mesa_H__
#define __SDL_Mesa_H__

#include "Majik.hpp"

class SDLMesaContext
{
 public:
   SDLMesaContext();
   SDLMesaContext(SDL_Surface *);
   ~SDLMesaContext();
   void makeCurrent();
   void makeContext(SDL_Surface *);
 public:
   SDL_Surface *surface;
   OSMesaContext context;
};

#endif
