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

#include "Scene.hpp"
#include "Debug.hpp"
#include "Landscape.hpp"
#include "ssgKeyFlier.h"

/* tux */
ssgTransform   *penguin  = NULL ;
sgCoord tuxpos, campos;

ssgTransform  *trees[100];

void 
keyboard ( unsigned char k, int, int )
{
   static int wireframe = FALSE ;
   
   if ( k == 'w' )
	 {
		wireframe = ! wireframe ;
		
		glPolygonMode ( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL ) ;
	 }
   
   if ( k == 0x03 || k == 'x' )
	 exit ( 0 ) ;
   
   switch (k) {
	case '8':
	  tuxpos.xyz[1] += 1.0f;
	  tuxpos.xyz[0] -= sin((tuxpos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  tuxpos.xyz[1] += cos((tuxpos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[0] += sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[1] += cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  break;
	case '2':
	  tuxpos.xyz[1] -= 1.0f;
	  tuxpos.xyz[0] += sin((tuxpos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  tuxpos.xyz[1] -= cos((tuxpos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[0] -= sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[1] -= cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  break;
	case '6':
	  tuxpos.hpr[0] -= 5.0f;
	  break;
	case '4':
	  tuxpos.hpr[0] += 5.0f;
	  break;
	case '+':
	  if (campos.hpr[1] > 90)
		campos.hpr[1] = 90;
	  else
		campos.hpr[1] += 1.0f;
	  break;
	case '-':
	  if (campos.hpr[1] < -90)
		campos.hpr[1] = -90;
	  else
		campos.hpr[1] -= 1.0f;
	  break;
   }
   
   //keyflier . incoming_keystroke ( k ) ;
}



Scene::Scene()
{    
   DEBUG("Scene constructor.");
}


Scene::~Scene()
{
      DEBUG("Scene destructor.");
}


void
Scene::init()
{
   scene_root = new ssgRoot;
   object_branch = new ssgTransform;

   scene_root->addKid ( object_branch);
   
   ssgModelPath   ("data");
   ssgTexturePath ("data");

   landscape->init(scene_root);
      
   /* tux */
   penguin  = new ssgTransform ;
   ssgEntity *tux_obj = ssgLoadAC ( "tuxedo.ac"   ) ;
   penguin  -> addKid ( tux_obj  ) ;
   ssgFlatten         ( tux_obj  ) ;
   ssgStripify        ( penguin  ) ;
   penguin  -> clrTraversalMaskBits ( SSGTRAV_HOT ) ;
   scene_root -> addKid ( penguin ) ;
   tuxpos.hpr[0] = 180.0f;
   /* ... */
   
    ssgEntity *tree_obj = ssgLoadAC ("tree.ac" );
   //   ssgFlatten         ( tree_obj );
      
      float tempx, tempy;
      
      for (int i=0; i<100; i++)
	 {
		
		trees[i] = new ssgTransform;
		trees[i] -> addKid( tree_obj );
		ssgStripify ( trees[i] );
		trees[i] -> clrTraversalMaskBits ( SSGTRAV_HOT ) ;
		
		
		sgCoord treepos;
		tempx = -500 + random() % 1000;
		tempy = -500 + random() % 1000;
		
		treepos.xyz[0] = tempx;
		treepos.xyz[1] = tempy;
		treepos.xyz[2] = getHOT(tempx, tempy);
		treepos.hpr[0] = 0.0f;
		treepos.hpr[1] = 0.0f;
		treepos.hpr[2] = 0.0f;
		
		trees[i] -> setTransform ( & treepos );
		
		
		scene_root -> addKid ( trees[i] );
		   
	 }
   
      
   sgVec4 skycol ; sgSetVec4 ( skycol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
   sgVec4 fogcol ; sgSetVec4 ( fogcol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
   
   glClearColor ( skycol[0], skycol[1], skycol[2], skycol[3] ) ;
   
    glEnable ( GL_DEPTH_TEST ) ;
   
     /*
	  *     Set up the viewing parameters
	  *   */
   
   ssgSetFOV     ( 60.0f, 0.0f ) ;
   ssgSetNearFar ( 1.0f, 30000.0f ) ;

    /*
	 *     Initial Position
	 *   */
   
     sgCoord startpos ;
     sgSetCoord ( &startpos, 2350.0f, -920.0f, 0.0f,   0.0f, 0.0f, 0.0f ) ;
   
    /*
	 *     *     Set up some fog
	 *     *   */
   
   
   glFogf ( GL_FOG_DENSITY, 0.015 / 100.0f ) ;
   glFogfv( GL_FOG_COLOR  , fogcol    ) ;
   glFogf ( GL_FOG_START  , 0.0       ) ;
   glFogi ( GL_FOG_MODE   , GL_EXP2   ) ;
   glHint ( GL_FOG_HINT   , GL_NICEST ) ;
   glEnable ( GL_FOG ) ;
   
   /*
	*     Set up the Sun.
	*   */
   
   sgVec3 sunposn ;
   sgVec4 sunamb  ;
   sgSetVec3 ( sunposn, 0.2f, -0.5f, 0.5f ) ;
   sgSetVec4 ( sunamb , 0.4f, 0.4f, 0.4f, 1.0f ) ;
   ssgGetLight ( 0 ) -> setPosition ( sunposn ) ;

   
      
}

float
Scene::getHOT( float x, float y )
{
   sgVec3 test_vec ;
   sgMat4 invmat ;
   sgMakeIdentMat4 ( invmat ) ;
   
   invmat[3][0] = -x ;
   invmat[3][1] = -y ;
   invmat[3][2] =  0.0f          ;
   
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


void
Scene::addObject(Object* ob)
{
   object_list.push_back(ob);
      
   object_branch->addKid ( ob-> plop );
      
}

void
Scene::removeObject(int id)
{

}

void
Scene::removeObject(Object *ob)
{
   object_branch->removeKid ( ob-> plop );
   delete ob;
}


void
Scene::update()
{
   static int frameno = 0 ;
   
   frameno++ ;
   
   sgVec3 test_vec ;
   sgMat4 invmat ;
   sgMakeIdentMat4 ( invmat ) ;
   
   invmat[3][0] = -tuxpos.xyz[0] ;
   invmat[3][1] = -tuxpos.xyz[1] ;
   invmat[3][2] =  0.0f          ;
   
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
   
   tuxpos . xyz [ 2 ] = hot + 0.1f ;
   tuxpos . hpr [ 1 ] = 0.0f ;
   tuxpos . hpr [ 2 ] = 0.0f ;
   
//   sgCopyVec3 ( campos.xyz, tuxpos.xyz ) ;
//   sgCopyVec3 ( campos.hpr, tuxpos.hpr ) ;

   campos . hpr [ 0 ] = tuxpos.hpr[0]-180.0f;
   //campos . hpr [ 1 ] = -10.0f ;
   campos . hpr [ 2 ] = 0.0f ;
   //campos . xyz [ 0 ] += 1.0f ;
   campos . xyz [ 1 ] = tuxpos.xyz[1]-30.0f ;
   campos . xyz [ 2 ] = getHOT(campos.xyz[0], campos.xyz[1])+12.0f ;
   //campos . hpr [1] = hot-getHOT(campos.xyz[0], campos.xyz[1]);
   ssgSetCamera ( & campos ) ;
   penguin -> setTransform ( & tuxpos ) ;
   
}

void
Scene::draw()
{
 //  static int frame =0;

   
 //  frame++;
   
//   sgCoord campos ;
   
//   sgSetCoord ( & campos, 0.0f, 350.0*sin((float)frame/90), 500.0f, 35.0*sin((float)frame/100), -20.0f, 0.0f ) ;
   
 //  ssgSetCamera ( & campos ) ;
   
   ssgCullAndDraw ( scene_root ) ;

}
