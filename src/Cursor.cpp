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

#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Cursor::Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor constructor");
   #endif
}

Cursor::~Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor destructor");
   #endif
}

void
Cursor::draw()
{
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0, display->width, 0, display->height, -1, 250);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glScalef(1.0f/640.0f, 1.0f/640.0f, 1.0f);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   
   
   
   glDisable(GL_BLEND);
}
