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

#include "Debug.hpp"
#include "Object.hpp"


Object::Object()
{
   plop = new ssgTransform;
   
   ssgEntity *tux_obj = ssgLoadAC ( "tuxedo.ac"   ) ;
   plop->addKid (tux_obj);

     ssgFlatten         ( tux_obj  ) ;
     ssgStripify        ( plop  ) ;
   
   
   sgCoord tuxpos ;
   sgSetCoord ( & tuxpos, (random() % 10 ) - 7,  random() % 100, random() % 10, 0, 0.0f, 0.0f ) ;
   
   plop -> setTransform ( &tuxpos );
   
//   scene->addObject(this);
      
   DEBUG (debug->string("Object constructor"));
}

Object::~Object()
{
//   scene->removeObject(this);
   DEBUG (debug->string("Object destructor"));
}

/*
Object::draw()
{


}
*/
