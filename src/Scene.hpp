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

#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <stdio.h>
#include <ssg.h>
#include <sg.h>
#include <pu.h>
#include "Object.hpp"

class Object;

/** A scene class. This class handles the 3D scene. */

class Scene
{
 public:
   Scene();
   ~Scene();
   
   ///
   void addObject(Object *);
   ///
   void removeObject(int);
   ///
   void removeObject(Object *);

   /// Draw the scene to screen.
   void draw();
   /// Update the scene.
   void update();
   /// Initialize the scene.
   void init();

   ///
   void drawText(puText *, sgVec3);
   
   ///

   float getHOT( float, float );
   

//   list<Object*> object_list;
   
   ///
   ssgRoot *scene_root;

   ///
   int initialized;

   
};

extern Scene *scene; 

#endif /* __SCENE_HPP__ */


