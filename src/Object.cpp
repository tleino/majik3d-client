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
#include <iostream>

#include "Debug.hpp"
#include "Object.hpp"
#include "Display.hpp"

Object *Object::first = NULL;
Object *Object::last = NULL;

Object::Object()
{
   if (first == NULL)
	 first = this;
   
   if (last != NULL)
	 prev = last;
   else
	 prev = NULL;
   
   if (prev != NULL)
	 prev->next = this;
   
   last = this;
   next = NULL;

   if (prev != NULL)
	 printf("Object parent: %d\n", prev->id);
   
   DEBUG (debug->string("Object constructor"));
   lock = 0;
}

Object::~Object()
{
   if (prev != NULL)
	 {
		if (next != NULL)
		  prev->next = next;
		else
		  prev->next = NULL;
	 }
   
   if (next != NULL)
	 {
		if (prev != NULL)
		  next->prev = prev;
		else
		  next->prev = NULL;
	 }
		
   if (this == last)
	 {
		if (prev != NULL)
		  last = prev;
		else
		  last = NULL;
	 }
		
   if (this == first)
	 {
		if (next != NULL)
		  first = next;
		else 
		  first = NULL;
	 }

   scene->scene_root->removeKid( trans );
   
   delete trans;
   delete puhe;
   
   DEBUG (debug->string("Object destructor"));
}

void
Object::init(int id, char *file_name)
{
   this->id = id;
   strcpy(this->file_name, file_name);
   
   trans = new ssgTransform;
   
   ssgEntity *pengu = ssgLoadAC(file_name);
   
   trans  -> addKid ( pengu  ) ;
   ssgFlatten         ( pengu  ) ;
   ssgStripify        ( trans  ) ;
   trans  -> clrTraversalMaskBits ( SSGTRAV_HOT ) ;
   
   scene->scene_root -> addKid ( trans ) ;

   puhe = new puText(0, 0);
   puhe->setLabel("");
}

void Object::moveTo(int x, int y, int h)
{
   // Smooth the movement from point a to point b.
   float old_x, old_y, old_h;
   float dif_x, dif_y, dif_h;
   int counter = 0;
   
   old_x = ob_pos.xyz[0];
   old_y = ob_pos.xyz[1];
   old_h = ob_pos.hpr[0];
   dif_x = x-old_x; dif_y = y-old_y; dif_h = h-old_h;
   
   // Disabled for a while.
   while (0) {
	  counter++;
	  
	  if (dif_h) {
		 old_h += dif_h/5.0f;
		 ob_pos.hpr[0] = old_h;
	  }
	  if (dif_x) {
		 old_x += dif_x/5.0f;
		 ob_pos.xyz[0] = old_x;
	  }
	  if (dif_y) {
		 old_y += dif_y/5.0f;
		 ob_pos.xyz[1] = old_y;
	  }
	  
	  if (counter == 5) {
		 ob_pos.xyz[0] = x;
		 ob_pos.xyz[1] = y;
		 ob_pos.hpr[0] = h;
		 break;
	  }
	  else {
		 movecounter++;
		 display->updateScreen();
	  }
   } 
   
   ob_pos.xyz[0] = x;
   ob_pos.xyz[1] = y;
   ob_pos.hpr[0] = h;
   movecounter++;
   lock = 0;
}
