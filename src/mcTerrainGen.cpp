// mcTerrainGen.cpp: implementation of the mcTerrainGen class.
//
//////////////////////////////////////////////////////////////////////

#include "mcTerrainGen.hpp"
#include "mcLandscape.hpp"
#include <sg.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mcTerrainGen::mcTerrainGen()
{

	r1 = 87; g1 = 186; b1 = 21;
	r2 = 88; g2 = 144; b2 = 51;

	r3 = 179; g3 = 177; b3 = 28;
	r4 = 230; g4 = 209; b4 = 98;
}

mcTerrainGen::~mcTerrainGen()
{

}

int mcTerrainGen::getPixel(double x, double y)
{
	double val;
	
	selectPerlinOctave(0);

	val = iperlin(x / 16.4, y / 16.4);

	val += iperlin(x / 8.2, y / 8.2) / 2.0 ;

	val += iperlin(x / 4.1, y / 4.1) / 4.0;

	val += iperlin(x / 2.05, y / 2.05) / 8.0;

	val += iperlin(x, y) / 16.0;

	val /= 1.8;



	int ra = (int) (r1 * val + r2 * (1.0 - val));

	int ga = (int) (g1 * val + g2 * (1.0 - val));

	int ba = (int) (b1 * val + b2 * (1.0 - val));

	int rb = (int) (r3 * val + r4 * (1.0 - val));

	int gb = (int) (g3 * val + g4 * (1.0 - val));

	int bb = (int) (b3 * val + b4 * (1.0 - val));


	selectPerlinOctave(1);

	val = iperlin(x / 64.4, y / 64.4);

	val += iperlin(x / 32.1, y / 32.1) / 2.0;

	val += iperlin(x / 16.2, y / 16.2) / 3.5;

	val += iperlin(x / 8.1, y / 8.1) / 4.0;

	val += iperlin(x / 4.1, y / 4.1) / 8.0;

	val += iperlin(x / 2.1, y / 2.1) / 16.0;

	val /= 1.9;

	if (val > 1.0)
		val = 1.0;

	val = gain(val, 0.9);

	int r = (int) (rb * val + ra * (1.0 - val));
	int g = (int) (gb * val + ga * (1.0 - val));
	int b = (int) (bb * val + ba * (1.0 - val));

	sgVec3 lightDir = { 0.0, 0.0, 1.0 };

	sgVec3 nrm;

	landscape->getNormal(nrm, x, y);

//	sgSetVec3(nrm, 0.0<, 0.7, 0.0);
	
	float d = sgScalarProductVec3(lightDir, nrm);
//	d += 0.5;
	if (d<0)
		d = 0.0;
	if (d>1.0)
		d = 1.0;

//	float d = landscape->getHOT(x, y) / 100.0;

//	float d = ((int)x % 10) / 10.0;

	r *=  d;
	b *=  d;
	g *=  d;

//	r = (int)255.0f*d;
//	g = (int)255.0f*d;
//	b = (int)255.0f*d;

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



