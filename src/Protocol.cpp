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

#include "Protocol.hpp"
#include "Menu.hpp"
#include "Socket.hpp"
#include <iostream.h>
#include "Debug.hpp"
#include "Object.hpp"
#include "Display.hpp"
#include <sg.h>

extern sgCoord tuxpos;
extern Object   *tuxi;

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
   int found, id;
   float x, y, h;
   float new_x, new_y, new_h;
   float old_x, old_y, old_h;
   float dif_x, dif_y, dif_h;
   Object *ob = NULL;
   char file_name[80], data[1024];
   
   sscanf(input, "%s", data);
   command = atoi(data);
   
   int tmp = strlen(data);
   
   strcpy(data, &input[tmp+1]);  

   switch (command) {
	case 50:
	  if (sscanf(data, "%d %f %f %f", &id, &x, &y, &h) < 4) {
		 cout << "ERROR: invalid parameters to 50" << endl;
		 break;
	  }
	  
	  found = 0;
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {		 
		 if (ob->id == id) {
			/*
			new_x = x;
			new_y = y;
			new_h = h;
			old_x = ob->ob_pos.xyz[0];
			old_y = ob->ob_pos.xyz[1];
			old_h = ob->ob_pos.hpr[0];
			dif_x = new_x-old_x;
			dif_y = new_y-old_y;
			dif_h = new_h-old_h;
			
			int counter = 0;
			
			while (1) {
			   counter++;
			   			   
			   if (dif_h) {
				  old_h += dif_h/5.0f;
				  ob->ob_pos.hpr[0] = old_h;
			   }
			   if (dif_x) {
				  old_x += dif_x/5.0f;
				  ob->ob_pos.xyz[0] = old_x;
			   }
			   if (dif_y) {
				  old_y += dif_y/5.0f;
				  ob->ob_pos.xyz[1] = old_y;
			   }
			   
			   if (counter == 5) {
				  ob->ob_pos.xyz[0] = x;
				  ob->ob_pos.xyz[1] = y;
				  ob->ob_pos.hpr[0] = h;
				  break;
			   }
			   else {
				  ob->movecounter++;
				  glutPostRedisplay();
				  //display->updateScreen();
			   }
			}
*/
			ob->ob_pos.xyz[0] = x;
			ob->ob_pos.xyz[1] = y;
			ob->ob_pos.hpr[0] = h;
			ob->movecounter++;
			found = 1;
			ob->lock = 0;
			break;
		 }
			 
		 ob = ob->next;
	  }
	  
	  if (!found) {
		 cout << "Object " << id << " not found!" << endl;
		 break;
	  }
	  
	  break;
	case 52:
	  sscanf(data, "%d", &id);
	  
	  ob = Object::first;
	  
	  while (ob != NULL) {
		 if (ob->id == id) {
			tuxi = ob;
			break;
		 }
	  }
	  break;
	case 53:
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
	  
	case 54:
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
	  
	case 55:
	  if (sscanf(data, "%d %f %f %f %s", &id, &x, &y, &h, file_name) != 5) {
		 cout << "ERROR: Invalid parameters to 55" << endl;
		 break;
	  }
	  
	  ob = new Object();
	  
	  ob->init(id, file_name);
	  
	  ob->ob_pos.xyz[0] = x;
	  ob->ob_pos.xyz[1] = y;
	  
	  ob->ob_pos.hpr[0] = h;
	  ob->ob_pos.hpr[1] = 0;
	  ob->ob_pos.hpr[2] = 0;
	  
	  ob->trans->setTransform( &ob->ob_pos );
	  
	  break;
	default:
	  break;
   }
}
