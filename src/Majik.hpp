#ifndef __MAJIK_H__
#define __MAJIK_H__

#define DEBUG

#include "SDL/SDL.h"
#include "GL/osmesa.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "SDLMesa.hpp"
#include "Detail.hpp"
#include "Display.hpp"
#include "Object.hpp"
#include "Protocol.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "Keyboard.hpp"
#include "Matrix.hpp"
#include "P3D.hpp"
#include "Perlin.hpp"
#include "Landscape.hpp"

extern Error *error;
extern Debug *debug;
extern Display *display;
extern Socket *sock;
extern Landscape *landscape;
extern Keyboard *keyboard;
extern SDL_Event event;
extern bool quit;

#endif
