#ifndef __MAJIK_H__
#define __MAJIK_H__

#define DEBUG

#include <GL/osmesa.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Detail.hpp"
#include "Display.hpp"
#include "Object.hpp"
#include "Protocol.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "Input.hpp"
#include "Matrix.hpp"
#include "P3D.hpp"
#include "Perlin.hpp"
#include "Landscape.hpp"
#include "Cursor.hpp"

extern Error *error;
extern Debug *debug;
extern Display *display;
extern Socket *sock;
extern Landscape *landscape;
extern Input *input;
extern bool quit;

#endif
