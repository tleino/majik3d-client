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

#include "Scene.hpp"
#include "Debug.hpp"
#include "Landscape.hpp"

Scene::Scene()
{
       
   DEBUG("Scene constructor.");
}


Scene::~Scene()
{
      DEBUG("Scene destructor.");
}


Scene::init()
{
   scene_root = new ssgRoot;
   object_branch = new ssgTransform;

   scene_root->addKid ( object_branch);
   
   ssgModelPath   ( "data" ) ;
   ssgTexturePath ("data");

   for (int i=0;i<20;i++)
	 addObject (new Object);
   
}

Scene::addObject(Object* ob)
{
   object_list.push_back(ob);
      
   object_branch->addKid ( ob-> plop );
      
}

Scene::removeObject(int id)
{

}

Scene::removeObject(Object *ob)
{
   object_branch->removeKid ( ob-> plop );
   delete ob;
}


Scene::update()
{
}

Scene::draw()
{
   static int frame =0;
   
   frame++;
   
   sgCoord campos ;
   
   sgSetCoord ( & campos, 0.0f, 35.0*sin((float)frame/90), 35.0*sin((float)frame/90), 35.0*sin((float)frame/100), 0.0f, 35.0*sin((float)frame/90) ) ;
   
   ssgSetCamera ( & campos ) ;
   
   ssgCullAndDraw ( scene_root ) ;
      
   landscape->draw();

}
