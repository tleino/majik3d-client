/* Note that this code is ported straight from the pseudo code at
 * http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
 * and needs some serious optimization and tuning.
 */

#include <math.h>
#include <stdio.h>
#include "Perlin.hpp"

#define PERSISTENCE 0.25
#define OCTAVES        4

Perlin::Perlin()
{
   #ifdef DEBUG
	 Debug("Perlin constructor");
   #endif
}

Perlin::~Perlin()
{
   #ifdef DEBUG
	 Debug("Perlin destructor");
   #endif
	 
}

float
Perlin::cosine_Interpolate(float a, float b, float x)
{
   float ft, f;
   
   ft = x * 3.1415927;
   f = (1 - cos(ft)) * .5;
	 
   return  a*(1-f) + b*f;
}

float
Perlin::noise(int octave, int x, int y)
{
   int n;
   n = x + y * 57;
   n = (n<<13) ^ n;
   
   switch ( octave )
	 {
	  case 0:
		return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
		break;
	  case 1:
		return ( 1.0 - ( (n * (n * n * 16097 + 788089) + 1370008667) & 0x7fffffff) / 1073741824.0);
		break;
	  case 2:
		return ( 1.0 - ( (n * (n * n * 16921 + 783131) + 1379991103) & 0x7fffffff) / 1073741824.0);
		break;
	  case 3:
		return ( 1.0 - ( (n * (n * n * 15373 + 784411) + 1379991589) & 0x7fffffff) / 1073741824.0);
		break;
	  case 4:
		return ( 1.0 - ( (n * (n * n * 15661 + 787939) + 1378991399) & 0x7fffffff) / 1073741824.0);
		break;
	  default:
		printf("ERROR: Noise-function for octave %d not defined.\n", octave);
		break;
	 }
}

float
Perlin::smoothedNoise(int octave, float x, float y)
{
   float corners, sides, center;
   
   corners = ( noise(octave, x-1, y-1)+noise(octave, x+1, y-1)+noise(octave, x-1, y+1)+noise(octave, x+1, y+1) ) / 16;
   sides   = ( noise(octave, x-1, y)  +noise(octave, x+1, y)  +noise(octave, x, y-1)  +noise(octave, x, y+1) ) /  8;
   center  =  noise(octave, x, y) / 4;
   return corners + sides + center;
}

float
Perlin::interpolatedNoise(int octave, float x, float y)
{
   int integer_X, integer_Y;
   float fractional_X, fractional_Y;
   float v1, v2, v3, v4, i1, i2;
   
   integer_X    = (int) x;
   fractional_X = x - integer_X;
   
   integer_Y    = (int) y;
   fractional_Y = y - integer_Y;
   
   v1 = smoothedNoise(octave, integer_X,     integer_Y);
   v2 = smoothedNoise(octave, integer_X + 1, integer_Y);
   v3 = smoothedNoise(octave, integer_X,     integer_Y + 1);
   v4 = smoothedNoise(octave, integer_X + 1, integer_Y + 1);
   
   i1 = cosine_Interpolate(v1 , v2 , fractional_X);
   i2 = cosine_Interpolate(v3 , v4 , fractional_X);
   
   return cosine_Interpolate(i1 , i2 , fractional_Y);

}

float
Perlin::perlinNoise_2D(float x, float y)
{
   int frequency, i, n;
   float p, amplitude, total = 0;
 
   p = PERSISTENCE;
   n = OCTAVES;

   for (i=0;i<n;i++)
	 {
		frequency = pow(2,i);
		amplitude = pow(p,i);

		total = total + interpolatedNoise(i, x * frequency, y * frequency) * amplitude;
	 }

   return total;

}
