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

#include <iostream>
#include <plib/pu.h>
#include <plib/sg.h>

#include "mcScene.hpp"
#include "mcDisplay.hpp"
#include "mcDebug.hpp"
#include "mcLandscape.hpp"
#include "mcObject.hpp"
#include "mcPlayer.hpp"
#include "mcConfig.hpp"
#include "mcSky.hpp"
#include "mcCamera.hpp"

#define random()	rand()

//sgMat4         frustumi;

//ssgTransform  *trees[100];
//ssgTransform  *lumiukko;
//ssgTransform  *sky_dome = NULL;

Scene::Scene()
{   
	m_landscape = new Landscape();
	m_sky = new mcSky(16, 8);
	m_camera = new mcCamera();
	m_player = new Player;
	sky_dome = NULL;

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
  assert ( model );

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
Scene::redrawSky()
{
  if (sky_dome == NULL) {
		// Create entity for sky when drawing first time
	sky_dome = new ssgTransform ();
    sky_entity = m_sky->Draw();
    sky_dome->addKid (sky_entity);
    scene_root->addKid (sky_dome);
  }
  else
	m_sky->Draw();	// Update vertex colors only 
}

void
Scene::init()
{
   sgCoord crd;
   
   sgSetVec3(crd.xyz, 0, 0, 0);
   sgSetVec3(crd.hpr, 0, 0, 0);
   
  scene_root = new ssgRoot;
  
  ssgModelPath   ("data");
  ssgTexturePath ("gfx");

  m_landscape->init(scene_root);
        
  sgVec4 skycol ; sgSetVec4 ( skycol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
  sgVec4 fogcol ; sgSetVec4 ( fogcol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
	  
  glEnable ( GL_DEPTH_TEST ) ;
  
  /* Set up the viewing parameters */
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

  redrawSky();

	m_camera->setMode (mcCamera::BEHIND);
}

float
Scene::getHOT( float x, float y )
{
	return m_landscape->getRealHOT(x, y);
/*
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
  */
}

//kludge
extern double read_time_of_day ();

void
Scene::update()
{
	// some quick kludges
	static float lastTime = read_time_of_day();
	float curTime = read_time_of_day();
	float frameTime = curTime - lastTime;
	if (frameTime <= 0.0)
		frameTime = 0.01;
	lastTime = curTime;

	for (Object *o = Object::getFirst(); o; o = o->getNext())
		o->update(frameTime);

	m_camera->setTarget(m_player);

	m_camera->update(frameTime);

   static int frameno = 0 ;   
   sgVec3 scale;
  
   frameno++ ;
  
   Object *ob = Object::getFirst();
  
   sgCoord tuxpos = m_player->getPos();

   while (ob != NULL)   //TODO: move this to Object!!
     {
       sgCoord pos = ob->getPos();
       
       if ((abs((int) (pos.xyz[0] - tuxpos.xyz[0])) > 500 ||
	    abs((int) (pos.xyz[0] - tuxpos.xyz[0])) > 500 ) && 
	   ob->getFileName() == "bee.ac"  ) 
	 {
	   Object *temp = ob;
	   ob = ob->getNext();
	   delete temp;
	   continue;
	 }
       
       scale[0] = 1.0f;
       scale[1] = 1.0f;
       scale[2] = sin( (float)ob->getMoveCounter() / 2.0 ) / 4 + 1;
       
       ob->setScale( scale );
       ob = ob->getNext();
       
     }
   
   sgCoord skypos;
   sgCopyVec3 (skypos.xyz, m_camera->getPosition().xyz);
   skypos.xyz[2] -= 3500.0f;
   skypos.hpr[0] = 0.0f;
   skypos.hpr[1] = 90.0f;
   skypos.hpr[2] = 0.0f;

   if (sky_dome)
     sky_dome->setTransform (&skypos);
}

void
Scene::drawText(Object *o, sgVec3 object_pos)
{
  assert ( o );

  if (!strlen(o->getTextObject()->getLabel()))
    return;

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
  sgCoord campos = m_camera->getPosition();
  
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
  
  textpos[0] = textpos[0]*display->getWidth()/2 + display->getWidth()/2;
  textpos[1] = textpos[1]*display->getHeight()/2 + display->getHeight()/2;
  
  int slen = strlen(o->getTextObject()->getLabel())*8;
  textpos[0] -= slen / 2;

  char buf[1000];
  char *buf2 = new char[1000];
  int len = 0;
  int max_len = 0;

  memset (buf, 0, sizeof (buf));
  memset (buf2, 0, sizeof (buf2));

  strcpy (buf, o->getTextObject()->getLabel());
  strcpy (buf2, "");

  for (unsigned int i=0;i<strlen(buf);i++,len++)
    {
      if (len > max_len)
		max_len = len;
      if (len*8 < display->getWidth() && buf[i] != '\n')
		sprintf (buf2, "%s%c", buf2, buf[i]);
      else if (buf[i] != '\n')
		{
		sprintf (buf2, "%s\n%c", buf2, buf[i]);
		len = 0;
		}
    }
  
  o->setSayString(buf2);

  max_len+=8;
  max_len*=8;
  
  if (textpos[0] < 0)
    textpos[0] = 0;
  else if (textpos[0] > display->getWidth()-max_len)
    textpos[0] = display->getWidth()-max_len;

  if (textpos[1] < 0)
    textpos[1] = 0;
  else if (textpos[1] > display->getHeight()-16)
    textpos[1] = display->getHeight()-16;

  o->getTextObject()->setPosition(textpos[0], textpos[1]);
}

void
Scene::draw()
{
	// Clear screen if necessary
	if (m_wireframe)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_DEPTH_BUFFER_BIT);
  
	if (!m_player->initialized())
		return; // We do not exist yet and thus no draw() needed.
  
	update();
  
  // Draw the stuff to screen.

	m_landscape->draw(m_camera);

	ssgCullAndDraw ( scene_root );

  // Overlay chat-strings, if any.
	sgVec3 posit;
  
	for (Object *ob = Object::getFirst(); ob; ob = ob->getNext())
	{
		sgCoord obPos = ob->getPos();

		if (!ob->hotFixed)
		{
			ob->moveTo(	obPos.xyz[0],
						obPos.xyz[1],
						obPos.hpr[0]);
			ob->hotFixed = true;
		}

		posit[0] = obPos.xyz[0];
		posit[1] = obPos.xyz[1];
		posit[2] = obPos.xyz[2]+ob->getRadius()*2 - .1f;

		drawText( ob, posit);
	}
}
