/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik-devel@majik.netti.fi>
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

class Perlin
{
 public:
   Perlin();
   ~Perlin();
   float perlinNoise_2D(float, float);
 private:
   float noise(int, int, int);
   float cosine_Interpolate(float, float, float);
   float smoothedNoise(int, float, float);
   float interpolatedNoise(int, float, float);
};

#endif /* __PERLIN_HPP__ */
