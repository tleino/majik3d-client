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

#ifndef __TEXGEN_HPP__
#define __TEXGEN_HPP__

class mcTexGen
{

  private:

	int *primes;		// Prime number table
	int octave;

  protected:

	mcTexGen();
  
	// Perlin noise functions
	double perlin(int x);

	double perlin(int x, int y);

	double iperlin(double x);
	double iperlin(double x, double y);

	double hperlin(double x);
	double hperlin(double x, double y);

	void selectPerlinOctave(int o) { octave = o; }


	// Interpolators
	double lerp(double a, double b, double t);		// Linear interpolation
	double hermite(double a, double b, double t);	// Hermite cubic spline interpolation
	
	// Helper functions
	double bias(double t, double b);					// 0.5 = no bias
	double gain(double t, double g);					// 0.5 = no gain

  public:

	virtual int getPixel(double x, double y) = 0;

	virtual void getPixels(unsigned char *image, double nwx, double nwy, double sex, double sey, int width, int height) = 0;
	
	virtual ~mcTexGen();
};

#endif
