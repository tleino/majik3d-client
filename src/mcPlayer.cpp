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

#include "mcPlayer.hpp"
#include "mcMapquad.hpp"

Player::Player()
{
	sgSetVec3(m_lastRedrawLocation, 0.0, 0.0, 0.0);
	m_movementLock = false;
}

void
Player::moveTo(float x, float y, float h)
{
	Object::moveTo(x, y, h);

	if (sgDistanceSquaredVec3(getPos().xyz, m_lastRedrawLocation) > 10.0)
	{
		sgCopyVec3 (m_lastRedrawLocation, getPos().xyz);

		int i, j;

		for (j= - 4; j < 4; j++)
			for (i= - 4; i < 4; i++)
			{
				Mapquad::root_map->getMapquad(12, (int)x + 256+ i*512, (int)y +256 + j*512);
			}

		Mapquad::root_map->resetBlocks();

		Mapquad::root_map->selectLOD(4, x, y);
		Mapquad::root_map->selectLOD(3, x, y);
		Mapquad::root_map->selectLOD(2, x, y);
		Mapquad::root_map->selectLOD(1, x, y);
		Mapquad::root_map->selectLOD(0, x, y);

		Mapquad::root_map->exchangeBorders();
		Mapquad::root_map->triangulateBlocks();
	}
}

