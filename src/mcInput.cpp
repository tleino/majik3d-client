/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik3d.org>
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

#include "mcMenu.hpp"
#include "mcDisplay.hpp"
#include "mcDebug.hpp"
#include "mcError.hpp"
#include "mcInput.hpp"
#include "mcScene.hpp"
#include "mcSocket.hpp"
#include "mcConfig.hpp"
#include "mcOverlay.hpp"
#include "mcProtocol.hpp"

#include <fstream.h>


#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3


extern Player       *tuxi;
int pitch;
sgCoord temppos;


void captureScreen();

Input::Input()
{
  mouse_x = 0;
  mouse_y = 0;

  debug->put ("Input constructor");
}

Input::~Input()
{
  debug->put ("Input destructor");
}

void
Input::keyDown(unsigned char k, int x, int y)
{
  if (tuxi == NULL && !overlay->inp->isVisible())
    return;
  
  puKeyboard (k, PU_DOWN);
  
  static int wireframe = 0;
  
  if (tuxi != NULL)
    {
      sgCoord tuxiPos = tuxi->getPos(); // Avoid multiple calls to getPos
      sgCopyVec3 ( temppos.xyz, tuxiPos.xyz ) ;
      sgCopyVec3 ( temppos.hpr, tuxiPos.hpr ) ;
    }
  
  if (k == '\t')
    {
      if (overlay->inp->isVisible())
	{
	  overlay->inp->rejectInput();
	  overlay->inp->hide();
	}
      else
	{
	  overlay->inp_command = CMD_SAY;
	  overlay->inp->reveal();
	  overlay->inp->setCursor(0);
	  overlay->inp->acceptInput();
	}
    }
  else if (!overlay->inp->isVisible()) 
    {
      switch (k)
	{
	case 'w':
	  // Wireframe.
	  wireframe = ! wireframe ;		 
	  glPolygonMode ( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL ) ;
	  break;
	case 0x03:
	case 'x':
	  // Exit.
	  exit ( 0 ) ;
	  break;
	case '8':
	  // Move forward.
	  if (tuxi->isMovementLocked() == true)
	    return;
	  
	  temppos.xyz[0] += sin((temppos.hpr[0]-180.0f)*
				SG_DEGREES_TO_RADIANS)*10.0f;
	  temppos.xyz[1] -= cos((temppos.hpr[0]-180.0f)*
				SG_DEGREES_TO_RADIANS)*10.0;;
	  
	  sock->writePacket(debug->string("50 %f %f %f",
					  (float) temppos.xyz[0],
					  (float) temppos.xyz[1],
					  (float) temppos.hpr[0]));
	  tuxi->lockMovement();
	  break;
	case '2':
	  // Move backward.
	  if (tuxi->isMovementLocked() == true)
	    return;

	  temppos.xyz[0] -= sin((temppos.hpr[0]-180.0f)*
				SG_DEGREES_TO_RADIANS)*10.0f;
	  temppos.xyz[1] += cos((temppos.hpr[0]-180.0f)*
				SG_DEGREES_TO_RADIANS)*10.0f;
	  
	  sock->writePacket(debug->string("50 %f %f %f",
					  (float) temppos.xyz[0],
					  (float) temppos.xyz[1],
					  (float) temppos.hpr[0]));
	  tuxi->lockMovement();
	  break;
	case '6':
	  // Turn right.
	  if (tuxi->isMovementLocked() == true)
	    return;
	  
	  temppos.hpr[0] -= 10.0f;
	  if (temppos.hpr[0] < 0)
	    temppos.hpr[0] += 360.0f;
	  
	  sock->writePacket(debug->string("50 %f %f %f",
					  (float) temppos.xyz[0],
					  (float) temppos.xyz[1],
					  (float) temppos.hpr[0]));
	  tuxi->lockMovement();
	  break;
	case '4':
	  // Turn left.
	  if (tuxi->isMovementLocked() == true)
	    return;
	  
	  temppos.hpr[0] += 10.0f;
	  if (temppos.hpr[0] > 355.0f)
	    temppos.hpr[0] = 0.0f;
	  
	  sock->writePacket(debug->string("50 %f %f %f",
					  (float) temppos.xyz[0],
					  (float) temppos.xyz[1],
					  (float) temppos.hpr[0]));
	  tuxi->lockMovement();
	  break;
	case '+':
	  // Look up.
	  display->pitch += 1.0f;
	  if (display->pitch > 75.0f)
	    display->pitch = 75.0f;
	  break;
	case '-':
	  // Look down.
	  display->pitch -= 1.0f;
	  if (display->pitch < -75.0f)
	    display->pitch = -75.0f;
	  break;
	case 13:
	  break;
	default:
	  error->put(ERROR_WARNING, "Unsupported key received: %d at (%d,%d)",
		     k, x, y);
	  break;
	}      
    }
}

