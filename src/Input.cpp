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

#include "Menu.hpp"
#include "Display.hpp"
#include "Debug.hpp"
//#include "Landscape.hpp"
#include "Error.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "Socket.hpp"
#include <fstream.h>


#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3


extern Object       *tuxi;
int pitch;
sgCoord temppos;

void captureScreen();

Input::Input()
{
   DEBUG ("Input constructor");
}

Input::~Input()
{
   DEBUG ("Input destructor");
}

void
Input::keyDown(unsigned char k, int x, int y)
{
 if (tuxi == NULL && !display->inp->isVisible())
	 return;
   
   puKeyboard (k, PU_DOWN);
   
   static int wireframe = 0;
   
   if (tuxi != NULL) {
	  sgCopyVec3 ( temppos.xyz, tuxi->ob_pos.xyz ) ;
	  sgCopyVec3 ( temppos.hpr, tuxi->ob_pos.hpr ) ;
   }
         
   if (k == '\t') {
	  if (display->inp->isVisible()) {
		 display->inp->rejectInput();
		 display->inp->hide();
	  }
	  else {
		 display->inp->reveal();
		 display->inp->setCursor(0);
		 display->inp->acceptInput();
	  }
   } else if (!display->inp->isVisible()) {
	  switch (k) {
	   case 'w':
		 wireframe = ! wireframe ;		 
		 glPolygonMode ( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL ) ;
		 break;
	   case 0x03:
	   case 'x':
		 exit ( 0 ) ;
		 break;
	   case '8':
		 if (tuxi->lock == 1) {
			
			break;
		 }
		 tuxi->ob_pos.xyz[0] -= sin((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		 tuxi->ob_pos.xyz[1] += cos((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		 sock->writePacket(debug->string("50 %d %d %d",
			 (int) tuxi->ob_pos.xyz[0],
			 (int) tuxi->ob_pos.xyz[1],
			 (int) tuxi->ob_pos.hpr[0]));

		 tuxi->movecounter++;
		 temppos.xyz[0] -= sin((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS)*10.0f;
		 temppos.xyz[1] += cos((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS)*10.0;;
				 
		 sock->writePacket(debug->string("50 %f %f %f",
										 (float) temppos.xyz[0],
										 (float) temppos.xyz[1],
										 (float) temppos.hpr[0]));
		 tuxi->lock = 1;
		 break;
	   case '2':
		 tuxi->ob_pos.xyz[0] += sin((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		 tuxi->ob_pos.xyz[1] -= cos((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		 sock->writePacket(debug->string("50 %d %d %d",
										 (int) tuxi->ob_pos.xyz[0],
										 (int) tuxi->ob_pos.xyz[1],
										 (int) tuxi->ob_pos.hpr[0]));
		 tuxi->movecounter++;
		 if (tuxi->lock == 1)
		   break;
		 
		 temppos.xyz[0] += sin((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS)*10.0f;
		 temppos.xyz[1] -= cos((tuxi->ob_pos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS)*10.0f;
		 
		 sock->writePacket(debug->string("50 %f %f %f",
										 (float) temppos.xyz[0],
										 (float) temppos.xyz[1],
										 (float) temppos.hpr[0]));
		 tuxi->lock = 1;

		 break;
	   case '6':
		 if (tuxi->lock == 1)
		   break;
		 
		 temppos.hpr[0] -= 5.0f;
		 if (temppos.hpr[0] < 0)
		   temppos.hpr[0] += 360.0f;
		 
		 sock->writePacket(debug->string("50 %f %f %f",
										 (float) temppos.xyz[0],
										 (float) temppos.xyz[1],
										 (float) temppos.hpr[0]));
		 tuxi->lock = 1;
		 break;
	   case '4':
		 if (tuxi->lock == 1)
		   break;
		 
		 temppos.hpr[0] += 5.0f;
		 if (temppos.hpr[0] > 355.0f)
		   temppos.hpr[0] = 0.0f;
		 
		 sock->writePacket(debug->string("50 %f %f %f",
										 (float) temppos.xyz[0],
										 (float) temppos.xyz[1],
										 (float) temppos.hpr[0]));
		 tuxi->lock = 1;
		 break;
	   case '+':
		 display->pitch += 1.0f;
		 if (display->pitch > 75.0f)
		   display->pitch = 75.0f;
		 break;
	   case '-':
		 display->pitch -= 1.0f;
		 if (display->pitch < -75.0f)
		   display->pitch = -75.0f;
		 break;
	   default:
		 //	  error->put(ERROR_WARNING, "Unsupported key received: %d at (%d,%d)", k, x, y);
		 //	  exit(0);
		 break;
	  }
   }
}

void 
Input::specialDown(int key, int x, int y)
	 {
		puKeyboard (key + PU_KEY_GLUT_SPECIAL_OFFSET, PU_DOWN);
   
   switch(key) {
    case GLUT_KEY_INSERT:
	  if (menu->menuBar->isVisible())
		menu->menuBar->hide();
	  else
      menu->menuBar->reveal();
      break;
	case GLUT_KEY_F2:
	  display->camera = 1;
	  break;
	case GLUT_KEY_F1:
	  display->camera = 0;
	  break;	 
	case GLUT_KEY_F12:
	  captureScreen();
	  break;
	
	default:
	  error->put(ERROR_WARNING, "Unsupported special key received: %d at (%d,%d)", key, x, y);
	  break;
   }
}

void
Input::mouseDown(int button, int updown, int x, int y)
{
      puMouse (button, updown, x, y);
      DEBUG (debug->string("mouseDown: %d %d %d %d", button, updown, x, y));
      glutPostRedisplay();
}


void
captureScreen()
{
   GLubyte *pixels = new GLubyte[display->width*display->height*3];
   
   glReadPixels( 0, 0, display->width, display->height, GL_RGB, GL_UNSIGNED_BYTE, pixels );
   
   ofstream to("screenshot.rgb", ios::binary | ios::app | ios::trunc);
   
   to.write( pixels, display->width*display->height*3);

   to.close();
}
