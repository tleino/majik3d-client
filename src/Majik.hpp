#ifndef __MAJIK_H__
#define __MAJIK_H__

#define DEBUG

#include "SDL/SDL.h"
#include "GL/osmesa.h"
#include "SDLMesa.hpp"
#include "Detail.hpp"
#include "Display.hpp"
#include "Landscape.hpp"
#include "Object.hpp"
#include "Protocol.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Debug.hpp"

extern Display *display;
extern Socket *socket;
extern Landscape *landscape;

#endif