void 
Input::specialDown(int key, int x, int y)
{
  puKeyboard (key + PU_KEY_GLUT_SPECIAL_OFFSET, PU_DOWN);
  
  switch(key)
    {
    case GLUT_KEY_INSERT:
      if (menu->menuBar->isVisible())
	menu->menuBar->hide();
      else
	menu->menuBar->reveal();
      break;
    case GLUT_KEY_F2:
      glutSetCursor (GLUT_CURSOR_NONE);
      glutWarpPointer (display->width/2, display->height/2);
      config->camera = 1;
      break;
    case GLUT_KEY_F1:
      glutSetCursor (GLUT_CURSOR_INHERIT);
      glutWarpPointer (display->width/2, display->height/2);
      config->camera = 0;
      break; 
    case GLUT_KEY_F12:
      captureScreen();
      break;
      
    default:
      error->put(ERROR_WARNING, "Unsupported special key received: %d at " \
		 "(%d,%d)", key, x, y);
      break;
    }
}

void
Input::mouseDown(int button, int updown, int x, int y)
{
  puMouse (button, updown, x, y);
  debug->put("mouseDown: %d %d %d %d", button, updown, x, y);
}

void 
Input::mouseMotion(int x, int y) 
{ 
  puMouse (x, y);

  float diff_x = (input->mouse_x - x) * (360.0f / (display->width / 2));
  float diff_y = (input->mouse_y - y) * (360.0f / (display->height / 2));

  if (tuxi != NULL && config->camera != 0)
    {
      sgCoord temppos = tuxi->getPos();
      temppos.hpr[0] += diff_x;
      display->pitch += diff_y;
      
      if (temppos.hpr[0] > 355.0f)
	temppos.hpr[0] = 0.0f;
      if (temppos.hpr[0] < 0.0f)
	temppos.hpr[0] += 360.0f;
      if (display->pitch > 360.0f) 
	display->pitch = 360.0f; 
      if (display->pitch < -360.0f) 
	display->pitch = -360.0f; 
      
      tuxi->setPos(temppos);

      if (x >= display->width-1 || y >= display->height-1 || x <= 0 || y <= 0)
	{
	  input->mouse_x = display->width/2;
	  input->mouse_y = display->height/2;
	  glutWarpPointer (input->mouse_x, input->mouse_y);

	  return;
	}
    }

  input->mouse_x = x; 
  input->mouse_y = y;
}

void
Input::promptDown (puObject *o)
{
  char *val = NULL;
  o->getValue (&val);
  debug->put ("got val: %s", val);
  if (strlen(val) < 1)
    return;
  
  sock->writePacket ("%d %s", overlay->inp_command, val);
  overlay->inp->setValue ("");
  overlay->inp->setLabel ("");
  overlay->inp->acceptInput  () ;
  overlay->inp->setCursor ( 0 ) ;
  overlay->inp->hide();
  if (overlay->inp_command == CMD_LOGIN)
    overlay->inp_command = CMD_SAY;
}

// FIXME: This isn't working and isn't inside a class either.
void
captureScreen()
{
  /*  GLubyte *pixels = new GLubyte[display->width*display->height*3];
  
  glReadPixels( 0, 0, display->width, display->height, GL_RGB,
		GL_UNSIGNED_BYTE, pixels );
  ofstream to("screenshot.rgb", ios::binary | ios::app | ios::trunc);
  to.write( pixels, display->width*display->height*3);
  to.close(); */
}
