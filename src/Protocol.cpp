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

#include <iostream.h>
#include <sg.h>

#include "Protocol.hpp"

#include "Menu.hpp"
#include "Socket.hpp"
#include "Debug.hpp"
#include "Object.hpp"
#include "Error.hpp"
#include "Mapquad.hpp"
#include "Display.hpp"
#include "Overlay.hpp"

extern sgCoord tuxpos;
extern Player *tuxi;
float meep = 0.0f;
int flag = 0;
int ownId = -1;

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

void 
Protocol::parseCommand(char *input)
{
   int command;
   int found = 0, id;
   int map_x, map_y, map_level;
   float x, y, h;
   Object *ob = NULL;
   char file_name[80], data[1024];
      
   sscanf(input, "%s\n", data);
   command = atoi(data);
   
   int tmp = strlen(data);
   strcpy(data, &input[tmp+1]);  

   switch (command) {
	case CMD_MOVE:
	  if (sscanf(data, "%d %f %f %f", &id, &x, &y, &h) < 4) {
		 error->put (ERROR_WARNING, "Invalid parameters to protocol command CMD_MOVE.");
		 break;
	  }

	  ob = Object::first;
	  while (ob != NULL) {
		 if (ob->getID() == id) {
			ob->moveTo(x, y, h);
			found = 1;
			break;
		 }
		 
		 ob = ob->getNext();
	  }
	  
	  if (!found) {
		 error->put (ERROR_WARNING, "Object %d not found!", id);
		 break;
	  }
	  
	  if (ob == tuxi)
		{
		   ((Player *)tuxi)->unLockMovement();
		   /* FIXME: Clean up the code. */
		   //		   Mapquad::root_map->getMapquad(12, (int)x-128, (int)y-128)->selectLOD(0);
		   //		   Mapquad::root_map->getMapquad(12, (int)x-128, (int)y+128)->selectLOD(0);
		   //		   Mapquad::root_map->getMapquad(12, (int)x+128, (int)y-128)->selectLOD(0);
		   //		   Mapquad::root_map->getMapquad(12, (int)x+128, (int)y+128)->selectLOD(0);
		   
		   
		   //		   Mapquad *temp = NULL;
		   int i, j;
		   /*
			for (j = -16; j < 16; j++) {
			   for (i = -16; i < 16; i++) {
				  
				Mapquad::root_map->getMapquad(12, (int)x +128+ i*256, (int)y +128+ j*256)->selectLOD(4);
				  
			  }
		   }
		   
		   for (j = -8; j < 8; j++) {
			  for (i = -8; i < 8; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x +128+ i*256, (int)y +128+ j*256)->selectLOD(3);
								  
			  }
		   }
*/
		   Mapquad *temp;
		   
		   for (j = -3; j < 3; j++) {
			  for (i = -3; i < 3; i++) {
				 if ( (j > -3 && j < 2) && (i > -3 && i < 2) )
				   continue;
				 
				 temp = Mapquad::root_map->tryMapquad(12, (int)x +256+ i*512, (int)y +256+ j*512);
				 if (temp != NULL)
				   temp->selectLOD(-1);
			  }
		   }
/*		   
		   for (j = -4; j < 4; j++) {
			  for (i = -4; i < 4; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x +128+ i*256, (int)y +128+ j*256)->selectLOD(2);
				 
			  }
		   }
*/		   
		   for (j = -2; j < 2; j++) {
			  for (i = -2; i < 2; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x +256+ i*512, (int)y +256+ j*512)->selectLOD(1);
				 
			  }
		   }   

		   for (j= - 1; j < 1; j++) {
			  for (i= - 1; i < 1; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x + 256+ i*512, (int)y +256 + j*512)->selectLOD(0);
				 
			  }
		   }		   
		   
		}  
	  break;
	case CMD_OWN_ID:
	  sscanf(data, "%d", &id);
	  
	  ownId = id;

	  break;
	case CMD_QUIT:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {
		 if (ob->getID() == id) {
			delete ob;
			break;
		 }
		 
		 ob = ob->getNext();
	  }	  
	  
	  break;
	case CMD_SAY:
	  char name[80];
	  char str[80];

	  sscanf(data, "%d %s %[^\n]", &id, name, str);
	  debug->put ("id=%d name=%s str=%s", id, name, str);
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {
		 if (ob->getID() == id) {
			debug->put(str);
			ob->setSayString(debug->string("%s: %s", name, str));
			ob->revealSayString();
			break;
		 }
		 ob = ob->getNext();
	  }
	  break;
	case CMD_SAYHIDE:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;
	  
	  while(ob != NULL)
		{
		   if(ob->getID() == id)
			 {
				ob->hideSayString();
				break;
			 }
		   ob = ob->getNext();
		}
	  break;
	case CMD_ADD_OBJECT:
	  if (sscanf(data, "%d %f %f %f %s", &id, &x, &y, &h, file_name) != 5) {
		 error->put (ERROR_WARNING, "Invalid parameters to protocol command CMD_ADD_OBJECT.");
		 break;
	  }
	 
	  if (id == ownId)
		{
		   ob = tuxi = new Player();
		   sock->writePacket("%d %f %f %f", CMD_MOVE, x, y, h);
		}
	  else
		ob = new Object();
	  
	  ob->init(id, file_name);
	  
	  sgCoord tmpPos = ob->getPos();

	  tmpPos.xyz[0] = x;
	  tmpPos.xyz[1] = y;
	  
	  tmpPos.hpr[0] = h;
	  tmpPos.hpr[1] = 0;
	  tmpPos.hpr[2] = 0;
	  
	  ob->moveTo(tmpPos);

