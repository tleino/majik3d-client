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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream.h>

#include "mcLandscape.hpp"
#include "mcDebug.hpp"
#include "mcPerlin.hpp"
#include "mcDisplay.hpp"
#include "mcConfig.hpp"
#include "mcTexGen.hpp"
#include "mcTerrainGen.hpp"
#include "mcTerrainHeightGen.hpp"
#include "mcMapquad.hpp"
#include "mcCamera.hpp"

double
linearInterpolate(double a, double b, double c)
{
  return ( 1.0 - c ) * a + c * b;
}


Landscape::Landscape()
{
  terrain  = NULL ;  
  debug->put("Landscape constructor");
}

Landscape::~Landscape()
{      
  debug->put("Landscape destructor.");
}

extern class mcTerrainHeightGen *terraingen;

/*
  ssgSetFOV     ( 60.0f, 45.0f ) ;
  ssgSetNearFar ( 0.5f, 10000.0f ) ;
  
  float nnear, ffar, top, bottom, left, right, hfov, vfov, v, h;
  
  nnear = 0.3f;
  ffar = 30000.0f;
  h = 60.0f;
  v = 45.0f;
  
  hfov = ( h <= 0 ) ? ( v * 3.0f / 2.0f ) : h ;
  vfov = ( v <= 0 ) ? ( h * 2.0f / 3.0f ) : v ;
  
  right = nnear * (SGfloat) tan ( hfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
  top   = nnear * (SGfloat) tan ( vfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
  left  = -right ;
  bottom   = -top   ;
*/
/*
  sgMakeCoordMat4 (viewmat, campos.xyz, campos.hpr);
  
  sgTransposeNegateMat4 ( invmat, viewmat );
  
  sgCopyMat4      ( viewmat, ploo ) ;
  sgPreMultMat4   ( viewmat, invmat ) ;
*/
/*
void
Landscape::draw(mcCamera *cam)
{
	glEnable (GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_COORD_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glFrustum ( -1.0f, 1.0f, -.75f, .75f, 0.5f, 10000.0f ) ;
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	sgMat4 mtx, inv;

	sgCoord c;
	cam->getPosition(c);

	sgMat4 ploo =
  {
    {  1.0f,  0.0f,  0.0f,  0.0f },
    {  0.0f,  0.0f, -1.0f,  0.0f },
    {  0.0f,  1.0f,  0.0f,  0.0f },
    {  0.0f,  0.0f,  0.0f,  1.0f }
  } ;
  

	sgMakeCoordMat4( mtx, c.xyz, c.hpr );
	sgTransposeNegateMat4 ( inv, mtx );
	
	sgCopyMat4      ( mtx, ploo );
	sgPreMultMat4   ( mtx, inv );
	
	glLoadMatrixf((float *)mtx);

	m_terrainState->apply();

	Mapquad::root_map->draw();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDisable(GL_VERTEX_ARRAY);
	glDisable (GL_TEXTURE_COORD_ARRAY);
}
*/

float
Landscape::getHOT(float x, float y)const
{
	return 2000.0f * terraingen->getHeight(x/1280.0, y/1280.0);
}

float
Landscape::getRealHOT(float x, float y)const
{
	Mapquad *mq = Mapquad::root_map->getMapquad(Mapquad::MAX_LEVEL, x, y);

	sgVec3 test_vec ;
  sgMat4 invmat ;
  sgMakeIdentMat4 ( invmat ) ;
  
  invmat[3][0] = -x ;
  invmat[3][1] = -y ;
  invmat[3][2] =  0.0f ;
  
  test_vec [0] = 0.0f ;
  test_vec [1] = 0.0f ;
  test_vec [2] = 100000.0f ;
  
  ssgHit *results ;
  int num_hits = ssgHOT ( (ssgRoot *)(mq->getTrans()), test_vec, invmat, &results ) ;
  
  float hot = -1000000.0f ;
  
  for ( int i = 0 ; i < num_hits ; i++ )
    {
      ssgHit *h = &results [ i ] ;
      
      float hgt = - h->plane[3] / h->plane[2] ;
      
      if ( hgt >= hot )
	hot = hgt ;
    }
  
  return hot;
};

void 
Landscape::getNormal(sgVec3& nrm, float x, float y)const
{
	sgVec3 a, b;
	float h = getHOT(x, y);
	const float d = 32.0f;
	
	a [0] = d;
	a [1] = 0;
	a [2] = getHOT(x+d, y) - h;
	
	b [0] = 0; 
	b [1] = d;
	b [2] = getHOT(x, y+d) - h;
	
	sgVectorProductVec3 (nrm, a, b);
	sgNormalizeVec3( nrm );
}

void
Landscape::init( ssgRoot *scene_root)
{
  assert ( scene_root );

  terrain  = new ssgTransform ;
  m_terrainState    = new ssgSimpleState ;
  m_terrainState -> enable     ( GL_TEXTURE_2D ) ;
  m_terrainState -> disable     ( GL_LIGHTING ) ;
  
  if (config->testFlag(mcConfig::SMOOTH))
    m_terrainState -> setShadeModel ( GL_SMOOTH ) ;
  else
    m_terrainState -> setShadeModel ( GL_FLAT );
  
  m_terrainState -> enable ( GL_COLOR_MATERIAL ) ;
  m_terrainState -> enable ( GL_CULL_FACE      ) ;
  m_terrainState -> setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;
  m_terrainState -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
  m_terrainState -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
  m_terrainState -> setShininess ( 0 ) ;
  m_terrainState -> setOpaque () ;
  m_terrainState -> disable ( GL_BLEND ) ;
  
  scene_root -> addKid ( terrain ) ;
  
}

GLuint
Landscape::getTextureHandle ( int level, int x, int y)const
{
	GLuint handle;
	
	GLubyte image[(TERRAIN_RESOLUTION)
		*(TERRAIN_RESOLUTION)*3];

	mcTexGen *tgen = new mcTerrainGen();

	tgen->getPixels(image, x, y, (x+BLOCK_WIDTH+32), (y+BLOCK_WIDTH+32), TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);

	glGenTextures (1, &handle);
	glBindTexture (GL_TEXTURE_2D, handle);

	gluBuild2DMipmaps(	GL_TEXTURE_2D,
						3, 
						TERRAIN_RESOLUTION, 
						TERRAIN_RESOLUTION,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						(GLvoid *) &image[0] );

	glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) ;
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) ;
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ) ;
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ) ;
	glBindTexture (GL_TEXTURE_2D, 0);

	return handle;
}