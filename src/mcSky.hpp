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

#ifndef __SKY_HPP__
#define __SKY_HPP__

#include <vector>
#include <sg.h>
#include <ssg.h>


struct SKYPOINT;

class mcSky
{
 private:
   sgVec3 	m_zenith;
   
   bool 	m_skyOk;			/* Has sky been calculated? */
   float 	m_turbidity;
   float 	m_luminancefactor;
   sgVec3 	m_sun;				/* Vector to sun */
   
   float 	m_aSunZenith;		/* Angle between sun and zenith */
  
   int 		m_cSkyPoints,
	 		m_skyWidth,			/* Number of horizontal segments */
	 		m_skyHeight;		/* Number of vertical segments */
   SKYPOINT* m_sky;				/* Sky coordinate data */
   
   float   	m_perez_Y[5],		/* Perez-function parameters */
	 		m_perez_x[5],
	 		m_perez_y[5];
   
   float	latitude, longitude,
	 		day, stndtime;
  
   // Light intensity and colour at zenith
   float	m_zenithY, m_zenithx, m_zenithy;
   
   std::vector<sgVec4 *>	m_scolors;			/* Vertex arrays */
   std::vector<sgVec3 *>	m_scoords;
   std::vector<ssgLeaf *>	m_strips;			/* Triangle strips */
   ssgBranch *m_skydome;

   ssgSimpleState *m_state;
  
  /* Private methods */
  
   void    	createSGVec(sgVec3 &v, float heading, float pitch);
   void     createSphere(int x_segs, int y_segs);
   void 	calculateSunAngles();
   void		calculatePerezCoeffs();
   void		calculateZenithValues();
   float	perez(float *params, float a_viewzenith, float a_sunview, float a_sunzenith);
   void		calculateSkyColors();
   void    	createSkyPoint(SKYPOINT *v, float heading, float pitch);
   void		xyYtoRGB(SKYPOINT *v, float x, float y, float Y);
  
 public:
   mcSky();
   mcSky(int x_segs, int y_segs);
   ~mcSky();
   
   void		setLuminanceFactor(float f);
   void		setTurbidity(float t);
   void    	setSunPosition(sgVec3 &v);
   void		setSunPosition(float heading, float elevation);
 
   ssgEntity * Draw();
};

#endif /* __SKY_HPP__ */
