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

#include <ssg.h>
#include <sg.h>
#include <pu.h>

class Object;
class Player;
class mcSky;
class mcCamera;
class Landscape;

/** A scene class. This class handles the 3D scene. */

class Scene
{
public:
					Scene			();
					~Scene			();
		void		addSpecial		(float x, float y, char *model, bool cutout);
		void		draw			();
		void		update			();
		void		init			();
		void		drawText		(Object *, sgVec3);
		void		redrawSky		();
		float		getHOT			(float x, float y);
		mcCamera*	getCamera		()	{ return m_camera; }
		mcSky*		getSky			()	{ return m_sky; }
		ssgRoot*	getSceneRoot	()	{ return scene_root; }
  const Landscape*	getLandscape	()	{ return m_landscape; }
		Player*		getPlayer		()	{ return m_player; }

private:
	sgMat4         frustumi;
	ssgRoot*	scene_root;
	ssgEntity*	sky_entity;
	ssgTransform  *sky_dome;
	mcSky*		m_sky;
	Landscape*	m_landscape;
	mcCamera*	m_camera;
	Player*		m_player;
};

extern Scene *scene; // PRRRRRRRRRR!!

#endif /* __SCENE_HPP__ */