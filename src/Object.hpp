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

#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <stdio.h>
#include <ssg.h>
#include <pu.h>

#include "Scene.hpp"

class Scene;

class Object
{
 public:
   Object();
   ~Object();

   void init(int, char*);
   
   sgCoord ob_pos;
   
   ssgTransform *trans;
   ssgEntity *obu;
   puText *puhe;

   char file_name[80];
   int movecounter;
   int id;
   Object *prev;
   Object *next;
   static Object *last;
   static Object *first;
};

extern Object *object;

#endif /* __OBJECT_HPP__ */
