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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include "mcSky.hpp"

float min_r, max_r, min_g, max_g, min_b, max_b;
float min_x, max_x, min_y, max_y, min_Y, max_Y;

#define S 6999.0f
#define M_PI 3.141592653

/**
 * Class constructor
 * 
 * Resets the sky to default settings 
 * */

mcSky::mcSky(int x_segs, int y_segs)
{
   sgSetVec3(this->zenith, 0.0, 1.0, 0.0);
   this->createSphere(x_segs, y_segs);
   this->setTurbidity(3.5);
   this->setSunPosition(M_PI/2.0, 1.3*M_PI/4.0);
   this->setLuminanceFactor(1.2);
   this->sky_ok = false;
}

/**
 * Class destructor
 * */

mcSky::~mcSky()
{
   if (this->sky != NULL)
	 delete[] sky;
   if (this->scolors != NULL)
	 delete[] scolors;
   if (this->scoords != NULL)
	 delete[] scoords;
}


/**
 * Class interface functions 
 * **/

void mcSky::setLuminanceFactor(float f)
{
   this->luminancefactor = f;
   this->sky_ok = false;
}


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
//   this->sun = v;
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
   this->calculateSunAngles();
   this->calculateZenithValues();
}

/**
 * void mcSky::Draw()
 * 
 * Renders the sky to screen
 * */

