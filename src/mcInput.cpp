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
#include "mcPlayer.hpp"

#include "mcCamera.hpp"


#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3


int pitch;
sgCoord temppos;

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
Input::keyUp(unsigned char k, int x, int y)
{
	sgCoord pos = scene->getPlayer()->getPos();
	switch(k)
	{
	case '8':
	case '2':
		//stop moving
		sock->writePacket("%d %f %f %f %d", Protocol::CMD_MOVE_DIRECTION, pos.xyz[0], pos.xyz[1], pos.hpr[0], 0);
		scene->getPlayer()->stopMoving();
		break;

	case '4':
	case '6':
		//stop turning
		sock->writePacket("%d %d", Protocol::CMD_TURN, 0);
		scene->getPlayer()->stopTurning();
		break;

	default:
		break;
	};
}

void
Input::keyDown(unsigned char k, int x, int y)
{
	Player *tuxi = scene->getPlayer();
	if (tuxi == NULL && !overlay->inp->isVisible())
		return;

	puKeyboard (k, PU_DOWN);
  
  if (k == '\t')
  {
    if (overlay->inp->isVisible())
		{
		  overlay->inp->rejectInput();
			overlay->inp->hide();
		}
    else
		{
			overlay->inp_command = Protocol::CMD_SAY;
			overlay->inp->reveal();
			overlay->inp->setCursor(0);
			overlay->inp->acceptInput();
		}
	}
  else if (!overlay->inp->isVisible()) 
  {
	  if (tuxi) //FIXME: remove this from somewhere
		tuxi->unLockMovement();

		sgCoord pos;
		pos = tuxi->getPos();

		switch (k)
		{
		case 'w':
			display->switchWireframe();
			break;
		case 0x03:

		case 'x':
			// Exit.
			exit ( 0 ) ; // FIXME: is there a glutExit function?
			break;		// because this isn't very nice..
		case '8':
			// Move forward.
			sock->writePacket("%d %f %f %f %d", Protocol::CMD_MOVE_DIRECTION, pos.xyz[0], pos.xyz[1], pos.hpr[0], 1);
			scene->getPlayer()->moveForward();
			break;
	
		case '2':
	  // Move backward.
			sock->writePacket("%d %f %f %f %d", Protocol::CMD_MOVE_DIRECTION, pos.xyz[0], pos.xyz[1], pos.hpr[0], -1);
			scene->getPlayer()->moveBackward();
			break;

		case '6':
			// turn right
			sock->writePacket("%d %d", Protocol::CMD_TURN, 1);
			scene->getPlayer()->turnRight();
			break;

		case '4':
		  // Turn left.
			sock->writePacket("%d %d", Protocol::CMD_TURN, -1);
			scene->getPlayer()->turnLeft();
			break;

		case '+':
		  // Look up.
			scene->getCamera()->pitch(1.0);
		  break;

		case '-':
		  // Look down.
			scene->getCamera()->pitch(-1.0);
		  break;

		case 13:
		  break;
		default:
			error->put(mcError::ERROR_WARNING, "Unsupported key received: %d at (%d,%d)",
				 k, x, y);
		  break;
		}      
    }
}

void
Input::specialUp(int key, int x, int y)
{
  puKeyboard (key + PU_KEY_GLUT_SPECIAL_OFFSET, PU_UP);

  switch (key)
    {
    case GLUT_KEY_UP:
      input->keyUp('8', x, y);
      break;
    case GLUT_KEY_DOWN:
      input->keyUp('2', x, y);
      break;
    case GLUT_KEY_LEFT:
      input->keyUp('4', x, y);
      break;
    case GLUT_KEY_RIGHT:
      input->keyUp('6', x, y);
      break;
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
      glutWarpPointer (display->getWidth()/2, display->getHeight()/2);
	  scene->getCamera()->setMode(mcCamera::FIRSTPERSON);
      config->setCameraMode(1);
	  scene->getPlayer()->setVisible(false);
      break;
    case GLUT_KEY_F1:
      glutSetCursor (GLUT_CURSOR_INHERIT);
      glutWarpPointer (display->getWidth()/2, display->getHeight()/2);
	  scene->getCamera()->setMode(mcCamera::BEHIND);
      config->setCameraMode(0);
	  scene->getPlayer()->setVisible(true);
      break; 
    case GLUT_KEY_F12:
      break;

    case GLUT_KEY_UP:
      input->keyDown('8', x, y);
      break;
    case GLUT_KEY_DOWN:
      input->keyDown('2', x, y);
      break;
    case GLUT_KEY_LEFT:
      input->keyDown('4', x, y);
      break;
    case GLUT_KEY_RIGHT:
      input->keyDown('6', x, y);
      break;
    default:
		error->put(mcError::ERROR_WARNING, "Unsupported special key received: %d at " \
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
	// time treshold would be better
	const static float turnTreshold = 15.f;
	static float lastHeading = 0.f;

  puMouse (x, y);

  float diff_x = (input->mouse_x - x) * (360.0f / (display->getWidth() / 2));
  float diff_y = (input->mouse_y - y) * (360.0f / (display->getHeight() / 2));

  Player *tuxi = scene->getPlayer();
  if (tuxi != NULL && config->getCameraMode() != 0)
    {
	  float h = tuxi->getPos().hpr[0];
      
      h += diff_x;
      scene->getCamera()->pitch(diff_y);
      
      while (h > 360.0f)
		h -= 360.0f;
      while (h < 0.0f)
		h += 360.0f;

	  tuxi->setHeading(h);
	  if (fabs(lastHeading - h) > turnTreshold)
	  {
		  sock->writePacket("%d %f", Protocol::CMD_HEADING, h);
		  lastHeading = h;
	  }

      if (x >= display->getWidth()-1 || y >= display->getHeight()-1 || x <= 0 || y <= 0)
	{
	  input->mouse_x = display->getWidth()/2;
	  input->mouse_y = display->getHeight()/2;
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
  if (overlay->inp_command == Protocol::CMD_LOGIN)
	  overlay->inp_command = Protocol::CMD_SAY;
}
