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

#include <iostream>
#include <plib/sg.h>

#include "mcProtocol.hpp"
#include "mcObject.hpp"
#include "mcPlayer.hpp"
#include "mcError.hpp"
#include "mcOverlay.hpp"
#include "mcConfig.hpp"
#include "mcSky.hpp"
#include "mcScene.hpp"
#include "version.hpp"

//extern sgCoord tuxpos;
//extern Player *tuxi;
float meep = 0.0f;
int flag = 0;
int ownId = -1;
sgVec3 tempPos = { 0, 0, 0 };

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

void 
Protocol::parseCommand(char *input)
{
  assert ( input );

//  cout << input << endl;
  
  int command;
  int found = 0, id;
  int map_x, map_y, map_level;
  int direction;
  int major = 0, minor = 0, patch = 0;
  float x, y, h;
  Object *ob = NULL;
  char file_name[80], data[1024];
  
  sscanf(input, "%s\n", data);
  command = atoi(data);
  
  int tmp = strlen(data);
  strcpy(data, &input[tmp+1]);  
  
	switch (command)
	{
	case CMD_REQUIREDVERSION:
		if (sscanf(data, "%d.%d.%d", &major, &minor, &patch) != 3)
		{
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
				"command CMD_REQUIREDVERSION.");
			break;
		}

		if (majik::g_clientVersion[0] < major ||
			(major == majik::g_clientVersion[0] && majik::g_clientVersion[1] < minor) ||
			(major == majik::g_clientVersion[0] && minor == majik::g_clientVersion[1] && majik::g_clientVersion[2] < patch))
			error->put(mcError::ERROR_FATAL, "Your client is too old. You need at least version " \
				"%d.%d.%d.\nNewer client is available from ftp://ftp.majik3d.org/majik3d/\n", 
				major, minor, patch);
		break;

	case CMD_MOVE_DIRECTION:
		if (sscanf(data, "%d %f %f %f %d", &id, &x, &y, &h, &direction) < 5)
		{
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
				"command CMD_MOVE_DIRECTION.");
			break;
		}

		ob = Object::getObject(id);
		if (ob)
		{
			ob->moveTo(x, y, h);
			switch(direction)
			{
			case 1:
				ob->moveForward();
				break;
			case -1:
				ob->moveBackward();
				break;
			case 0:
				ob->stopMoving();
				break;
			default:
				error->put(	mcError::ERROR_WARNING,
							"invalid parameter to MOVE_DIRECTION");
				break;
			}
		}
		// else error object not found

      break;

	  case CMD_TURN:
		if (sscanf(data, "%d %d", &id, &direction) < 2)
		{
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
				"command CMD_TURN.");
			break;
		}
		
		ob = Object::getObject(id);
		if (ob)
		{
			switch(direction)
			{
			case 1:
				ob->turnRight();
				break;
			case -1:
				ob->turnLeft();
				break;
			case 0:
				ob->stopTurning();
				break;
			default:
				error->put(	mcError::ERROR_WARNING,
							"invalid parameter to CMD_TURN");
				break;
			}
		}
		// else error object not found

      break;

	case CMD_HEADING:
		if (sscanf(data, "%d %f", &id, &h) < 2)
		{
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
												"command CMD_HEADING.");
			break;
		}

		ob = Object::getObject(id);
		if (ob)
			ob->setHeading(h);

		break;

	case CMD_MOVE:
		if (sscanf(data, "%d %f %f %f", &id, &x, &y, &h) < 4)
		{
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
												"command CMD_MOVE.");
			break;
		}

		ob = Object::getObject (id);

		if (ob)
		  ob->moveTo(x, y, h);
		else
		{
			error->put (mcError::ERROR_WARNING, "Object %d not found!", id);
			break;
		}
