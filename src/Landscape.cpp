/** Hieno landscape classi. */

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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <math.h>
#include <ssg.h>
#include <ssgKeyFlier.h>
#include <GL/glut.h>
#include <iostream.h>

#include "Landscape.hpp"
#include "Debug.hpp"
#include "Perlin.hpp"
#include "Display.hpp"
#include "Config.hpp"

#define TILE_SIZE                 256.0f      /* cubits */
#define LAMBDA                   (TILE_SIZE/16.0f)
#define TILE_GRID_SIZE            20          /* Even number please! */
#define TRIANGLE_GRID_SIZE        16          /* Num vertices */
#define ELEVATION_SCALE           4.0f
#define ONLINE_TERRAIN_RANGE ((float)(TILE_GRID_SIZE/2  )* TILE_SIZE ) /* cubits */
#define VISUAL_RANGE         ((float)(TILE_GRID_SIZE/2-1)* TILE_SIZE ) /* cubits */

ssgSimpleState *state    = NULL ;

ssgTransform   *tilegrid [ TILE_GRID_SIZE ][ TILE_GRID_SIZE ] ;

Landscape::Landscape()
{
   terrain  = NULL ;
   
   debug->put("Landscape constructor");

}

Landscape::~Landscape()
{      
   debug->put("Landscape destructor.");
}

void 
Landscape::init( ssgRoot *scene_root)
{
   landscape->initialized = 1;
   
   terrain  = new ssgTransform ;
   state    = new ssgSimpleState ;
   state -> setTexture ( "gfx/bumpnoise.rgb") ;
   state -> enable     ( GL_TEXTURE_2D ) ;
   state -> enable     ( GL_LIGHTING ) ;
   
   if (config->nosmooth)
	 state -> setShadeModel ( GL_FLAT ) ;
   else
	 state -> setShadeModel ( GL_SMOOTH );
   
   state -> enable ( GL_COLOR_MATERIAL ) ;
   state -> enable ( GL_CULL_FACE      ) ;
   state -> setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;
   state -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
   state -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
   state -> setShininess ( 0 ) ;
   state -> setOpaque () ;
   state -> disable ( GL_BLEND ) ;

   scene_root -> addKid ( terrain ) ;
   landscape->initialized = 2;
}

ssgBranch *
Landscape::createTileLOD ( int level, int x, int y, int ntris, char *terrain_map ) 
{
   sgVec4 *scolors = new sgVec4 [ (ntris+1) * (ntris+1) ] ;
   sgVec2 *tcoords = new sgVec2 [ (ntris+1) * (ntris+1) ] ;
   sgVec3 *snorms  = new sgVec3 [ (ntris+1) * (ntris+1) ] ;
   sgVec3 *scoords = new sgVec3 [ (ntris+1) * (ntris+1) ] ;
   
   for ( int j = 0 ; j < (ntris+1) ; j++ ) {
	  for ( int i = 0 ; i < (ntris+1) ; i++ ) {
		 
		 float zz = 1500*perlin->perlinNoise_2D((x + (float)i/(float)ntris*TILE_SIZE)/1000,
											   (y + (float)j/(float)ntris*TILE_SIZE)/1000 );
		 
		 float zzN = 1500*perlin->perlinNoise_2D((x + (float)i/(float)ntris*TILE_SIZE)/1000,
												(y + (float)(j+1)/(float)ntris*TILE_SIZE)/1000 );
		 
		 float zzE = 1500*perlin->perlinNoise_2D((x + (float)(i+1)/(float)ntris*TILE_SIZE)/1000,
												(y + (float)j/(float)ntris*TILE_SIZE)/1000 );
		 
		 float rr = (float) 0.2 ;
		 float gg;
		 float bb = (perlin->perlinNoise_2D((x + (float)i/(float)ntris*TILE_SIZE)/200,
											(y + (float)j/(float)ntris*TILE_SIZE)/200) + 1) / 2.0f;
		 
//		 float bb = 0.2f;
		 
		 bb = pow (bb, 2);
		 gg = 1 - bb;
		 
		 float xx = (float) i * (TILE_SIZE/(float)ntris) ;
		 float yy = (float) j * (TILE_SIZE/(float)ntris) ;

		 sgVec3 a, b;
		 
		 a [0] = TILE_SIZE/ntris;
		 a [1] = 0;
		 a [2] = zz - zzE;
		 
		 b [0] = 0; 
		 b [1] = TILE_SIZE/ntris;
		 b [2] = zz - zzN;
		 
		 sgVec3 nrm ;
		 
		 sgVectorProductVec3 (nrm, a, b);
		 
		 /*
		 nrm [ 0 ] = (zz - zzE)  / (TILE_SIZE/16.0f) ;
		 nrm [ 1 ] = (zz - zzN)  / (TILE_SIZE/16.0f) ;
		 nrm [ 2 ] = sqrt ( nrm[0] * nrm[0] + nrm[1] * nrm[1] ) ;
		 */
		 sgNormalizeVec3( nrm );
		 
		 sgCopyVec3( snorms  [i+j*(ntris+1)], nrm ) ;
		 sgSetVec2 ( tcoords [i+j*(ntris+1)], xx/LAMBDA, yy/LAMBDA ) ;
		 sgSetVec4 ( scolors [i+j*(ntris+1)], rr, gg, bb, 1.0f ) ;
		 sgSetVec3 ( scoords [i+j*(ntris+1)], xx, yy, zz ) ;
	  }
   }
   
   ssgBranch *branch = new ssgBranch () ;
   
   for ( int i = 0 ; i < ntris ; i++ ) {
	  unsigned short *vlist = new unsigned short [ 2 * (ntris+1) ] ;
	  
	  for ( int j = 0 ; j < (ntris+1) ; j++ ) {
		 vlist [   j + j   ] = j + (i+1) * (ntris+1) ;
		 vlist [ j + j + 1 ] = j +   i   * (ntris+1) ;
	  }
	  
	  ssgLeaf *gset = new ssgVTable ( GL_TRIANGLE_STRIP,
									 2 * (ntris+1), vlist, scoords,
									 2 * (ntris+1), vlist, snorms ,
									 2 * (ntris+1), vlist, tcoords,
									 2 * (ntris+1), vlist, scolors ) ;
	  gset -> setState ( state ) ;
	  branch -> addKid ( gset  ) ;
   }

   return branch ;

}
