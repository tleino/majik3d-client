/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik.netti.fi>
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

#ifndef __PERLIN_HPP__
#define __PERLIN_HPP__

/** A perlin noise algorithm. Note that this code is ported straight from the
    pseudo code at http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
	and needs some serious optimization and tuning. */

class Perlin
{
 public:
   Perlin();
   ~Perlin();
   /** Generate some noise by x and y. 
	   @return A double within range of -1 and 1. */
   double perlinNoise_2D(double x, double y);
 private:
   double *interpolation_table;
   
   double noise(int octave, int x, int y);
   double cosine_Interpolate(double a, double b, double x);
   double smoothedNoise(int octave, double x, double y);
   double interpolatedNoise(int octave, double x, double y);
};

extern Perlin *perlin;

#endif /* __PERLIN_HPP__ */
