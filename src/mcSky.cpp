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
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include <sg.h>
#include <ssg.h>

#include "mcSky.hpp"

#define DRAW_FILLED
//#define DRAW_POINTS

/**
 * Class constructor
 * 
 * Resets the sky to default settings 
 * */

mcSky::mcSky()
{
  this->zenith[0] = 0.0;	
  this->zenith[1] = 1.0;	
  this->zenith[2] = 0.0;
	
  this->createSphere(16, 16);
  this->setTurbidity(4.0);
  this->setSunPosition(0.0, 0.0);
  this->sky_ok = false;
}

mcSky::mcSky(int x_segs, int y_segs)
{
  this->zenith[0] = 0.0;
  this->zenith[1] = 1.0;
  this->zenith[2] = 0.0;
  this->createSphere(x_segs, y_segs);
  this->setTurbidity(4.0);
  this->setSunPosition(0.0, 0.0);
  this->sky_ok = false;
}

/**
 * Class destructor
 * */

mcSky::~mcSky()
{
  if (this->sky != NULL)
    delete[] sky;
}


/**
 * Class interface functions 
 * **/


/**
 * void mcSky::setTurbidity(float t)
 * 
 * Defines the turbidity (ratio of particles/haze in atmosphere) of the sky
 * */

void mcSky::setTurbidity(float t)
{
  this->turbidity = t;
  this->calculatePerezCoeffs();
  this->calculateZenithValues();
}

/**
 * void mcSky::setSunPosition(sgVec3 &v)
 * 
 * Defines the location of the sun 
 * */

void mcSky::setSunPosition(sgVec3 &v)
{
  this->sun = v;
  this->a_sunzenith = acos(sgScalarProductVec3(this->sun, this->zenith));	
  this->calculateSunAngles();
  this->calculateZenithValues();
}

/**
 * void mcSky::setSunPosition(float heading, float pitch)
 * 
 * Defines the location of the sun in euler angles. 
 * */

void mcSky::setSunPosition(float heading, float pitch)
{
  this->createSGVec(this->sun, heading, pitch);
  this->a_sunzenith = acos(sgScalarProductVec3(this->sun, this->zenith));
  //	printf("SZ: %f\n", this->a_sunzenith);
  this->calculateSunAngles();
  this->calculateZenithValues();
}

/**
 * void mcSky::Draw()
 * 
 * Renders the sky to screen
 * */

ssgBranch * 
mcSky::Draw()
{
  int x, y;
  SKYPOINT *v;
	
  if (!this->sky_ok)
    this->calculateSkyColors();

  ssgSimpleState *state = new ssgSimpleState;
  ssgBranch *skydome = new ssgBranch;

  state -> setShadeModel (GL_SMOOTH);
  state -> enable (GL_CULL_FACE);
	
  /* Draw top fan */

#ifdef DRAW_FILLED
  int ntris = this->sky_width+2;
  sgVec4 *scolors = new sgVec4 [ntris];
  sgVec3 *scoords = new sgVec3 [ntris];

  v = &this->sky[0];

  sgSetVec4 (scolors[0], v->r, v->g, v->b, 0.0f);
  sgSetVec3 (scoords[0], v->xyz[0], v->xyz[1], v->xyz[2]);

  for (x = 0; x < this->sky_width; x++)
    {
      v = &this->sky[x + 1];
      sgSetVec4 (scolors[x + 1], v->r, v->g, v->b, 0.0f);
      sgSetVec3 (scoords[x + 1], v->xyz[0], v->xyz[1], v->xyz[2]);
    }
  v = &this->sky[1];
  sgSetVec3 (scolors[x + 1], v->r, v->g, v->b);
  sgSetVec3 (scoords[x + 1], v->xyz[0], v->xyz[1], v->xyz[2]);

  ssgLeaf *topfan = new ssgVTable (GL_TRIANGLE_FAN,
				   2*ntris, scoords,
				   2*ntris, NULL,
				   2*ntris, NULL,
				   2*ntris, scolors
				   );

  topfan -> setState (state);
  skydome -> addKid (topfan);
	
  /* Draw horizontal strips */
   
  ntris = (this->sky_height-1*3)*(this->sky_width*2);
  scolors = new sgVec4 [ntris];
  scoords = new sgVec3 [ntris];

  for (y = 0; y < this->sky_height - 1; y++)
    {
      v = &this->sky[y * this->sky_width + x + 1];
      sgSetVec4(scolors[y*this->sky_width+x], v->r, v->g, v->b, 0.0f);
      sgSetVec3(scoords[y*this->sky_width+x], v->xyz[0], v->xyz[1], v->xyz[2]);

      for (x = 0; x < this->sky_width; x++)
	{
	  v = &this->sky[(y + 1) * this->sky_width + x + 1];
	  sgSetVec4(scolors[y*this->sky_width+x], v->r, v->g, v->b, 0.0f);
	  sgSetVec3(scoords[y*this->sky_width+x], v->xyz[0], v->xyz[1],
		    v->xyz[2]);
	
	  v = &this->sky[y * this->sky_width + x + 1];
	  sgSetVec4(scolors[y*this->sky_width+x+1], v->r, v->g, v->b, 0.0f);
	  sgSetVec3(scoords[y*this->sky_width+x+1], v->xyz[0], v->xyz[1],
		    v->xyz[2]);
	}	

      v = &this->sky[(y + 1) * this->sky_width + 1];
      sgSetVec4(scolors[y*this->sky_width+x+1], v->r, v->g, v->b, 0.0f);
      sgSetVec3(scoords[y*this->sky_width+x+1], v->xyz[0], v->xyz[1],
		v->xyz[2]);

      v = &this->sky[y * this->sky_width + 1];
      sgSetVec4(scolors[y*this->sky_width+x+2], v->r, v->g, v->b, 0.0f);
      sgSetVec3(scoords[y*this->sky_width+x+2], v->xyz[0], v->xyz[1],
		v->xyz[2]);
    }

  ssgLeaf *hstrip = new ssgVTable (GL_TRIANGLE_STRIP,
				   2*ntris, scoords,
				   2*ntris, NULL,
				   2*ntris, NULL,
				   2*ntris, scolors
				   );
  
  hstrip -> setState (state);
  skydome -> addKid (hstrip);
#endif
	
#ifdef DRAW_POINTS
  glDisable(GL_DEPTH_TEST);
  glBegin(GL_POINTS);
	
  glColor3f(1.0, 1.0, 1.0);
  for (i = 0; i < this->cskypoints; i++)
    {
      v = &this->sky[i];
      glVertex3f(v->xyz[0], v->xyz[1], v->xyz[2]);
    }
  glEnd();
  glEnable(GL_DEPTH_TEST);
#endif	

  return skydome;
}

