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
#include <pu.h>

#include "Display.hpp"
#include "Debug.hpp"
#include "Landscape.hpp"
#include "Error.hpp"
#include "Input.hpp"

#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3

Input::Input()
{
   DEBUG ("Input constructor");
}

Input::~Input()
{
   DEBUG ("Input destructor");
}

void
Input::keyDown(unsigned char key, int x, int y)
{
   switch (key) {
	case ',':
	  landscape->sun_pos += 2;
	  break;
	case '.':
	  landscape->sun_pos -= 2;
	  break;
	case 'a':
	  landscape->shiftMap_1(DIR_WEST);
	  break;
	case 'd':
      landscape->shiftMap_1(DIR_EAST);
	  break;
	case 's':
      landscape->shiftMap_1(DIR_SOUTH);
	  break;
 case 'w':
	  landscape->shiftMap_1(DIR_NORTH);
	  break;
	default:
	  error->put(ERROR_WARNING, "Unsupported key received: %d at (%d,%d)", key, x, y);
	  exit(0);
	  break;
   }
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
	  landscape->tilt += 2;
	  break;
	case GLUT_KEY_RIGHT:
	  landscape->angle += 2;
	  break;
	case GLUT_KEY_DOWN:
	  landscape->tilt -= 2;
	  break;
	case GLUT_KEY_PAGE_UP:
	  landscape->distance += 2;
	  break;
	case GLUT_KEY_PAGE_DOWN:
	  landscape->distance -= 2;
	  break;
	case GLUT_KEY_HOME:
	  landscape->angle = 75;
	  landscape->tilt = 15;
	  landscape->distance = -20;
	  break;
    case GLUT_KEY_INSERT:
      display->main_menu_bar->reveal();
      break;
    case GLUT_KEY_END:
      display->main_menu_bar->hide();
      break;
	default:
	  error->put(ERROR_WARNING, "Unsupported special key received: %d at (%d,%d)", key, x, y);
	  break;
   }

//   glutPostRedisplay();
}

void
Input::mouseDown(int button, int updown, int x, int y)
{
      puMouse (button, updown, x, y);
      DEBUG (debug->string("mouseDown: %d %d %d %d", button, updown, x, y));
      glutPostRedisplay();
}
