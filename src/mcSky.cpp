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

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include "mcSky.hpp"

#define S 6999.0f
#ifndef M_PI
#define M_PI 3.141592653f
#endif /* M_PI */

/**
 * Class constructor
 * 
 * Resets the sky to default settings 
 * */

mcSky::mcSky(int x_segs, int y_segs)
{
	sky = NULL;

	sgSetVec3(this->zenith, 0.0, 1.0, 0.0);
	this->createSphere(x_segs, y_segs);
	this->setTurbidity(3.5f);
	this->setSunPosition(M_PI/2.0f, 1.3f*M_PI/4.0f);
	this->setLuminanceFactor(1.2f);

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
   this->a_sunzenith = (float)acos(sgScalarProductVec3(this->sun, this->zenith));	
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
   this->a_sunzenith = acosf(sgScalarProductVec3(this->sun, this->zenith));
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
   state->disable(GL_TEXTURE_2D);
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
   float p1 = (1.0f + params[0] * expf(params[1] / cosf(a_viewzenith))) *
	 ((1.0f + params[2] * expf(params[3] * a_sunview) + params[4] * cosf(a_sunview) * cosf(a_sunview)));
   float p2 = (1.0f + params[0] * expf(params[1])) *
	 ((1.0f + params[2] * expf(params[3] * a_sunzenith) + params[4] * cosf(a_sunzenith) * cosf(a_sunzenith)));
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
   this->perez_Y[0] =  0.1787f * this->turbidity - 1.9630f;	// 1.4630   
   this->perez_Y[1] = -0.3554f * this->turbidity + 0.5275f;	// 0.4275	
   this->perez_Y[2] = -0.0227f * this->turbidity + 5.3251f;
   this->perez_Y[3] =  0.1206f * this->turbidity - 2.5771f;		
   this->perez_Y[4] = -0.0670f * this->turbidity + 0.3703f;	
   
   this->perez_x[0] = -0.0193f * this->turbidity - 0.2592f;
   this->perez_x[1] = -0.0665f * this->turbidity + 0.0008f;	
   this->perez_x[2] = -0.0004f * this->turbidity + 0.2125f;
   this->perez_x[3] = -0.0641f * this->turbidity - 0.8989f;		
   this->perez_x[4] = -0.0033f * this->turbidity + 0.0452f;	

   this->perez_y[0] = -0.0167f * this->turbidity - 0.2608f;
   this->perez_y[1] = -0.0950f * this->turbidity + 0.0092f;	
   this->perez_y[2] = -0.0079f * this->turbidity + 0.2102f;
   this->perez_y[3] = -0.0441f * this->turbidity - 1.6537f;		
   this->perez_y[4] = -0.0109f * this->turbidity + 0.0529f;	
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
		v->a_sun = (float)acos(sgScalarProductVec3(v->xyz, this->sun));
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
     
   chi = (float)(4.0f / 9.0f - this->turbidity / 120.0f) * (M_PI - 2.0f * this->a_sunzenith);
   this->zenith_Y = (4.0453f * this->turbidity - 4.9710f) * tanf(chi) - 0.2155f * this->turbidity + 2.4192f;
   
   this->zenith_x =
	 ( 0.00166f*a_sunzenith3 - 0.00375f*a_sunzenith2 + 0.00209f*a_sunzenith + 0.0f)       * T2 +
	 (-0.02903f*a_sunzenith3 + 0.06377f*a_sunzenith2 - 0.03202f*a_sunzenith + 0.00394f) * T +
	 ( 0.11693f*a_sunzenith3 - 0.21196f*a_sunzenith2 + 0.06052f*a_sunzenith + 0.25886f);
   
   this->zenith_y =  
	 ( 0.00275f*a_sunzenith3 - 0.00610f*a_sunzenith2 + 0.00317f*a_sunzenith + 0.0f)       * T2 +
	 (-0.04214f*a_sunzenith3 + 0.08970f*a_sunzenith2 - 0.04153f*a_sunzenith + 0.00516f) * T +
	 ( 0.15346f*a_sunzenith3 - 0.26756f*a_sunzenith2 + 0.06670f*a_sunzenith + 0.26688f);
   
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
  SKYPOINT *v = NULL;
	
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
	 delete this->sky;
   
   if (x_segs * y_segs <= 0)
	 return;
   
   this->cskypoints = 1 + x_segs * y_segs;
   this->sky_width = x_segs;
   this->sky_height = y_segs;

   this->sky = new SKYPOINT[this->cskypoints];
   
   this->scolors = new sgVec4[(sky_height-3)*(sky_width*2)];
   this->scoords = new sgVec3[(sky_height-3)*(sky_width*2)];	
   
   v = 0;
   sgSetVec3(this->sky[v].xyz, 0.0f, 1.0f, 0.0f);
   this->sky[v].a_zenith = 0.0f;		// Angle to zenith is 0 because we're at zenith
   v++;

   for (y = 1; y <= y_segs; y++)
    {
	   theta = (float)y * M_PI * 0.5f / (float)y_segs;
	   for (x = 0; x < x_segs; x++, v++)
		 {
			phi = (float)x * 2.0f * M_PI / (float)x_segs;
			sgSetVec3(this->sky[v].xyz,
					  sinf(theta) * cosf(phi),
					  cosf(theta),
					  sinf(theta) * sinf(phi));
			this->sky[v].a_zenith = acosf(sgScalarProductVec3(this->sky[v].xyz, this->zenith));
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
   v[0] = sinf(pitch) * cosf(heading);
   v[1] = sinf(pitch) * sinf(heading);
   v[2] = cosf(pitch);
}

/**
 * mcSky::xyYtoRGB()
 * 
 * Converts a color represented in CIE xyY space to standard RGB
 * */


static float gmin = -2.0f, gmax = 45.0f;
static inline float gamma(float l, float amount)
{
   return powf((l - gmin) / (gmax + gmin), amount);
}	

void mcSky::xyYtoRGB(SKYPOINT *v, float x, float y, float Y)
{
   float X, Z;
   
   if (Y < -5.0f || Y > 10000.0f)
	 {
		v->r = 0.0f;
		v->g = 0.0f;
		v->b = 0.0f;
	 }

   X = x * Y / y;
   Z = (1.0f - (x + y)) * Y / y;
	
   v->r = gamma( 2.043f * X - 0.568f * Y - 0.344f * Z, 0.8f);
   v->g = gamma(-1.035f * X + 1.939f * Y + 0.042f * Z, 0.8f);	
   v->b = gamma( 0.011f * X - 0.184f * Y + 1.078f * Z, 0.8f);
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
   
   solartime = this->stndtime + 0.170f * sinf((4.0f * M_PI * (this->day - 80.0f)) / 373.0f) - 0.129f * sinf((2 * M_PI * (this->day - 8.0f) / 355.0f)) + 12.0f * (this->latitude - this->longitude) / M_PI;
   solardeclination = 0.4093f * sinf((2.0f * M_PI * (this->day - 81.0f)) / 368.0f);
 
   thetas = (M_PI / 2.0f - asinf(sinf(this->latitude) * sinf(solardeclination) - cosf(this->latitude) * cosf(solardeclination) * cosf((M_PI * this->stndtime) / 12.0f)));
   omegas = atan2f(-cosf(solardeclination) * sinf((M_PI * this->stndtime)/12.0f), cosf(this->latitude) * sinf(solardeclination) - sinf(this->latitude) * cosf(solardeclination) * cosf((M_PI * this->stndtime) / 12.0f));
   this->setSunPosition(thetas, omegas);
}

