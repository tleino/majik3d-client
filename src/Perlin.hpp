#ifndef __PERLIN_H__
#define __PERLIN_H__

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

#endif
