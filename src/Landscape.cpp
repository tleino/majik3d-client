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

#include "Landscape.hpp"


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

#include "Debug.hpp"
#include "Perlin.hpp"
#include "Display.hpp"

#define TILE_SIZE                 400.0f      /* cubits */
#define LAMBDA                   (TILE_SIZE/48.0f)
#define TILE_GRID_SIZE            20          /* Even number please! */
#define TRIANGLE_GRID_SIZE        12          /* Num vertices */
#define ELEVATION_SCALE           4.0f
#define ONLINE_TERRAIN_RANGE ((float)(TILE_GRID_SIZE/2  )* TILE_SIZE ) /* cubits */
#define VISUAL_RANGE         ((float)(TILE_GRID_SIZE/2-1)* TILE_SIZE ) /* cubits */

ssgSimpleState *state    = NULL ;

ssgTransform   *terrain  = NULL ;
ssgTransform   *tilegrid [ TILE_GRID_SIZE ][ TILE_GRID_SIZE ] ;

Landscape::Landscape()
{
      DEBUG("Landscape constructor");
}

Landscape::~Landscape()
{
      
   DEBUG ("Landscape destructor.");
}

void 
Landscape::init( ssgRoot *scene_root)
{
   terrain  = new ssgTransform ;
   state    = new ssgSimpleState ;
   state -> setTexture ( "gfx/bumpnoise.rgb") ;
   state -> enable     ( GL_TEXTURE_2D ) ;
   state -> enable     ( GL_LIGHTING ) ;
   printf ("%d\n", display->nosmooth);
   if (display->nosmooth)
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
   
   
   for ( int i = 0 ; i < TILE_GRID_SIZE ; i++ )
	 for ( int j = 0 ; j < TILE_GRID_SIZE ; j++ )
	 {
		tilegrid [ i ][ j ] = new ssgTransform ;
		
		terrain -> addKid ( tilegrid [ i ][ j ] ) ;
		
		sgVec3 tilepos ;
		sgSetVec3 ( tilepos, (float)i * TILE_SIZE - ONLINE_TERRAIN_RANGE,
				   (float)j * TILE_SIZE - ONLINE_TERRAIN_RANGE, 0.0f ) ;
		
		tilegrid [ i ][ j ] -> setTransform ( tilepos ) ;
		createTile ( tilegrid [ i ][ j ], i, j, state ) ;
	 }
   
     scene_root -> addKid ( terrain ) ;
}


