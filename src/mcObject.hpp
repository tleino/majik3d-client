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

#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <stdio.h>
#include <ssg.h>
#include <pu.h>
#include <iostream.h>


/** An object class. This class contains an interactive object and
    handles all things like moving and removing it. */

class Object
{
public:
  Object();
  virtual  ~Object();
  
  /// Initialize a new object.
  void init(int id , char *file_name);
  /// Called when object moves.
  void makeSound(int); 
  /// For client to get sound made.
  int getCurrentSound();
  /// Move object to a new position.
  virtual void moveTo(float x, float y, float h);
  /// Move object to a new position.
  void moveTo(sgCoord where);
  /// Set the Object's current position.
  /// Get the Object's current position.
  const sgCoord& getPos();
  /// Set the object's scale.
  void setScale( sgVec3 );
  /// Rotate the object X-axis.
  void rotateX ( double );
  /// Return the amount of moves made, used for animation kludge.
  int getMoveCounter();
  
  /// A pointer to next object.
  Object *getNext();
  /// Return the Object ID, OID.
  int getID();
  /// Return the say string object.
  puText *getTextObject() { return puhe; } // a kludge
  /// Return the object's model filename.
  
  char *getFileName();
  
  void setSayString(char *);
  
  void revealSayString();
  
  void hideSayString();
  /// Bounding sphere radius.
  float getRadius();
  /// Bounding sphere center.
  float *getCenter();
  /// Get Y-length.
  float getLenY();
  /// FIXME: A kludge.
  bool hotFixed;

		bool	initialized		() { return m_initialized; }

		void	setHeading		(float);
		void	setMoveSpeed	(float);

  		void	moveForward		();
		void	moveBackward	();
		void	stopMoving		();
		void	turnRight		();
		void	turnLeft		();
		void	stopTurning		();


virtual void	update			(float);

static	Object	*getObject		(int id);
static	Object	*getFirst		()			{ return first; }

private:
	void setPos(sgCoord where);

	float	m_moveSpeed;
	float	m_turnSpeed;

	bool	m_initialized;

	static Object *last;
	static Object *first;

	ssgTransform *trans;
	ssgEntity *obu;
	puText *puhe;  
	char file_name[80];
	int movecounter;
	int id;

	Object *prev;
	Object *next;

	int currentSound;
	sgCoord ob_pos;
	float scaleZ;
	float lenY;
};

extern Object *object;


#endif /* __OBJECT_HPP__ */
