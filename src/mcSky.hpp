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

#include <sg.h>
#include <ssg.h>

struct SKYPOINT
{
  sgVec3 xyz;      /* X, Y and Z-coordinates */
  float r, g, b;   /* Red, green, blue */
  
  float a_sun;     /* Angle to sun */
  float a_zenith;  /* Angle to zenith */
};

class mcSky
{
 private:
   sgVec3 	zenith;
   
   bool 	sky_ok;			/* Has sky been calculated? */
   float 	turbidity;
   float 	luminancefactor;
   sgVec3 	sun;			/* Vector to sun */
   
   float 	a_sunzenith;	/* Angle between sun and zenith */
  
   int 		cskypoints,
	 		sky_width,		/* Number of horizontal segments */
	 		sky_height;		/* Number of vertical segments */
   SKYPOINT* sky;			/* Sky coordinate data */
   
   float   	perez_Y[5],		/* Perez-function parameters */
	 		perez_x[5],
	 		perez_y[5];
   
   float	latitude, longitude,
	 		day, stndtime;
  
   // Light intensity and colour at zenith
   float	zenith_Y, zenith_x, zenith_y;
   
   sgVec4 *	scolors;
   sgVec3 *	scoords;
  
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
   void		updateSunPosition();
  
 public:
   mcSky();
   mcSky(int x_segs, int y_segs);
   ~mcSky();
   
   void		setLuminanceFactor(float f);
   void		setTurbidity(float t);
   void    	setSunPosition(sgVec3 &v);
   void		setSunPosition(float heading, float elevation);
   void     setTimeInDay(float t);      // [0.0 .. 1.0]
   void     setDayInYear(float d);      // [0.0 .. 1.0]
   void     setLatLong(float lat, float lon);   // radians
  
   ssgEntity * Draw();
};

extern mcSky *mc_sky;

#endif /* __SKY_HPP__ */