ssgBranch *
Landscape::createTileLOD ( int x, int y, ssgSimpleState *state, int ntris, float z_off ) 
{
  sgVec4 *scolors = new sgVec4 [ (ntris+1) * (ntris+1) ] ;
  sgVec2 *tcoords = new sgVec2 [ (ntris+1) * (ntris+1) ] ;
  sgVec3 *snorms  = new sgVec3 [ (ntris+1) * (ntris+1) ] ;
  sgVec3 *scoords = new sgVec3 [ (ntris+1) * (ntris+1) ] ;

  for ( int j = 0 ; j < (ntris+1) ; j++ )
    for ( int i = 0 ; i < (ntris+1) ; i++ )
    {
    //  int address  =  (int)floor( 12.0f * ((float)x + (float)i / (float)ntris)      ) % 256  +
    //                 ((int)floor( 12.0f * ((float)y + (float)j / (float)ntris)      ) % 256 ) * 256 ;
/*      int addressN =  (int)floor( 12.0f * ((float)x + (float)i / (float)ntris)      ) % 256  +
                     ((int)floor( 12.0f * ((float)y + (float)j / (float)ntris)+1.0f ) % 256 ) * 256 ;
      int addressE =  (int)floor( 12.0f * ((float)x + (float)i / (float)ntris)+1.0f ) % 256  +
                     ((int)floor( 12.0f * ((float)y + (float)j / (float)ntris)      ) % 256 ) * 256 ;*/
/*
      float zz  = (float) elevation_map [ address  ] * ELEVATION_SCALE + z_off ;
      float zzN = (float) elevation_map [ addressN ] * ELEVATION_SCALE + z_off ;
      float zzE = (float) elevation_map [ addressE ] * ELEVATION_SCALE + z_off ;
 */	   
	   float zz =  2100*perlin->perlinNoise_2D((x + (float)i/(float)ntris)/7,
											   (y + (float)j/(float)ntris)/7) + z_off;
	   float zzN = 2100*perlin->perlinNoise_2D((x + (float)i/(float)ntris)/7,     
											   (y + (float)j/(float)ntris + 1)/7) + z_off;
	   float zzE = 2100*perlin->perlinNoise_2D((x + (float)i/(float)ntris + 1)/7, 
											   (y + (float)j/(float)ntris)/7) + z_off;
	   /*
	   float rr = (float) image_map [ address * 3 + 0 ] / 255.0f ;
	   float gg = (float) image_map [ address * 3 + 1 ] / 255.0f ;
	   float bb = (float) image_map [ address * 3 + 2 ] / 255.0f ;
		*/

	   float rr = (float) 0.2 ;
//	   float gg = (float) (perlin->perlinNoise_2D( floor( 12.0f * ((float)x + (float)i / (float)ntris)) + 600,
//												  floor( 12.0f * ((float)y + (float)j / (float)ntris)) + 600) + 1) / 2.0f;
	   float bb = (float) (perlin->perlinNoise_2D( (float)( 12.0f * ((float)x + (float)i / (float)ntris))/2 + 600,
												  (float)( 12.0f * ((float)y + (float)j / (float)ntris))/2 + 600) + 1) / 2.0f;
	   
//	   rr = 0.2;
	   
	   bb = pow (bb, 2);
//	   if (bb > gg)
//		 gg -= (bb - gg);
//	   else
//		 gg -= bb;
	   float gg = 1 - bb;
	   
//	   if (gg > 1)
//		 gg = 1;
	   
//	   float temp = gg;
//	   gg *= (1 - bb);
//	   bb *= (1 - temp);

	   
	   
	   
	   float xx = (float) i * (TILE_SIZE/(float)ntris) ;
	   float yy = (float) j * (TILE_SIZE/(float)ntris) ;

      sgVec3 nrm ;

      nrm [ 0 ] = (zz - zzE) / (TILE_SIZE/12.0f) ;
      nrm [ 1 ] = (zz - zzN) / (TILE_SIZE/12.0f) ;
      nrm [ 2 ] = sqrt ( nrm[0] * nrm[0] + nrm[1] * nrm[1] ) ;

      sgCopyVec3( snorms  [i+j*(ntris+1)], nrm ) ;
      sgSetVec2 ( tcoords [i+j*(ntris+1)], xx/LAMBDA, yy/LAMBDA ) ;
      sgSetVec4 ( scolors [i+j*(ntris+1)], rr, gg, bb, 1.0f ) ;
      sgSetVec3 ( scoords [i+j*(ntris+1)], xx, yy, zz ) ;
    }

  ssgBranch *branch = new ssgBranch () ;

  for ( int i = 0 ; i < ntris ; i++ )
  {
    unsigned short *vlist = new unsigned short [ 2 * (ntris+1) ] ;

    for ( int j = 0 ; j < (ntris+1) ; j++ )
	   {
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


void 
Landscape::createTile ( ssgTransform *tile, int x, int y, ssgSimpleState *state ) 
{
  float rr[] = { 0.0f, 2000.0f, 4000.0f, 8000.0f } ;
  ssgRangeSelector *lod = new ssgRangeSelector () ;

  lod  -> addKid ( createTileLOD ( x, y, state, TRIANGLE_GRID_SIZE   - 1,   0.0f ) ) ;
  lod  -> addKid ( createTileLOD ( x, y, state, TRIANGLE_GRID_SIZE/4 - 1, -10.0f ) ) ;
  lod  -> addKid ( createTileLOD ( x, y, state, TRIANGLE_GRID_SIZE/6 - 1, -20.0f ) ) ;
  lod  -> setRanges ( rr, 4 ) ;

  tile -> addKid ( lod ) ;
}
