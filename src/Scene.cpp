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

#include <iostream.h>
#include <pu.h>
#include <sg.h>

#include "Scene.hpp"
#include "Display.hpp"
#include "Debug.hpp"
#include "Landscape.hpp"
#include "Socket.hpp"
#include "Object.hpp"
#include "Config.hpp"

#define random()	rand()

sgCoord        campos;

Player        *tuxi = NULL;
sgMat4         frustumi;

ssgTransform  *trees[100];
ssgTransform  *lumiukko;
ssgTransform  *sky_dome = NULL;

Scene::Scene()
{    
  debug->put("Scene constructor");
}

Scene::~Scene()
{
  if (scene_root != NULL)
    delete scene_root; // hmmm
  
  debug->put("Scene destructor");
}

void
Scene::addSpecial(float x, float y, char *model, bool cutout)
{     
  ssgTransform *trans = new ssgTransform ();
  ssgEntity *entity = ssgLoadAC (model);
  sgCoord tmpPos;
  
  tmpPos.xyz[0] = x;
  tmpPos.xyz[1] = y;
  tmpPos.xyz[2] = getHOT(x, y);
  tmpPos.hpr[1] = 0.0f;
  tmpPos.hpr[2] = 0.0f;
  
  trans->addKid (entity);
  trans->clrTraversalMaskBits (SSGTRAV_ISECT|SSGTRAV_HOT);
  
  trans->setTransform (&tmpPos);
  scene_root->addKid (trans);
}