/**
 * Private methods 
 * */

/**
 * float mcSky::perez(float *params, float a_viewzenith, float a_sunview)
 * 
 * Calculates Perez-function with given parameters
 * */

float mcSky::perez(float *params, float a_viewzenith, float a_sunview)
{
  return (1.0 + params[0] * exp(params[1] / cos(a_viewzenith))) * 
    ((1.0 + params[2] * exp(params[3] * a_sunview) + params[4] * cos(a_sunview) * cos(a_sunview)));
}

/**
 * void mcSky::calculatePerezCoeffs()
 * 
 * Calculates Perez-function coefficients for current turbidity 
 * */

void mcSky::calculatePerezCoeffs()
{
  printf("Calculating Perez-function coefficients .. \n");
  this->perez_Y[0] =  0.1787 * this->turbidity - 1.4630;
  this->perez_Y[1] = -0.3554 * this->turbidity + 0.4275;	
  this->perez_Y[2] = -0.0227 * this->turbidity + 5.3251;
  this->perez_Y[3] =  0.1206 * this->turbidity - 2.5771;		
  this->perez_Y[4] = -0.0670 * this->turbidity + 0.3703;	

  this->perez_x[0] = -0.0193 * this->turbidity - 0.2592;
  this->perez_x[1] = -0.0665 * this->turbidity + 0.0008;	
  this->perez_x[2] = -0.0004 * this->turbidity + 0.2125;
  this->perez_x[3] = -0.0641 * this->turbidity - 0.8989;		
  this->perez_x[4] = -0.0033 * this->turbidity + 0.0452;	

  this->perez_y[0] = -0.0167 * this->turbidity - 0.2608;
  this->perez_y[1] = -0.0950 * this->turbidity + 0.0092;	
  this->perez_y[2] = -0.0079 * this->turbidity + 0.2102;
  this->perez_y[3] = -0.0441 * this->turbidity - 1.6537;		
  this->perez_y[4] = -0.0109 * this->turbidity + 0.0529;	
}

/**
 * void mcSky::calculateSunAngles()
 * 
 * Recalculates sky vertex angles to sun 
 * */

void mcSky::calculateSunAngles()
{
  int i;
  SKYPOINT *v;

  //	printf("SUN -  x %3.2f   y %3.2f   z %3.2f\n", this->sun[0], this->sun[1], this->sun[2]);
  for (i = 0; i < this->cskypoints; i++)
    {
      v = &this->sky[i];
      v->a_sun = acos(sgScalarProductVec3(v->xyz, this->sun));
      //		printf("VIEW -  x %3.2f   y %3.2f   z %3.2f    AS %3.2f\n", v->xyz[0], v->xyz[1], v->xyz[2], v->a_sun);		
    }
}

/**
 * void mcSky::calculateZenithValues()
 * 
 * Calculates luminance and chrominances at zenith
 * */

