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
#include "Display.hpp"
#include "Debug.hpp"
#include "Landscape.hpp"
#include "ssgKeyFlier.h"
#include "Socket.hpp"
#include "Object.hpp"
#include <pu.h>
#include <sg.h>

#define random()	rand()

sgCoord        campos;

Object         *tuxi = NULL;
sgMat4         frustumi;

ssgTransform  *trees[100];
ssgTransform  *lumiukko;


//float cam_slide = 0.0f;

Scene::Scene()
{    
   DEBUG("Scene constructor");
}


Scene::~Scene()
{
   if (scene_root != NULL)
	 delete scene_root; // hmmm
         
   DEBUG("Scene destructor");
}


void
Scene::init()
{
   scene_root = new ssgRoot;
   
   ssgModelPath   ("data");
   ssgTexturePath ("data");

   landscape->init(scene_root);
   
   ssgEntity *tree_obj = ssgLoadAC ("tree.ac" );
   
   //   ssgFlatten         ( tree_obj );
      
   float tempx, tempy;
   
   for (int i=0; i<100; i++)
	 {
		
		trees[i] = new ssgTransform;
		trees[i] -> addKid( tree_obj );
//		ssgStripify ( trees[i] );
		trees[i] -> clrTraversalMaskBits ( SSGTRAV_HOT ) ;
		
		sgCoord treepos;
		tempx = -1000 + random() % 20000;
		tempy = -1000 + random() % 12000;
		
		treepos.xyz[0] = tempx;
		treepos.xyz[1] = tempy;
		treepos.xyz[2] = getHOT(tempx, tempy);
		treepos.hpr[0] = random() % 360;
		treepos.hpr[1] = 0.0f;
		treepos.hpr[2] = 0.0f;
		
		trees[i] -> setTransform ( & treepos  );
		
		scene_root -> addKid ( trees[i] );
	 }
   
   ssgEntity *snowman_obj = ssgLoadAC ("snowman.ac" );
   
   lumiukko = new ssgTransform;
   lumiukko -> addKid( snowman_obj );
   lumiukko -> clrTraversalMaskBits ( SSGTRAV_HOT ) ;
   
   sgCoord ukkopos;
   ukkopos.xyz[0] = 100;
   ukkopos.xyz[1] = 100;
   ukkopos.xyz[2] = getHOT( 100, 100 );
   ukkopos.hpr[0] = 0.0f;
   ukkopos.hpr[1] = 0.0f;
   ukkopos.hpr[2] = 0.0f;
   
   lumiukko -> setTransform ( &ukkopos, 10.0f, 10.0f, 10.0f );
   
   scene_root -> addKid ( lumiukko );
   
   sgVec4 skycol ; sgSetVec4 ( skycol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
   sgVec4 fogcol ; sgSetVec4 ( fogcol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
   
   glClearColor ( skycol[0], skycol[1], skycol[2], skycol[3] ) ;
   
   glEnable ( GL_DEPTH_TEST ) ;
   
     /*
	  *     Set up the viewing parameters
	  *   */
   
   ssgSetFOV     ( 90.0f, 60.0f ) ;
   ssgSetNearFar ( 1.0f, 10000.0f ) ;

   float near, far, top, bottom, left, right, hfov, vfov, v, h;
   
   near = 1.0f;
   far = 10000.0f;
   h = 90.0f;
   v = 60.0f;
   
   hfov = ( h <= 0 ) ? ( v * 3.0f / 2.0f ) : h ;
   vfov = ( v <= 0 ) ? ( h * 2.0f / 3.0f ) : v ;
   
   right = near * (SGfloat) tan ( hfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
   top   = near * (SGfloat) tan ( vfov * SG_DEGREES_TO_RADIANS / SG_TWO ) ;
   left  = -right ;
   bottom   = -top   ;
   
   cout << "near: " << near;
   cout << "far: " << far;
   cout << "left: " << left;
   cout << "right: " << right;
   cout << "bottom: " << bottom;
   cout << "top: " << top << endl;
   
   cout << (frustumi[0][0] = (2*near)/(right-left)) << " ";
   cout << (frustumi[1][0] = 0.0f) << " ";
   cout << (frustumi[2][0] = (right+left)/(right-left)) << " ";
   cout << (frustumi[3][0] = 0.0f) << " " << endl;
   cout << (frustumi[0][1] = 0.0f) << " ";
   cout << (frustumi[1][1] = (2*near)/(top-bottom)) << " ";
   cout << (frustumi[2][1] = (top+bottom)/(top-bottom)) << " ";
   cout << (frustumi[3][1] = 0.0f) << " " << endl;
   cout << (frustumi[0][2] = 0.0f) << " ";
   cout << (frustumi[1][2] = 0.0f) << " ";
   cout << (frustumi[2][2] = -(far+near)/(far-near)) << " ";
   cout << (frustumi[3][2] = (-2*far*near)/(far-near)) << " " << endl;
   cout << (frustumi[0][3] = 0.0f) << " ";
   cout << (frustumi[1][3] = 0.0f) << " ";
   cout << (frustumi[2][3] = -1.0f) << " ";
   cout << (frustumi[3][3] = 0.0f) << " " << endl;
   
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


void
Scene::addObject(Object* ob)
{

      
}

void
Scene::removeObject(int id)
{

}

void
Scene::removeObject(Object *ob)
{

}


void
Scene::update()
{
   static int frameno = 0 ;
   sgCoord poo;
   
   frameno++ ;
   
   Object *ob = Object::first;
   
   while (ob != NULL)
	 {
		ob->ob_pos.xyz [ 2 ] = getHOT(ob->ob_pos.xyz [ 0 ],
									  ob->ob_pos.xyz [ 1 ] ) + 0.1f;
		ob->ob_pos.hpr [ 1 ] = 0.0f ;
		ob->ob_pos.hpr [ 2 ] = 0.0f ;
//		cout << "x: " << ob->ob_pos.xyz [ 0 ] << "y: " << ob->ob_pos.xyz [ 1 ] << "z: " << ob->ob_pos.xyz [ 2 ] << "\n";
		if (strcmp(ob->file_name, "tuxedo.ac"))
		  {
			 poo = ob->ob_pos;
			 
			 poo.hpr[0] += 180.0f;
			 ob->trans->setTransform(&poo, 1.0f, 1.0f, (sin((float)ob->movecounter/2.0))/4+1);
		
		  }
		else
		  ob->trans->setTransform( &ob->ob_pos, 1.0f, 1.0f, (sin((float)ob->movecounter/2.0))/4+1);
		
		ob = ob->next;
	 }
      
   sgVec3 test_vec ;
   sgMat4 invmat ;
   sgMakeIdentMat4 ( invmat ) ;
   
   invmat[3][0] = -tuxi->ob_pos.xyz[0] ;
   invmat[3][1] = -tuxi->ob_pos.xyz[1] ;
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
   
//   tuxpos . xyz [ 2 ] = hot + 0.1f ;
//   tuxpos . hpr [ 1 ] = 0.0f ;
 //  tuxpos . hpr [ 2 ] = 0.0f ;

//   printf("tuxpos: %f %f %f\n", tuxpos . xyz [ 0 ], tuxpos . xyz [ 1 ], tuxpos . xyz [ 2 ]);
   
   sgCopyVec3 ( campos.xyz, tuxi->ob_pos.xyz ) ;
   sgCopyVec3 ( campos.hpr, tuxi->ob_pos.hpr ) ;
//   campos . hpr [ 1 ] = 0.0f ;
//   campos . hpr [ 2 ] = 0.0f ;
   
   campos.hpr[0] += 180.0f /*+ cam_slide*/;
   
   //cam_slide *= 0.9;

   if (display->camera == 0) {
	  campos.xyz[0] -= 20*sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[1] += 20*cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);  
	  campos.xyz[2] += 5+(getHOT(campos.xyz[0], campos.xyz[1])-hot);
	  campos.hpr[1] -= 5+(getHOT(campos.xyz[0], campos.xyz[1])-hot);
   } else {
	  campos.xyz[0] += 2*sin((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[1] -= 2*cos((campos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
	  campos.xyz[2] += 2;
	  campos.hpr[1] = display->pitch;
   }
   
   ssgSetCamera ( & campos ) ;

   if (strcmp(tuxi->file_name, "tuxedo.ac"))
	 {
		poo = tuxi->ob_pos;
		
		poo.hpr[0] += 180.0f;
		tuxi->trans->setTransform(&poo, 1.0f, 1.0f, (sin((float)tuxi->movecounter/2.0))/4+1);
		
	 }	
      
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
   
//   textpos[0] = temppos[0];
//   textpos[1] = temppos[1];
//   textpos[2] = temppos[2];
//   textpos[3] = 1.0f;
  
//   cout << "textpos xform jalk: " << textpos[0] << " " << textpos[1] << " " << textpos[2] << endl;
   
   textpos[0] = frustumi[0][0] * temppos[0] +  frustumi[1][0] * temppos[1] + frustumi[2][0] * temppos[2] +  frustumi[3][0];
   textpos[1] = frustumi[0][1] * temppos[0] +  frustumi[1][1] * temppos[1] + frustumi[2][1] * temppos[2] +  frustumi[3][1];
   textpos[2] = frustumi[0][2] * temppos[0] +  frustumi[1][2] * temppos[1] + frustumi[2][2] * temppos[2] +  frustumi[3][2];
   textpos[3] = frustumi[0][3] * temppos[0] +  frustumi[1][3] * temppos[1] + frustumi[2][3] * temppos[2] +  frustumi[3][3];
   
   textpos[0] /= textpos[3];
   textpos[1] /= textpos[3];
//   textpos[2] /= textpos[3];       // mhmhmmh

//   cout << "textpos frustum jalk.: " << textpos[0] << " " << textpos[1] << " " << textpos[2] << " " << textpos[3] << endl;
   
   textpos[0] *= display->width/2;
   textpos[1] *= display->height/2;
   
   if (textpos[2] < 0 || textpos[2] > 250)
	 text_object->hide();
   else
	 text_object->reveal();
   
//   cout << "textpos proj. jalk.: " << textpos[0] + display->width/2 << " " << textpos[1] + display->height/2 << " " << textpos[2] << endl;
   
   text_object->setPosition(textpos[0]+ display->width/2, textpos[1] + display->height/2);
}
   

void
Scene::draw()
{
 //  static int frame =0;

  //  frame++;
   
//   cout << "Kohta rendaa.\n";
   ssgCullAndDraw ( scene_root ) ;

   sgVec3 posit;
   
   Object *ob = Object::first;
   
   while (ob != NULL)
	 {
		posit[0] = ob->ob_pos.xyz[0];
		posit[1] = ob->ob_pos.xyz[1];
		posit[2] = ob->ob_pos.xyz[2];
				
		drawText( ob->puhe, posit);
		
		ob = ob->next;
	 }
}
