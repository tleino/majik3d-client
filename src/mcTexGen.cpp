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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcTexGen.hpp"

static int primetable[3 * 5] = 
{
	15731,	789221,	1376312589, 
	16097,	788089,	1370008667,
	16921,	788089,	1370008667,
	15373,	784411,	1379991589,
	15661,	787939,	1378991399
};

static double oneper30 = 1.0 / 1073741824.0;

mcTexGen::mcTexGen()
{
	primes = &primetable[0];
	octave = 0;
}

mcTexGen::~mcTexGen()
{
	primes = NULL;
}

/*** INTERPOLATORS ***********************************************************/

// Linear interpolation

double mcTexGen::lerp(double a, double b, double t)
{
	return t * b + (1.0 - t) * a;
}

// Hermite cubic spline interpolation

double mcTexGen::hermite(double a, double b, double t)
{
	t = (t * t * (3.0 - 2.0 * t));
	return t * b + (1.0 - t) * a;
}

/*** MISC FUNCTIONS **********************************************************/

double mcTexGen::bias(double t, double b)
{
	#define	LOGHALF	-0.6931471805599		// natural logarithm of 0.5
	return pow(t, log(b) / LOGHALF);
}

double mcTexGen::gain(double t, double g)
{
	if (t < 0.5)
		return bias(2 * t, 1.0 - g) * 0.5;
	else
		return 1.0 - bias(2.0 - 2.0 * t, 1.0 - g) * 0.5;
}

/*** PERLIN NOISE ************************************************************/

double	mcTexGen::perlin(int t)
{
	t = (t << 13) ^ t;
	return ((t * (t * t * primes[octave*3 + 0] + primes[octave*3 + 1]) + primes[octave*3 + 2]) & 0x3FFFFFFF) * oneper30;
}

double	mcTexGen::perlin(int x, int y)
{
	int t = x + y * 51;

	t = (t << 13) ^ t;
	return ((t * (t * t * primes[octave*3 + 0] + primes[octave*3 + 1]) + primes[octave*3 + 2]) & 0x3FFFFFFF) * oneper30;
}

double	mcTexGen::iperlin(double x)
{
	return lerp(perlin((int)x), perlin((int)x + 1), x - (int)x);
}

double	mcTexGen::hperlin(double x)
{
	return hermite(perlin((int)x), perlin((int)x + 1), x - (int)x);
}


double	mcTexGen::iperlin(double x, double y)
{
	int ix = (int)x,
		iy = (int)y;
	double nw = perlin(ix, iy),
		   ne = perlin(ix + 1, iy),
		   sw = perlin(ix, iy + 1),
		   se = perlin(ix + 1, iy + 1);

	double w = lerp(nw, sw, y - iy),
		   e = lerp(ne, se, y - iy);
	return lerp(w, e, x - ix);

}

double	mcTexGen::hperlin(double x, double y)
{
	int ix = (int)x,
		iy = (int)y;
	double nw = perlin(ix, iy),
		   ne = perlin(ix + 1, iy),
		   sw = perlin(ix, iy + 1),
		   se = perlin(ix + 1, iy + 1);

	double w = hermite(nw, sw, y - iy),
		   e = hermite(ne, se, y - iy);
	return hermite(w, e, x - ix);

}