//	  ob->trans->setTransform( &tmpPos );
	  break;
	case CMD_MAP:
	  char *tmp;
	  tmp = new char[16*16+1];
	  
	  if (sscanf(data, "%d %d %d %s", &map_level, &map_x, &map_y, tmp) !=4)
		{
		   error->put (ERROR_WARNING, "Invalid parameters to protocol command CMD_MAP.");
		   delete tmp;
		   break;
		}
	  //	  cout << "got map, level: " << map_level << " (" << map_x << "," << map_y << "): " << tmp << endl;
	  FILE *fp;
	  fp = fopen(debug->string("cache/%01x%05x%05x.map", map_level, map_x, map_y), "w");
	  fprintf (fp, "%s", tmp);
	  fclose (fp);
	  Mapquad::root_map->getMapquad(map_level, map_x, map_y)->setMap(tmp);	  
	  break;
	case CMD_PROMPT:
	  sscanf(data, "%d", &overlay->inp_command);
	  overlay->inp->reveal(); ///The data written to the prompt is handled in
	                          ///the input object's callback function
   	  break;
	case CMD_SUN_POS:
	  int mod;
	  float r_add, g_add, b_add;
	  sscanf (data, "%d", &mod);
	  mod = mod % 40;
	  if (mod == 0) {
		flag = 0;
	  }
	  
	  if (flag == 1 || mod > 20) {
		 mod -= (mod-20)*2;
		 flag = 1;
	  }

	  sgVec3 sunposn ;
	  sgVec4 sunamb  ;
	  sgSetVec3 ( sunposn, -1.0f+(float) mod/10.0f, 0.0f, 0.1f ) ;
	  sgSetVec4 ( sunamb , 1.0f, 1.0f, 1.0f, 1.0f ) ;
	  ssgGetLight ( 0 ) -> setPosition ( sunposn ) ;
	  ssgGetLight ( 0 ) -> setColour ( GL_AMBIENT_AND_DIFFUSE, sunamb ) ;
	  r_add = (float) mod/20.0f;
	  g_add = (float) mod/20.0f;
	  b_add = (float) mod/20.0f;
	  r_add = r_add/1.6f;
	  g_add = g_add/1.3f;
	  b_add = b_add;
	  if (r_add > 0.4f)
		r_add = 0.4f;
	  if (g_add > 0.7f)
		g_add = 0.7f;
	  if (b_add > 1.0f)
		b_add = 1.0f;
	  sgVec4 skycol ; sgSetVec4 ( skycol, 0.0f+r_add, 0.0f+g_add, 0.0f+b_add, 1.0f ) ;
//	  glFogf ( GL_FOG_DENSITY, 0.035f / 100.0f ) ;
	  glFogfv( GL_FOG_COLOR  , skycol    ) ;
//	  glFogf ( GL_FOG_START  , 3500.0       ) ;
//	  glFogf ( GL_FOG_END    , 5000.0      ) ;
//	  glFogi ( GL_FOG_MODE   , GL_EXP2   ) ;
	  // glHint ( GL_FOG_HINT   , GL_NICEST ) ;
//	  glEnable ( GL_FOG ) ;
	  glClearColor ( skycol[0], skycol[1], skycol[2], skycol[3] ) ;
	  break;
	default:
	  error->put (ERROR_WARNING, "Unknown protocol command received: 0x%X", command);
	  break;
   }

}
