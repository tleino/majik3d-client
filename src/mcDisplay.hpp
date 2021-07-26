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

#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__

#include <plib/pu.h>

/** A display class. This class handles all the things to do with the
    display, opening window, reshaping, updating and so on. */

class Display
{
public:
				Display				();
				Display				(int width, int height, int bpp);
				~Display			();
									
		void	openScreen			();
		void	closeScreen			();
static	void	updateScreen		();
static	void	resizeScreen		(int width, int height);

		void	setScreenProperties (int width, int height, int bpp)
		{
			m_width = width;
			m_height = height;
			m_bpp = bpp;
		}

		void	setWidth			(int w)	{ m_width = w; }
		void	setHeight			(int h)	{ m_height = h; }
		int		getWidth			()		{ return m_width; }
		int		getHeight			()		{ return m_height; }
		int		getInitialWidth		()		{ return m_initialWidth; }
		int		getInitialHeight	()		{ return m_initialHeight; }
private:

static	void	idle			();

		int		m_bpp;
		int		m_width;
		int		m_height;
		int		m_initialWidth;
		int		m_initialHeight;
		int		m_state;
		bool	m_wireframe;
		float	m_pitch;
		
		float	sun_x, sun_y, sun_z;
		char	stxt[8192*3]; // mikas se taa on?
		
		static bool sceneVisible;
};

extern Display *display;

#endif /* __DISPLAY_HPP__ */
