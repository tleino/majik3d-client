/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MAJIK_HPP__
#define __MAJIK_HPP__

#define DEBUG 1

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Cursor.hpp"
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

extern Error *error;
extern Debug *debug;
extern Display *display;
extern Socket *sock;
extern Landscape *landscape;
extern Input *input;
extern Config *config;
extern bool quit;

#endif /* __MAJIK_HPP__ */
