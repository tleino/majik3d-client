// mcTerrainGen.cpp: implementation of the mcTerrainGen class.
//
//////////////////////////////////////////////////////////////////////

#include "mcTerrainGen.hpp"
#include "mcLandscape.hpp"
#include "mcScene.hpp"
#include <sg.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mcTerrainGen::mcTerrainGen()
{
	sgSetVec3(c_snow, 255, 255, 255);
	sgSetVec3(c_shadow, 102, 120, 192);
}

mcTerrainGen::~mcTerrainGen()
{

}

int mcTerrainGen::getPixel(double x, double y)
{
	double val;
	int r, g, b;
	
	sgVec3 lightDir = { 0.0, 0.707, 0.707 };
	sgVec3 nrm;
	
	scene->getLandscape()->getNormal(nrm, x, y);
//	nrm[0] *= nrm[0];
//	nrm[1] *= nrm[1];
//	nrm[2] *= nrm[2];


	float d = sgScalarProductVec3(lightDir, nrm);
	r = (int)lerp(c_shadow[0], c_snow[0], d);
	g = (int)lerp(c_shadow[1], c_snow[1], d);
	b = (int)lerp(c_shadow[2], c_snow[2], d);
	
	return (int)((r << 16) + (g << 8) + b);
}

void mcTerrainGen::getPixels(GLubyte *image, double nwx, double nwy, double sex, double sey, int width, int height)
{
	double dx = (sex - nwx) / (double)width, 
		   dy = (sey - nwy) / (double)height;

	double xc = nwx, 
		   yc = nwy;

	for (int y = 0; y < height; y++)
	{
		xc = nwx;

		for (int x = 0; x < width; x++)
		{
			/* Haetaan pikselin väri, voitaisiin optimoida 2d:tä varten oma koodi */

			int px = getPixel(xc, yc);

			*image++ = (px & 0xFF0000) >> 16;
			*image++ = (px & 0xFF00) >> 8;
			*image++ = px & 0xFF;

			xc += dx;
		}

		yc += dy;
	}
}