/*	
	if (ob == tuxi)  
	{
		((Player *)tuxi)->unLockMovement();

		sgCoord tuxPos = ob->getPos();

		if (fabs(tempPos[0]-tuxPos.xyz[0]) > 10 || fabs(tempPos[1]-tuxPos.xyz[1]) > 10)
		{
			sgCopyVec3 (tempPos, tuxPos.xyz);

			int i, j;
			
			for (j= - 4; j < 4; j++)
				for (i= - 4; i < 4; i++)
				{
					Mapquad::root_map->getMapquad(12, (int)x + 256+ i*512, (int)y +256 + j*512);
				}

			Mapquad::root_map->resetBlocks();

			Mapquad::root_map->selectLOD(4, x, y);
			Mapquad::root_map->selectLOD(3, x, y);
			Mapquad::root_map->selectLOD(2, x, y);
			Mapquad::root_map->selectLOD(1, x, y);
			Mapquad::root_map->selectLOD(0, x, y);

			Mapquad::root_map->exchangeBorders();
			Mapquad::root_map->triangulateBlocks();
		}

	}
*/
      break;
    case CMD_OWN_ID:
      sscanf(data, "%d", &id);
      
      ownId = id;
      
      break;
    case CMD_QUIT:
      sscanf(data, "%d", &id);
      
      ob = Object::getObject(id);
	  if (ob)
		delete ob;
	  else
		  ;// ERROR - object not found

      break;
    case CMD_SAY:
//      char name[80];
      char *str;
	  str = new char[1000];
      
      if (sscanf(data, "%d %[^\n]", &id, str) < 2)
	  {
			error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
		      "command CMD_SAY.");
			break;
	  }

//      debug->put ("id=%d name=%s str=%s", id, name, str);
      
      ob = Object::getObject(id);
      if (ob)
	  {
	      ob->setSayString(str);
	      ob->revealSayString();
	  }
	  // else ERROR - object not found

      break;
    case CMD_SAYHIDE:
      sscanf(data, "%d", &id);
      
      ob = Object::getObject(id);
      if (ob)
		ob->hideSayString();
	  // else ERROR - object not found

      break;
    case CMD_ADD_OBJECT:
      if (sscanf(data, "%d %f %f %f %s", &id, &x, &y, &h, file_name) != 5)
	{
		  error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
		      "command CMD_ADD_OBJECT.");
	  break;
	}
      
    if (id == ownId)
	{
	  ob = scene->getPlayer();
	}
      else
	ob = new Object();
      
      ob->init(id, file_name);
      
      if (0)//id == ownId)
	{
//	  sock->writePacket("%d %f %f %f", CMD_MOVE, x, y, h); 
	}
      else
	{
	  sgCoord tmpPos = ob->getPos();

	  tmpPos.xyz[0] = x;
	  tmpPos.xyz[1] = y;
	  
	  tmpPos.hpr[0] = h;
	  tmpPos.hpr[1] = 0;
	  tmpPos.hpr[2] = 0;
	  
	  ob->moveTo(tmpPos);
	}
      break;
    case CMD_MAP:
      char tmp[16*16+1];
      
      
      if (sscanf(data, "%d %d %d %s", &map_level, &map_x, &map_y, tmp) !=4)
	{
	  error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
		      "command CMD_MAP.");
	  break;
	}

      FILE *fp;
      char buf[80];
      sprintf (buf, "cache/%01x%05x%05x.map", map_level, map_x, map_y);
      fp = fopen(buf, "w");
      fprintf (fp, "%s", tmp);
      fclose (fp);
      
//      Mapquad::Map_data map_data;
      
  //    map_data.terrain = tmp;
      
//      Mapquad::root_map->getMapquad(map_level, map_x, map_y)->setMap(map_data);
      break;
    case CMD_PROMPT:
      // The data written to the prompt is handled in the input object's
      // callback function.
      sscanf(data, "%d", &overlay->inp_command);
      if (overlay->inp_command == CMD_LOGIN)
	overlay->status_text->setLabel("By what name do you wish to be known?");
      overlay->inp->reveal();
      break;
	case CMD_SUN_POS:
	
      float heading, pitch, luminance, turbidity;
      if (sscanf (data, "%f %f %f %f", &heading, &pitch, &luminance,
		  &turbidity) == 4)
	{
	  //printf ("CMD_SUN_POS %f %f %f %f\n", heading, pitch, luminance,
	  //	  turbidity);

	  scene->getSky()->setLuminanceFactor(luminance);
	  scene->getSky()->setSunPosition(heading, pitch); 
	  scene->getSky()->setTurbidity(turbidity);
	  scene->redrawSky();
	}
      else
	error->put (mcError::ERROR_WARNING, "Invalid parameters to protocol " \
		    "command CMD_SUN_POS.");
      break;
    default:
      error->put (mcError::ERROR_WARNING, "Unknown protocol command received: 0x%X",
		  command);
      break;
    }  
}
