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

extern sgCoord tuxpos;
extern Object *tuxi;


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
		 if (ob->id == id) {
			ob->moveTo(x, y, h);
			found = 1;
			break;
		 }
		 
		 ob = ob->next;
	  }
	  
	  if (!found) {
		 error->put (ERROR_WARNING, "Object %d not found!", id);
		 break;
	  }
	  
	  if (ob == tuxi)
		{
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
		   for (j = -4; j < 4; j++) {
			  for (i = -4; i < 4; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x +128+ i*256, (int)y +128+ j*256)->selectLOD(2);
				 
			  }
		   }
		   
		   for (j = -2; j < 2; j++) {
			  for (i = -2; i < 2; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x +128+ i*256, (int)y +128+ j*256)->selectLOD(1);
				 
			  }
		   }   

		   for (j= - 1; j < 1; j++) {
			  for (i= - 1; i < 1; i++) {
				 
				 Mapquad::root_map->getMapquad(12, (int)x + 128+ i*256, (int)y +128 + j*256)->selectLOD(0);
				 
			  }
		   }		   
		   
		}  
	  break;
	case CMD_OWN_ID:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;

	  while (ob != NULL) {
		 if (ob->id == id) {
			tuxi = ob;
			break;
		 }
	  }
	  break;
	case CMD_QUIT:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {
		 if (ob->id == id) {
			delete ob;
			break;
		 }
		 
		 ob = ob->next;
	  }	  
	  break;
	case CMD_SAY:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {
		 if (ob->id == id) {
			cout << data << endl;
			ob->puhe->setLabel(debug->string("%s", data));
			break;
		 }
		 ob = ob->next;
	  }
	  break;
	case CMD_ADD_OBJECT:
	  if (sscanf(data, "%d %f %f %f %s", &id, &x, &y, &h, file_name) != 5) {
		 error->put (ERROR_WARNING, "Invalid parameters to protocol command CMD_ADD_OBJECT.");
		 break;
	  }
	  
	  ob = new Object();
	  
	  ob->init(id, file_name);
	  
	  sgCoord tmpPos = ob->getPos();

	  tmpPos.xyz[0] = x;
	  tmpPos.xyz[1] = y;
	  
	  tmpPos.hpr[0] = h;
	  tmpPos.hpr[1] = 0;
	  tmpPos.hpr[2] = 0;
	  
	  ob->moveTo(tmpPos);

	  ob->trans->setTransform( &tmpPos );
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
	default:
	  error->put (ERROR_WARNING, "Unknown protocol command received: 0x%X", command);
	  break;
   }

}