void mcSky::calculateZenithValues()
{
  float chi;
  float a_sunzenith = this->a_sunzenith,
    a_sunzenith2 = a_sunzenith * a_sunzenith,
    a_sunzenith3 = a_sunzenith * a_sunzenith2;
  float T = this->turbidity,
    T2 = T * T;
	
  chi = (4.0 / 9.0 - this->turbidity / 120.0) * (M_PI - 2 * this->a_sunzenith);
  this->zenith_Y = (4.0453 * this->turbidity - 4.9710) * tan(chi) - 0.2155 * this->turbidity + 2.4192;
	
  this->zenith_x =
    ( 0.00166*a_sunzenith3 - 0.00375*a_sunzenith2 + 0.00209*a_sunzenith + 0)       * T2 +
    (-0.02903*a_sunzenith3 + 0.06377*a_sunzenith2 - 0.03202*a_sunzenith + 0.00394) * T +
    ( 0.11693*a_sunzenith3 - 0.21196*a_sunzenith2 + 0.06052*a_sunzenith + 0.25886);

  this->zenith_y =  
    ( 0.00275*a_sunzenith3 - 0.00610*a_sunzenith2 + 0.00317*a_sunzenith + 0)       * T2 +
    (-0.04214*a_sunzenith3 + 0.08970*a_sunzenith2 - 0.04153*a_sunzenith + 0.00516) * T +
    ( 0.15346*a_sunzenith3 - 0.26756*a_sunzenith2 + 0.06670*a_sunzenith + 0.26688);
	
  printf("Zenith:   xyY [ %4.2f, %4.2f, %4.2f ]\n", this->zenith_x, this->zenith_y, this->zenith_Y);

  this->sky_ok = false;
}


/**
 * void mcSky::calculateSkyColors()
 * 
 * Recalculates sky colors
 * */

void mcSky::calculateSkyColors()
{
  int i;
  float x, y, Y;
  SKYPOINT *v;
	
	
  for (i = 0; i < this->cskypoints; i++)
    {
      v = &this->sky[i];

      ////	Y = this->perez(this->perez_Y, v->a_zenith, v->a_sun);	   
      Y = 0.05 * this->zenith_Y * perez(this->perez_Y, v->a_zenith, v->a_sun) / perez(this->perez_Y, 0.0, this->a_sunzenith);
      x = -0.5 + this->zenith_x * perez(this->perez_x, v->a_zenith, v->a_sun) / perez(this->perez_x, 0.0, this->a_sunzenith);
      y = 0.5 + this->zenith_y * perez(this->perez_y, v->a_zenith, v->a_sun) / perez(this->perez_y, 0.0, this->a_sunzenith);
		
      this->xyYtoRGB(v, x, y, Y);
    }
  this->sky_ok = true;
}





/**
 * Miscellaneous utility functions 
 * **/


/**
 * SKYPOINT *mcSky::createSphere(int x_segs, int y_segs)
 * 
 * Creates a sphere 
 * */

void mcSky::createSphere(int x_segs, int y_segs)
{
  int x, y, v;
  float theta, phi;
	
  if (this->sky != NULL)
    delete[] this->sky;
	
  if (x_segs * y_segs <= 0)
    return;
	
  this->cskypoints = 1 + x_segs * y_segs;
  this->sky_width = x_segs;
  this->sky_height = y_segs;

  this->sky = new SKYPOINT[this->cskypoints];
	
  v = 0;
  this->sky[v].xyz[0] = 0; 
  this->sky[v].xyz[1] = 1.0;
  this->sky[v].xyz[2]= 0;
  v++;

  for (y = 1; y <= y_segs; y++)
    {
      theta = (float)y * M_PI*0.5 / (float)y_segs;
      for (x = 0; x < x_segs; x++, v++)
	{
	  phi = (float)x * 2.0 * M_PI / (float)x_segs;
	  this->sky[v].xyz[0] = sin(theta) * cos(phi);
	  this->sky[v].xyz[2] = sin(theta) * sin(phi);
	  this->sky[v].xyz[1] = cos(theta);
			
	  this->sky[v].a_zenith = acos(sgScalarProductVec3(this->sky[v].xyz, this->zenith));
	}
    }
}


/** 
 * mcSky::createSGVec()
 * 
 * Creates sgVec3 unit direction vector from euler angles. 
 */

void mcSky::createSGVec(sgVec3 &v, float heading, float pitch)
{
  v[0] = sin(pitch) * cos(heading);
  v[1] = sin(pitch) * sin(heading);
  v[2] = cos(pitch);
}

/**
 * mcSky::xyYtoRGB()
 * 
 * Converts a color represented in CIE xyY space to standard RGB
 * */

void mcSky::xyYtoRGB(SKYPOINT *v, float x, float y, float Y)
{
  v->r = (Y + x);
  v->g = (Y - ((0.3 / 0.59) * x + (0.11 / 0.59) * y));
  v->b = (Y + y);
	
  if (v->r > 0 && v->g > 0 && v->b > 0)
    {
      //		printf("Point:   xyz [ %2.3f, %2.3f, %2.3f ]   VS  %2.3f   VZ  %2.3f\n", v->xyz[0], v->xyz[1], v->xyz[2], v->a_sun, v->a_zenith);
      //		printf("Color:   xyY [ %2.3f, %2.3f, %2.3f ]  =>  RGB [ %2.3f, %2.3f, %2.3f ]\n", x, y, Y, v->r, v->g, v->b);
    }
	
  if (v->r < 0.0)
    v->r = 0.0;
  if (v->g < 0.0)
    v->g = 0.0;
  if (v->b < 0.0)
    v->b = 0.0;
}