void
Scene::init()
{
  scene_root = new ssgRoot;
  
  ssgModelPath   ("data");
  ssgTexturePath ("data");
  
  landscape->init(scene_root);
  
  sky_dome = new ssgTransform ();
  ssgEntity *entity = ssgLoadAC ("skydome.ac");
    
   sky_dome->addKid (entity);

  //scene_root->addKid (sky_dome);

  sgVec4 skycol ; sgSetVec4 ( skycol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
  sgVec4 fogcol ; sgSetVec4 ( fogcol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
  
  glEnable ( GL_DEPTH_TEST ) ;
  
  /* Set up the viewing parameters */
  ssgSetFOV     ( 90.0f, 60.0f ) ;
  ssgSetNearFar ( 1.0f, 30000.0f ) ;
  
  float nnear, ffar, top, bottom, left, right, hfov, vfov, v, h;
  
  nnear = 1.0f;
  ffar = 30000.0f;
  h = 90.0f;
  v = 60.0f;
  
  hfov = ( h <= 0 ) ? ( v * 3.0f / 2.0f ) : h ;
  vfov = ( v <= 0 ) ? ( h * 2.0f / 3.0f ) : v ;
  
  right = nnear * (SGfloat) tan ( hfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
  top   = nnear * (SGfloat) tan ( vfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
  left  = -right ;
  bottom   = -top   ;
  
  debug->put ("near: %f far: %f", nnear, ffar);
  debug->put ("left: %f right: %f", left, right);
  debug->put ("bottom: %f top: %f", bottom, top);
  
  debug->put ("%f %f %f %f", 
	      (frustumi[0][0] = (2*nnear)/(right-left)),
	      (frustumi[1][0] = 0.0f),
	      (frustumi[2][0] = (right+left)/(right-left)),
	      (frustumi[3][0] = 0.0f));
  debug->put ("%f %f %f %f",
	      (frustumi[0][1] = 0.0f),
	      (frustumi[1][1] = (2*nnear)/(top-bottom)),
	      (frustumi[2][1] = (top+bottom)/(top-bottom)));
  debug->put ("%f %f %f %f",
	      (frustumi[3][1] = 0.0f),
	      (frustumi[0][2] = 0.0f),
	      (frustumi[1][2] = 0.0f),
	      (frustumi[2][2] = -(ffar+nnear)/(ffar-nnear)));
  debug->put ("%f %f %f %f",
	      (frustumi[3][2] = (-2*ffar*nnear)/(ffar-nnear)),
	      (frustumi[0][3] = 0.0f),
	      (frustumi[1][3] = 0.0f),
	      (frustumi[2][3] = -1.0f),
	      (frustumi[3][3] = 0.0f));
  
  /* Set up some fog */
  glFogf ( GL_FOG_DENSITY, 0.035f / 100.0f);
  glFogfv( GL_FOG_COLOR  , fogcol);
  glFogf ( GL_FOG_START  , 3500.0);
  glFogf ( GL_FOG_END    , 5000.0);
  glFogi ( GL_FOG_MODE   , GL_EXP2);
  glEnable ( GL_FOG );
  
  /* Set up the Sun */
  sgVec3 sunposn ;
  sgVec4 sunamb  ;
  sgSetVec3 ( sunposn, 0.2f, -0.5f, 0.5f ) ;
  sgSetVec4 ( sunamb , 0.4f, 0.4f, 0.4f, 1.0f ) ;
  ssgGetLight ( 0 ) -> setPosition ( sunposn ) ;
  ssgGetLight ( 0 ) -> setColour ( GL_AMBIENT, sunamb ) ;
}

float
Scene::getHOT( float x, float y )
{
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
  int num_hits = ssgHOT ( scene_root, test_vec, invmat, &results ) ;
  
  float hot = -1000000.0f ;
  
  for ( int i = 0 ; i < num_hits ; i++ )
    {
      ssgHit *h = &results [ i ] ;
      
      float hgt = - h->plane[3] / h->plane[2] ;
      
      if ( hgt >= hot )
	hot = hgt ;
    }
  
  return hot;
}


/// FIXME: Unimplemented.
void
Scene::addObject(Object* ob)
{
  
}

// FIXME: Unimplemented.
void
Scene::removeObject(int id)
{ 
  
}

// FIXME: Unimplemented.
void
Scene::removeObject(Object *ob)
{
  
}

void
Scene::update()
{
  static int frameno = 0 ;   
  sgVec3 scale;
  
  frameno++ ;
  
  Object *ob = Object::first;
  
  while (ob != NULL)
    {
	   scale[0] = 1.0f;
	   scale[1] = 1.0f;
	   scale[2] = sin( (float)ob->getMoveCounter() / 2.0 ) / 4 + 1;
      
	   if (!strcmp(ob->getFileName(), "tuxedo.ac"))
		 {
	//		ob->rotateX( 180.0f );
			ob->setScale( scale );	 
		 }
	   else
		 ob->setScale( scale );
	   
	   ob = ob->getNext();
    }
  
   sgCoord tuxpos = tuxi->getPos();
   
   sgCopyVec3 ( campos.xyz, tuxpos.xyz ) ;
   sgCopyVec3 ( campos.hpr, tuxpos.hpr ) ;
   
   if (config->camera == 0)
	 {
		// 3rd person mode.
		campos.xyz[0] -= 20*sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		campos.xyz[1] += 20*cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);  
		
		float tuxhot = getHOT(tuxpos.xyz[0], tuxpos.xyz[1]);
		float camhot = getHOT(campos.xyz[0], campos.xyz[1]);
		campos.xyz[2] += 5+(camhot-tuxhot);
		campos.hpr[1] -= 15+(camhot-tuxhot);
	 }
   else
	 {
		// 1st person mode.
		campos.xyz[0] += 2*sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		campos.xyz[1] -= 2*cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		campos.xyz[2] += 2;
		campos.hpr[1] = display->pitch;
	 }
   
   if (!strcmp(tuxi->getFileName(), "tuxedo.ac"))
    {
//      tuxi->rotateX( 180.0f );
    }
   
  ssgSetCamera ( & campos ) ;
  sky_dome->setTransform (  campos.xyz );
}

void
Scene::drawText(puText *text_object, sgVec3 object_pos)
{
  sgMat4 ploo =
  {
    {  1.0f,  0.0f,  0.0f,  0.0f },
    {  0.0f,  0.0f, -1.0f,  0.0f },
    {  0.0f,  1.0f,  0.0f,  0.0f },
    {  0.0f,  0.0f,  0.0f,  1.0f }
  } ;
  
  sgMat4 viewmat, invmat;
  sgVec3 temppos;
  sgVec4 textpos;
  
  sgMakeCoordMat4 (viewmat, campos.xyz, campos.hpr);
  
  sgTransposeNegateMat4 ( invmat, viewmat );
  
  sgCopyMat4      ( viewmat, ploo ) ;
  sgPreMultMat4   ( viewmat, invmat ) ;
  
  temppos[0] = object_pos[0];
  temppos[1] = object_pos[1];
  temppos[2] = object_pos[2];
  
  sgFullXformPnt3 (temppos, viewmat);
  
  textpos[0] = frustumi[0][0] * temppos[0] +  frustumi[1][0] * temppos[1] +
    frustumi[2][0] * temppos[2] +  frustumi[3][0];
  textpos[1] = frustumi[0][1] * temppos[0] +  frustumi[1][1] * temppos[1] +
    frustumi[2][1] * temppos[2] +  frustumi[3][1];
  textpos[2] = frustumi[0][2] * temppos[0] +  frustumi[1][2] * temppos[1] +
    frustumi[2][2] * temppos[2] +  frustumi[3][2];
  textpos[3] = frustumi[0][3] * temppos[0] +  frustumi[1][3] * temppos[1] +
    frustumi[2][3] * temppos[2] +  frustumi[3][3];
  
  textpos[0] /= textpos[3];
  textpos[1] /= textpos[3];
  
  textpos[0] = textpos[0]*display->width/2 + display->width/2;
  textpos[1] = textpos[1]*display->height/2 + display->height/2;
  
  if (textpos[0] < 0)
    textpos[0] = 0;
  else if (textpos[0] > display->width)
    textpos[0] = display->width;
  
  if (textpos[1] < 0)
    textpos[1] = 0;
  else if (textpos[1] > display->height)
    textpos[1] = display->height;
  
  text_object->setPosition(textpos[0], textpos[1]);
}

void
Scene::draw()
{
  if (tuxi == NULL)
    return; // We do not exist yet and thus no draw() needed.
  
  // Update camera's position in relation to ourself.
  update();
  
  // Draw the stuff to screen.
  ssgCullAndDraw ( scene_root ) ;
  
  // Overlay chat-strings, if any.
  sgVec3 posit;
  
  Object *ob = Object::first;
  
  while (ob != NULL)
    {
      sgCoord obPos = ob->getPos();
      posit[0] = obPos.xyz[0];
      posit[1] = obPos.xyz[1];
      posit[2] = obPos.xyz[2];
      
      drawText( ob->getTextObject(), posit);
      
      ob = ob->getNext();
    }
}