ssgEntity *mcSky::Draw()
{
   int x, y, p = 0;
   SKYPOINT *v;
   
   if (!this->sky_ok)
	 this->calculateSkyColors();
   
   ssgSimpleState *state = new ssgSimpleState;
   state->enable(GL_COLOR_MATERIAL);
   state->setShadeModel(GL_SMOOTH);
   state->disable(GL_CULL_FACE);
   state->setOpaque();
   state->disable(GL_FOG);
   //state->setShininess(0);
   //state->setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
   //state->setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ; 
   state->disable(GL_BLEND);
   state->disable(GL_LIGHTING);

   ssgBranch *skydome = new ssgTransform;
   
   /* Draw top fan */
   
   this->scolors = new sgVec4[(sky_height-3)*(sky_width*2)];
   this->scoords = new sgVec3[(sky_height-3)*(sky_width*2)];  
   p = 0;
   v = &this->sky[0];
   sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
   sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
   p++;
   
   for (x = 0; x < this->sky_width; x++, p++)
	 {
		v = &this->sky[x + 1];
		sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
	 }
   v = &this->sky[1];
   sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
   sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
   p++;
   
   ssgLeaf *topfan = new ssgVTable(GL_TRIANGLE_FAN,
								   p, scoords,
								   0, NULL,
								   0, NULL,
								   p, scolors);
   topfan->setState(state);
   topfan->setCullFace(FALSE);
   skydome->addKid(topfan);									
   
   /* Draw horizontal strips */
      
   for (y = 0; y < this->sky_height - 1; y++)
	 {
		p = 0;
		this->scolors = new sgVec4[(sky_height-3)*(sky_width*2)];
		this->scoords = new sgVec3[(sky_height-3)*(sky_width*2)]; 
		
		v = &this->sky[y * this->sky_width + x + 1];
		sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
		p++;
		
		for (x = 0; x < this->sky_width; x++)
		  {
			 v = &this->sky[(y + 1) * this->sky_width + x + 1];
			 sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
			 sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
			 p++;
			 
			 v = &this->sky[y * this->sky_width + x + 1];
			 sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
			 sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
			 p++;
		  }	
		
		v = &this->sky[(y + 1) * this->sky_width + 1];
		sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
		p++;
		
		v = &this->sky[y * this->sky_width + 1];
		sgSetVec4(scolors[p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(scoords[p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
		
		ssgLeaf *hstrip = new ssgVTable(GL_TRIANGLE_STRIP,
										p+1, scoords,
										0, NULL,
										0, NULL,
										p+1, scolors);
		hstrip->setState(state);
		hstrip->setCullFace(FALSE);
		skydome->addKid(hstrip);
	 }
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

float mcSky::perez(float *params, float a_viewzenith, float a_sunview, float a_sunzenith)
{
   float p1 = (1.0 + params[0] * exp(params[1] / cos(a_viewzenith))) *
	 ((1.0 + params[2] * exp(params[3] * a_sunview) + params[4] * cos(a_sunview) * cos(a_sunview)));
   float p2 = (1.0 + params[0] * exp(params[1])) *
	 ((1.0 + params[2] * exp(params[3] * a_sunzenith) + params[4] * cos(a_sunzenith) * cos(a_sunzenith)));
   float result = p1 / p2;
   
   return result;
}

/**
 * void mcSky::calculatePerezCoeffs()
 * 
 * Calculates Perez-function coefficients for current turbidity 
 * */

void mcSky::calculatePerezCoeffs()
{
   this->perez_Y[0] =  0.1787 * this->turbidity - 1.9630;	// 1.4630   
   this->perez_Y[1] = -0.3554 * this->turbidity + 0.5275;	// 0.4275	
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
   
   for (i = 0; i < this->cskypoints; i++)
	 {
		v = &this->sky[i];
		v->a_sun = acos(sgScalarProductVec3(v->xyz, this->sun));
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

      Y = this->zenith_Y * perez(this->perez_Y, v->a_zenith, v->a_sun, this->a_sunzenith);// / perez(this->perez_Y, 0.0, this->a_sunzenith);
      x = this->zenith_x * perez(this->perez_x, v->a_zenith, v->a_sun, this->a_sunzenith);//) / perez(this->perez_x, 0.0, this->a_sunzenith);
      y = this->zenith_y * perez(this->perez_y, v->a_zenith, v->a_sun, this->a_sunzenith);//) / perez(this->perez_y, 0.0, this->a_sunzenith);
	
      Y *= this->luminancefactor;
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
   
   this->scolors = new sgVec4[(sky_height-3)*(sky_width*2)];
   this->scoords = new sgVec3[(sky_height-3)*(sky_width*2)];	
   
   v = 0;
   sgSetVec3(this->sky[v].xyz, 0.0, 1.0, 0.0);
   v++;

  for (y = 1; y <= y_segs; y++)
    {
	   theta = (float)y * M_PI *.5/ (float)y_segs;
	   for (x = 0; x < x_segs; x++, v++)
		 {
			phi = (float)x * 2.0 * M_PI / (float)x_segs;
			sgSetVec3(this->sky[v].xyz,
					  sin(theta) * cos(phi),
					  cos(theta),
					  sin(theta) * sin(phi));
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


static float gmin = -2.0, gmax = 45.0;
static inline float gamma(float l, float amount)
{
   return pow((l - gmin) / (gmax + gmin), amount);
}	

void mcSky::xyYtoRGB(SKYPOINT *v, float x, float y, float Y)
{
  float X, Z;
   
   if (Y < -5.0 || Y > 10000.0)
	 {
		v->r = 0.0;
		v->g = 0.0;
		v->b = 0.0;
	 }

   X = x * Y / y;
   Z = (1.0 - (x + y)) * Y / y;
	
   v->r = gamma( 2.043 * X - 0.568 * Y - 0.344 * Z, 0.8);
   v->g = gamma(-1.035 * X + 1.939 * Y + 0.042 * Z, 0.8);	
   v->b = gamma( 0.011 * X - 0.184 * Y + 1.078 * Z, 0.8);
}


/**
 * Sun position functions 
 * */

void mcSky::setTimeInDay(float t)
{
   this->stndtime = t;
   this->updateSunPosition();
}
 
void mcSky::setDayInYear(float d)
{
   this->day = 365 * d;
   this->updateSunPosition();
}
 
void mcSky::setLatLong(float latitude, float longitude)
{
   this->latitude = latitude;
   this->longitude = longitude;
   this->updateSunPosition();
}
 
void mcSky::updateSunPosition()
{
   float solartime, solardeclination;
   float thetas, omegas;
   
   solartime = this->stndtime + 0.170 * sin((4.0 * M_PI * (this->day - 80)) / 373.0) - 0.129 * sin((2 * M_PI * (this->day - 8) / 355.0)) + 12 * (this->latitude - this->longitude) / M_PI;
   solardeclination = 0.4093 * sin((2.0 * M_PI * (this->day - 81)) / 368.0);
 
   thetas = M_PI / 2.0 - asin(sin(this->latitude) * sin(solardeclination) - cos(this->latitude) * cos(solardeclination) * cos((M_PI * this->stndtime) / 12.0));
   omegas = atan2(-cos(solardeclination) * sin((M_PI * this->stndtime)/12.0), cos(this->latitude) * sin(solardeclination) - sin(this->latitude) * cos(solardeclination) * cos((M_PI * this->stndtime) / 12.0));
   this->setSunPosition(thetas, omegas);
}

