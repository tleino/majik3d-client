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
#include "mcConfig.hpp"

#define S 4999.0f
#ifndef M_PI
#define M_PI 3.141592653f
#endif /* M_PI */

struct SKYPOINT
{
  sgVec3 xyz;      /* X, Y and Z-coordinates */
  float r, g, b;   /* Red, green, blue */
  
  float a_sun;     /* Angle to sun */
  float a_zenith;  /* Angle to zenith */
};


/**
 * Class constructor
 * 
 * Resets the sky to default settings 
 * */

mcSky::mcSky(int x_segs, int y_segs)
{
	m_sky = NULL;

	sgSetVec3(m_zenith, 0.0, 1.0, 0.0);
	createSphere(x_segs, y_segs);
	setTurbidity(3.5f);
	setSunPosition(M_PI/2.0f, 1.3f*M_PI/4.0f);
	setLuminanceFactor(1.2f);

	m_skyOk = false;
}

/**
 * Class destructor
 * */

mcSky::~mcSky()
{
	delete m_sky;
}


/**
 * Class interface functions 
 * **/

/**
 * void mcSky::setLuminanceFactor(float f)
 *
 * Defines the luminance factor. Good values are around 1.0 .. 1.2.
 * */

void mcSky::setLuminanceFactor(float f)
{
   m_luminancefactor = f;
   m_skyOk = false;
}


/**
 * void mcSky::setTurbidity(float t)
 * 
 * Defines the turbidity (ratio of particles/haze in atmosphere) of the sky
 * */

void mcSky::setTurbidity(float t)
{
   m_turbidity = t;
   calculatePerezCoeffs();
   calculateZenithValues();
}

/**
 * void mcSky::setSunPosition(sgVec3 &v)
 * 
 * Defines the location of the sun 
 * */

void mcSky::setSunPosition(sgVec3 &v)
{
	sgCopyVec3(m_sun, v);
	m_aSunZenith = (float)acos(sgScalarProductVec3(m_sun, m_zenith));	
	calculateSunAngles();
	calculateZenithValues();
}

/**
 * void mcSky::setSunPosition(float heading, float pitch)
 * 
 * Defines the location of the sun in euler angles. 
 * */

