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

#include <stdio.h>
#include <iostream.h>

#include "mcDebug.hpp"
#include "mcObject.hpp"
#include "mcDisplay.hpp"

Object *Object::first = NULL;
Object *Object::last = NULL;

Object::Object()
{
  debug->put("Object constructor");
  
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
    debug->put("Object parent: %d", prev->id);
  
  movecounter = 0;
  hotFixed = false;
}

Object::~Object()
{
  debug->put("Object destructor");
  
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
}

float
Object::getRadius()
{
  sgSphere *bs = trans->getBSphere(); 
  return bs->getRadius() * scaleZ; 
}

float *
Object::getCenter()
{
  sgSphere *bs = trans->getBSphere();
  return (float *) bs->getCenter();
}

float
Object::getLenY()
{
  return lenY;
}

void
Object::init(int id, char *file_name)
{
  assert ( (id > 0) && file_name );

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

  /*if (!strcmp(file_name, "dog3.ac"))
    puhe->setColour (PUCOL_LABEL, 0.0, 0.5, 1.0);
  else if (!strcmp(file_name, "jennifers_alien.ac"))
    puhe->setColour (PUCOL_LABEL, 0.0, 1.0, 1.0);
  else if (!strcmp(file_name, "elephant.ac"))
    puhe->setColour (PUCOL_LABEL, 1.0, 0.5, 0.5);
    else*/
    puhe->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
  
  sgSphere *bs = trans->getBSphere();  
  lenY = bs->getRadius();
}

void 
Object::moveTo(float x, float y, float h)
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
  while (false) {
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
    }
  } 
  
  ob_pos.xyz[0] = x;
  ob_pos.xyz[1] = y;
  ob_pos.hpr[0] = h;
  
  ob_pos.xyz [ 2 ] = scene->getHOT(ob_pos.xyz [ 0 ],
				   ob_pos.xyz [ 1 ] ) + 0.1f;
  ob_pos.hpr [ 1 ] = 0.0f ;
  ob_pos.hpr [ 2 ] = 0.0f ;
  
  movecounter++;
}

void 
Object::moveTo(sgCoord where)
{
  moveTo(where.xyz[0], where.xyz[1], where.hpr[0]);
}

void
Object::setPos(sgCoord where)
{
  sgCopyVec3 (ob_pos.xyz, where.xyz);
  sgCopyVec3 (ob_pos.hpr, where.hpr);
}

sgCoord 
Object::getPos()
{
  // This may need to be explicitly done using a sgCopyVec3?
  // hmm, I guess you're right
  return ob_pos;
}

void
Object::setScale( sgVec3 scale )
{
  sgCoord temp = getPos();
  
  trans->setTransform( &temp, scale[0], scale[1], scale[2] );
  this->scaleZ = scale[2];
}

void
Object::rotateX( double a )
{
  ob_pos.hpr[0] += a;
}

int
Object::getMoveCounter()
{
  return movecounter;
}

int
Object::getID()
{
  return id;
}

Object *
Object::getNext()
{
  return next;
}

void
Object::setSayString(char *str)
{
  assert ( str );
  puhe->setLabel(str);
}

void
Object::revealSayString()
{
  puhe->reveal();
}

void
Object::hideSayString()
{
  puhe->hide();
}

char *
Object::getFileName()
{
  return file_name;
}
