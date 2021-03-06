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

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "mcObject.hpp"

class Player : public Object
{
public:
				Player			();
virtual			~Player			()		{}
  
				
		void	lockMovement	()		{ m_movementLock = true; }
		void	unLockMovement	()		{ m_movementLock = false; }
		bool	isMovementLocked()		{ return m_movementLock; }

private:
virtual void	moveTo			(float x, float y, float h);

	bool	m_movementLock;
};

#endif /* __OBJECT_HPP__ */