void mcSky::setSunPosition(float heading, float pitch)
{
   if (sin(pitch) < -0.4)
	  pitch = M_PI + 0.5;
   createSGVec(m_sun, heading, pitch);
   m_aSunZenith = acosf(sgScalarProductVec3(m_sun, m_zenith));
   calculateSunAngles();
   calculateZenithValues();
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
   
   if (!m_skyOk)
	 calculateSkyColors();
   
   /* Draw top fan */
   
   p = 0;
   v = &m_sky[0];
   sgSetVec4(m_scolors[0][p], v->r, v->g, v->b, 1.0f);
   sgSetVec3(m_scoords[0][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
   p++;
   
   for (x = 0; x < m_skyWidth; x++, p++)
	 {
		v = &m_sky[x + 1];
		sgSetVec4(m_scolors[0][p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(m_scoords[0][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
	 }
   v = &m_sky[1];
   sgSetVec4(m_scolors[0][p], v->r, v->g, v->b, 1.0f);
   sgSetVec3(m_scoords[0][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
   p++;

   m_strips[0]->recalcBSphere();
   
   /* Draw horizontal strips */
      
   for (y = 0; y < m_skyHeight+1; y++)
	 {
		p = 0;

		for (x = 0; x < m_skyWidth; x++)
		  { 
			 v = &m_sky[y * m_skyWidth + x + 1];
			 sgSetVec4(m_scolors[y+1][p], v->r, v->g, v->b, 1.0f);
			 sgSetVec3(m_scoords[y+1][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
			 p++;

			 v = &m_sky[(y + 1) * m_skyWidth + x + 1];
			 sgSetVec4(m_scolors[y+1][p], v->r, v->g, v->b, 1.0f);
			 sgSetVec3(m_scoords[y+1][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
			 p++;
		  }	
		
		v = &m_sky[y * m_skyWidth + 1];
		sgSetVec4(m_scolors[y+1][p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(m_scoords[y+1][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]); 
		p++;

		v = &m_sky[(y + 1) * m_skyWidth + 1];
		sgSetVec4(m_scolors[y+1][p], v->r, v->g, v->b, 1.0f);
		sgSetVec3(m_scoords[y+1][p], S*v->xyz[0], S*v->xyz[1], S*v->xyz[2]);
		
		m_strips[y]->recalcBSphere();
	 }
   return m_skydome;
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
   m_perez_Y[0] =  0.1787f * m_turbidity - 1.9630f;	// 1.4630   
   m_perez_Y[1] = -0.3554f * m_turbidity + 0.5275f;	// 0.4275	
   m_perez_Y[2] = -0.0227f * m_turbidity + 5.3251f;
   m_perez_Y[3] =  0.1206f * m_turbidity - 2.5771f;		
   m_perez_Y[4] = -0.0670f * m_turbidity + 0.3703f;	
   
   m_perez_x[0] = -0.0193f * m_turbidity - 0.2592f;
   m_perez_x[1] = -0.0665f * m_turbidity + 0.0008f;	
   m_perez_x[2] = -0.0004f * m_turbidity + 0.2125f;
   m_perez_x[3] = -0.0641f * m_turbidity - 0.8989f;		
   m_perez_x[4] = -0.0033f * m_turbidity + 0.0452f;	

   m_perez_y[0] = -0.0167f * m_turbidity - 0.2608f;
   m_perez_y[1] = -0.0950f * m_turbidity + 0.0092f;	
   m_perez_y[2] = -0.0079f * m_turbidity + 0.2102f;
   m_perez_y[3] = -0.0441f * m_turbidity - 1.6537f;		
   m_perez_y[4] = -0.0109f * m_turbidity + 0.0529f;	
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
   
   for (i = 0; i < m_cSkyPoints; i++)
	 {
		v = &m_sky[i];
		v->a_sun = (float)acos(sgScalarProductVec3(v->xyz, m_sun));
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
   float a_sunzenith = m_aSunZenith,
	 	a_sunzenith2 = a_sunzenith * a_sunzenith,
	 	a_sunzenith3 = a_sunzenith * a_sunzenith2;
   float T = m_turbidity,
	    T2 = T * T;
     
   chi = (float)(4.0f / 9.0f - T / 120.0f) * (M_PI - 2.0f * a_sunzenith);
   m_zenithY = (4.0453f * T - 4.9710f) * tanf(chi) - 0.2155f * T + 2.4192f;
   
   m_zenithx =
	 ( 0.00166f*a_sunzenith3 - 0.00375f*a_sunzenith2 + 0.00209f*a_sunzenith + 0.0f)       * T2 +
	 (-0.02903f*a_sunzenith3 + 0.06377f*a_sunzenith2 - 0.03202f*a_sunzenith + 0.00394f) * T +
	 ( 0.11693f*a_sunzenith3 - 0.21196f*a_sunzenith2 + 0.06052f*a_sunzenith + 0.25886f);
   
   m_zenithy =  
	 ( 0.00275f*a_sunzenith3 - 0.00610f*a_sunzenith2 + 0.00317f*a_sunzenith + 0.0f)       * T2 +
	 (-0.04214f*a_sunzenith3 + 0.08970f*a_sunzenith2 - 0.04153f*a_sunzenith + 0.00516f) * T +
	 ( 0.15346f*a_sunzenith3 - 0.26756f*a_sunzenith2 + 0.06670f*a_sunzenith + 0.26688f);
   
   m_skyOk = false;
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
	SKYPOINT *v = NULL, *prev = NULL;
	
	for (i = 0; i < m_skyHeight * m_skyWidth; i++)
    {
		v = &m_sky[i];

		Y = m_zenithY * perez(m_perez_Y, v->a_zenith, v->a_sun, m_aSunZenith);
		x = m_zenithx * perez(m_perez_x, v->a_zenith, v->a_sun, m_aSunZenith);
		y = m_zenithy * perez(m_perez_y, v->a_zenith, v->a_sun, m_aSunZenith);
	
		Y *= m_luminancefactor;
		xyYtoRGB(v, x, y, Y);
    }

	for (i = m_skyHeight * m_skyWidth; i < (m_skyHeight + 1) * m_skyWidth; i++)
	{
		v = &m_sky[i];
		prev = &m_sky[i-m_skyWidth];
		v->r = prev->r;
		v->g = prev->g;
		v->b = prev->b;
	}
	
	m_skyOk = true;
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
   
    if (m_sky != NULL)
		delete m_sky;
   
    if (x_segs * y_segs <= 0)
		return;
   
    m_cSkyPoints = 1 + x_segs * (y_segs + 1);
    m_skyWidth = x_segs;
    m_skyHeight = y_segs;

    // Allocate space
    m_sky = new SKYPOINT[m_cSkyPoints];
    m_scolors.resize(y_segs+2);		// segments + top and bottom
    for (y = 0; y < y_segs+2; y++)
		m_scolors[y] = new sgVec4[m_skyWidth * 2 + 2];

    m_scoords.resize(y_segs+2);
    for (y = 0; y < y_segs+2; y++)
		m_scoords[y] = new sgVec3[m_skyWidth * 2 + 2];

    m_strips.resize(y_segs+2);
    m_skydome = new ssgTransform;

    m_state = new ssgSimpleState;
    m_state->enable(GL_COLOR_MATERIAL);
    if (config->testFlag(mcConfig::SMOOTH))
		m_state->setShadeModel (GL_SMOOTH);
    else
		m_state->setShadeModel (GL_FLAT);
	m_state->enable(GL_CULL_FACE);
	m_state->disable(GL_TEXTURE_2D);
	m_state->setOpaque();
	m_state->disable(GL_FOG);
	m_state->disable(GL_BLEND);
	m_state->disable(GL_LIGHTING);
   
	// Initialize
	v = 0;
	sgSetVec3(m_sky[v].xyz, 0.0f, 1.0f, 0.0f);

	m_sky[v].a_zenith = 0.0f;		// Angle to zenith is 0 because we're at zenith
	m_strips[0] = new ssgVTable(GL_TRIANGLE_FAN,
								x_segs + 2, m_scoords[0],
								0, NULL,
								0, NULL,
								x_segs + 2, m_scolors[0]);
	m_strips[0]->setState(m_state);
	m_strips[0]->setCullFace(TRUE);
	m_skydome->addKid(m_strips[0]);

	v++;

	for (y = 1; y <= y_segs; y++)
	{
		m_strips[y] = new ssgVTable(GL_TRIANGLE_STRIP,
									x_segs * 2 + 2, m_scoords[y],
									0, NULL,
									0, NULL,
									x_segs * 2 + 2, m_scolors[y]);
	    m_strips[y]->setState(m_state);
	    m_strips[y]->setCullFace(TRUE);
	    m_skydome->addKid(m_strips[y]);
									
		theta = 0.99f * (float)y * M_PI * 0.5f / (float)y_segs;
		for (x = 0; x < x_segs; x++, v++)
		{
			phi = (float)x * 2.0f * M_PI / (float)x_segs;
			sgSetVec3(m_sky[v].xyz,
					  sinf(theta) * cosf(phi),
					  cosf(theta),
					  sinf(theta) * sinf(phi));
			m_sky[v].a_zenith = acosf(sgScalarProductVec3(m_sky[v].xyz, m_zenith));
		}
	}

    m_strips[y_segs+1] = new ssgVTable(GL_TRIANGLE_STRIP,
								x_segs * 2 + 2, m_scoords[y_segs+1],
								0, NULL,
								0, NULL,
								x_segs * 2 + 2, m_scolors[y_segs+1]);
	m_strips[y_segs+1]->setState(m_state);
	m_strips[y_segs+1]->setCullFace(TRUE);
	m_skydome->addKid(m_strips[y_segs+1]);
								
    theta = (float)SG_DEGREES_TO_RADIANS * 175.0;
    for (x = 0; x < x_segs; x++, v++)
	{
		phi = (float)x * 2.0f * M_PI / (float)x_segs;
		sgSetVec3(m_sky[v].xyz,
				  sinf(theta) * cosf(phi),
				  cosf(theta),
				  sinf(theta) * sinf(phi));
		m_sky[v].a_zenith = acosf(sgScalarProductVec3(m_sky[v].xyz, m_zenith));
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


