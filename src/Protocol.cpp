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
   int command, len = strlen(input);
   int found, id, x, y, h;
   Object *ob = NULL;
   char file_name[80];
   
   
  //cout << "commandstring lenghtnt: " << input << std::endl;
   
   char data[1024];
   
   sscanf(input, "%s", data);
   
//   cout << "data: " << data << std::endl;
   
   command = atoi(data);
   
   int tmp = strlen(data);
   
   strcpy(data, &input[tmp+1]);
   
     
//  cout << "data: " << data << endl;
   
   switch (command)
	 {
	  case 50:
		if (sscanf(data, "%d %d %d %d", &id, &x, &y, &h) < 4)
		  {
			 cout << "ERROR: invalid parameters to 50" << endl;
			 break;
		  }
		//      printf ("data=%s\n", data);
		//printf ("id=%d x=%d y=%d h=%d\n", id, x, y, h);

		found = 0;
		
		ob = Object::first;
		
		while (ob != NULL)
		  {
			 if (ob->id == id)
			   {
				  //				   sgCoord ob_pos;
				  //  cout << "set pos obu: " << id << "\n";
				  ob->ob_pos.xyz[0] = x;
				  ob->ob_pos.xyz[1] = y;
				  
				  ob->ob_pos.hpr[0] = h;
				  ob->ob_pos.hpr[1] = 0;
				  ob->ob_pos.hpr[2] = 0;
				  
				  ob->trans->setTransform( &ob->ob_pos );
				  found = 1;
				  ob->movecounter++;
				  break;
			   }
			 
			 ob = ob->next;
		  }
		
		if (!found)
		  {
			 cout << "Object " << id << " not found!" << endl;
			 break;
		  }
		//	 cout << "pluuplii3\n";
		
   
		break;
	  case 52:
		sscanf(data, "%d", &id);
		cout << "own id: " << id << endl;

		ob = Object::first;
		
		while (ob != NULL)
		  {
			 if (ob->id == id)
			   {
				  tuxi = ob;
				  break;
			   }
		  }
		break;
	  case 53:
		sscanf(data, "%d", &id);
		
		ob = Object::first;
		
		while (ob != NULL)
		  {
			 if (ob->id == id)
			   {
				  delete ob;
				  break;
			   }
			 
			 ob = ob->next;
		  }	  
		break;
		
	  case 54:
		sscanf(data, "%d", &id);
		
		ob = Object::first;
		
		while (ob != NULL)
		  {
			 if (ob->id == id)
			   {
				  cout << data << endl;
				  ob->puhe->setLabel(debug->string("%s", data));
				  break;
			   }
			 ob = ob->next;
		  }
	
		break;
		
	  case 55:
		if (sscanf(data, "%d %d %d %d %s", &id, &x, &y, &h, file_name) != 5)
		  {
			 cout << id << x << y << h << file_name << endl;
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
	  case 100:
		//cout << "dialog data: " << data << std::endl;
		//menu->mkDialog(debug->string("%s", data));
		sock->writePacket("100 kiitti dialogista!\r\n");
		break;
	  default:
		//cout << "data:" << data << std::endl;
		break;
	 }
//   cout << "protocol done" << endl;
}
		
void 
Protocol::sendCommand()
{
}
