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

#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <stdio.h>
#include <ssg.h>
#include <sg.h>
#include <pu.h>

#include "mcObject.hpp"

class Object;

/** A scene class. This class handles the 3D scene. */

class Scene
{
public:
  Scene();
  ~Scene();
  
  /// FIXME: Unimplemented.
  void addObject(Object *);
  /// FIXME: Unimplemented.
  void removeObject(int);
  /// FIXME: Unimplemented.
  void removeObject(Object *);
  /// Add a special object that doesn't be in any list, a tree for example.
  void addSpecial(float x, float y, char *model, bool cutout);
  /// Draw the scene to screen.
  void draw();
  /// Update the scene.
  void update();
  /// Initialize the scene.
  void init();
  /// Draw the chat-string to screen.
  void drawText(Object *, sgVec3);
  /// Get Height of Terrain of (x,y).
  float getHOT( float x, float y);
  /// A pointer to scene's root node.
  ssgRoot *scene_root;
};

extern Scene *scene; 

#endif /* __SCENE_HPP__ */


