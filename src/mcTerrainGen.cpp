// mcTerrainGen.cpp: implementation of the mcTerrainGen class.
//
//////////////////////////////////////////////////////////////////////

#include "mcTerrainGen.hpp"
#include "mcLandscape.hpp"
#include "mcScene.hpp"
#include <plib/sg.h>

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

void mcTerrainGen::getPixel(GLubyte *ptr, double x, double y)
{
	sgVec3 lightDir = { 0.0, 0.70710678, 0.70710678 };
	sgVec3 nrm;
	
	scene->getLandscape()->getNormal(nrm, x, y);

	float d = sgScalarProductVec3(lightDir, nrm);
	if (d < 0.0)
		d = 0.0;
	if (d > 1.0)
		d = 1.0;

	ptr[0] = (GLubyte)lerp(c_snow[0], c_shadow[0], d);
	ptr[1] = (GLubyte)lerp(c_snow[1], c_shadow[1], d);
	ptr[2] = (GLubyte)lerp(c_snow[2], c_shadow[2], d);
	
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
			/* Fetch pixel color, we could optimize the algorithm here for bigger blocks */

			getPixel(image, xc, yc);
			image += 3;
			xc += dx;
		}

		yc += dy;
	}
}



