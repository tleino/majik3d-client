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

Input::Input()
{
   #ifdef DEBUG
	 debug->put("Input constructor");
   #endif
}

Input::~Input()
{
   #ifdef DEBUG
	 debug->put("Input destructor");
   #endif
}

void
Input::keyDown(unsigned char key, int x, int y)
{
   switch (key) {
	default:
	  error->put(ERROR_WARNING, "Unsupported key received: %d at (%d,%d)", key, x, y);
	  break;
   }
   exit(0);
//   glutPostRedisplay();
}

void 
Input::specialDown(int key, int x, int y)
{
   switch(key) {
	case GLUT_KEY_LEFT:
	  landscape->angle -= 2;
	  break;
	case GLUT_KEY_UP:
	  break;
	case GLUT_KEY_RIGHT:
	  landscape->angle += 2;
	  break;
	case GLUT_KEY_DOWN:
	  break;
	case GLUT_KEY_PAGE_UP:
	  break;
	case GLUT_KEY_PAGE_DOWN:
	  break;
	case GLUT_KEY_HOME:
	  landscape->angle = 0;
	  break;
	case GLUT_KEY_END:
	  break;
	default:
	  error->put(ERROR_WARNING, "Unsupported special key received: %d at (%d,%d)", key, x, y);
	  break;
   }

//   glutPostRedisplay();
}

void
Input::mousePassiveMotion(int x, int y)
{
   #ifdef DEBUG
	 debug->put("Mouse moved to: %d %d", x, y);
   #endif
	 
   display->cursor->x_pos = x;
   display->cursor->y_pos = y;   
}
