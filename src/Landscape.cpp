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

/*
 *  Note that much of this file is experimental code and thus yet quite uncommented. 
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Error.hpp"
#include "Display.hpp"
#include "Debug.hpp"
#include "Perlin.hpp"
#include "Texture.hpp"
#include "Landscape.hpp"

/* These should be moved into headers */
#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3

#define MAP1_WIDTH       20
#define MAP1_GRID_WIDTH   2

#define MAP2_WIDTH       20
#define MAP2_GAP          4
#define MAP2_RATIO        5
#define MAP2_GRID_WIDTH  ( MAP2_RATIO*MAP1_GRID_WIDTH )  

#define MAP3_WIDTH       20
#define MAP3_GAP          4
#define MAP3_RATIO        5
#define MAP3_GRID_WIDTH  ( MAP3_RATIO*MAP2_GRID_WIDTH )  

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)

#define MAP1(x, y) map_1[(y) * MAP1_WIDTH + (x)]
#define MAP2(x, y) map_2[(y) * MAP2_WIDTH + (x)]
#define MAP3(x, y) map_3[(y) * MAP3_WIDTH + (x)]


Landscape::Landscape()
{
   first = NULL;
   
   DEBUG("Landscape constructor");
}

Landscape::~Landscape()
{
   if(first != NULL)
	 {
	    delete first;
		first = NULL;
	 }
   
   DEBUG ("Landscape destructor");
}

void Landscape::init()
{
   float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   float light_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   float light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
   float lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   
   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_FOG);
   
   glShadeModel(GL_SMOOTH);
   
   setViewport(0, 0, display->width, display->height);
   
   map_1Mesh.numVertices = (MAP1_WIDTH+1)*(MAP1_WIDTH+1);
   map_1Mesh.vertices  =  new double[3*map_1Mesh.numVertices];
   map_1Mesh.face_normals = new P3D[(MAP1_WIDTH)*(MAP1_WIDTH)*2];
   map_1Mesh.normals = new double[3*map_1Mesh.numVertices];

   map_2Mesh.numVertices = (MAP2_WIDTH+1)*(MAP2_WIDTH+1);
   map_2Mesh.vertices  =  new double[3*map_2Mesh.numVertices];
   map_2Mesh.face_normals = new P3D[(MAP2_WIDTH)*(MAP2_WIDTH)*2];
   map_2Mesh.normals = new double[3*map_2Mesh.numVertices];
   
   map_3Mesh.numVertices = (MAP3_WIDTH+1)*(MAP3_WIDTH+1);
   map_3Mesh.vertices  =  new double[3*map_2Mesh.numVertices];
   map_3Mesh.face_normals = new P3D[(MAP3_WIDTH)*(MAP3_WIDTH)*2];
   map_3Mesh.normals = new double[3*map_3Mesh.numVertices];

   zmap_1 = new double[(MAP1_WIDTH+1)*(MAP1_WIDTH+1)];
   zmap_2 = new double[(MAP2_WIDTH+1)*(MAP2_WIDTH+1)];
   zmap_3 = new double[(MAP3_WIDTH+1)*(MAP3_WIDTH+1)];
   
   map1_x = map1_y = MAP3_WIDTH*(MAP3_GRID_WIDTH/2) - MAP1_WIDTH*(MAP1_GRID_WIDTH/2);
   map2_x = map2_y = MAP3_WIDTH*(MAP3_GRID_WIDTH/2) - MAP2_WIDTH*(MAP2_GRID_WIDTH/2);
   map3_x = map3_y = 0;
   
   listId_1 = -1;
   listId_2 = -1;
   listId_3 = -1;
   listId_4 = -1;

   map1_shift_x = 0;
   map1_shift_y = 0;
   map2_shift_x = 0;
   map2_shift_y = 0;

   angle = 75;
   tilt = 15;
   distance = -20; 
   sun_pos = 0;
   
   makeHeightMaps();
   
   initMap_1Mesh();
   makeMap_1();

   initMap_2Mesh();
   makeMap_2();
   
   initMap_3Mesh();
   makeMap_3();
   
//   glEnable(GL_FOG);
   
	 {
		GLfloat fogColor[4] =
		  {0.5, 0.5, 0.5, 1.0};
		
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.005);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glClearColor(0.0, 0.0, 1.0, 1.0);
	 }
   
   glFogf(GL_FOG_START, 400.0);
   glFogf(GL_FOG_END, 7000.0);

   /* THE FOLLOWING IS MEANT TO BE VERY TEMPORARY */
   
   playerId = glGenLists(1);
   glNewList(playerId, GL_COMPILE);
   glDisable(GL_CULL_FACE);
   glBegin(GL_QUADS);
	 {
		float color[4] = { 1.0, 1.0, 1.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		
		glNormal3f( 0.0, 1.0, 0.0);
		glTexCoord2f(0.00, 0.25); glVertex3f( -1.0, -1.0, 0.0);
		glTexCoord2f(0.00, 0.50); glVertex3f( -1.0, -1.0, 2.0);
		glTexCoord2f(0.25, 0.50); glVertex3f( 1.0, -1.0, 2.0);
		glTexCoord2f(0.25, 0.25); glVertex3f( 1.0, -1.0, 0.0);
		
		glNormal3f( -1.0, 0.0, 0.0);
		glTexCoord2f(0.25, 0.25); glVertex3f( 1.0, -1.0, 0.0);
		glTexCoord2f(0.25, 0.50); glVertex3f( 1.0, -1.0, 2.0);
		glTexCoord2f(0.50, 0.50); glVertex3f( 1.0, 1.0, 2.0);
		glTexCoord2f(0.50, 0.25); glVertex3f( 1.0, 1.0, 0.0);

		glNormal3f( 0.0, -1.0, 0.0);
		glTexCoord2f(0.50, 0.25); glVertex3f( 1.0, 1.0, 0.0);
		glTexCoord2f(0.50, 0.50); glVertex3f( 1.0, 1.0, 2.0);
		glTexCoord2f(0.75, 0.50); glVertex3f( -1.0, 1.0, 2.0);
		glTexCoord2f(0.75, 0.25); glVertex3f( -1.0, 1.0, 0.0);
		
		glNormal3f( 1.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.25); glVertex3f( -1.0, 1.0, 0.0);
		glTexCoord2f(1.0, 0.50); glVertex3f( -1.0, 1.0, 2.0);
		glTexCoord2f(0.75, 0.50); glVertex3f( -1.0, -1.0, 2.0);
		glTexCoord2f(0.75, 0.25); glVertex3f( -1.0, -1.0, 0.0);
		
		glNormal3f( 0.0, 0.0, -1.0);
		glTexCoord2f(0.25, 0.75); glVertex3f( -1.0, -1.0, 2.0);
		glTexCoord2f(0.25, 0.50); glVertex3f( -1.0, 1.0, 2.0);
		glTexCoord2f(0.50, 0.50); glVertex3f( 1.0, 1.0, 2.0);
		glTexCoord2f(0.50, 0.75); glVertex3f( 1.0, -1.0, 2.0);
		
		glNormal3f( 0.0, 0.0, 1.0);
		glTexCoord2f(0.50, 0.25); glVertex3f( 1.0, 1.0, 0.0);
		glTexCoord2f(0.25, 0.25); glVertex3f( 1.0, -1.0, 0.0);
		glTexCoord2f(0.25, 0.00); glVertex3f( -1.0, -1.0, 0.0);
		glTexCoord2f(0.50, 0.00); glVertex3f( -1.0, 1.0, 0.0);
	 }
   glEnd();
   glEnable(GL_CULL_FACE);
   glEndList();
   
   int i, k;
   Texture *picture;
   
   /* Load textures. FIXME: loadTexture shouldn't return the id instantly,
	* it would be nicer to be able to fetch it using findTexture, so we
	* wouldn't need to assign those ids as global */
   
   picture = new Texture;
   grassTex_id = picture->loadTexture("gfx/grass.png");
   
   picture = new Texture;
   sandstoneTex_id = picture->loadTexture("gfx/sandstone.png");

   picture = new Texture;
   playerTex_id = picture->loadTexture("gfx/ukkelipukkeli.png");
}

/* This should be cut into several smaller functions */
void Landscape::makeHeightMaps()
{
   int i, j;
   double *temp;
   Perlin *perl = new Perlin;
   
   temp = zmap_3;
   
   for (j = 0; j <= MAP3_WIDTH; j++) 
	 {
		for (i = 0; i <= MAP3_WIDTH; i++)
		  {
			 *(temp++) = perl->perlinNoise_2D((double)(map3_x+i*MAP3_GRID_WIDTH)/300, 
											  (double)(map3_y+j*MAP3_GRID_WIDTH)/300)*200;
		  } 
	 }
   
   temp = zmap_2;
   for (i = 0; i <= MAP2_WIDTH; i++)
	 {
		*(temp++) = getHeight( map2_x + i*MAP2_GRID_WIDTH, map2_y);
	 }
   
   for (j = 1; j < MAP2_WIDTH; j++)
	 {
		*(temp++) = getHeight( map2_x, map2_y + j*MAP2_GRID_WIDTH);
		
		for (i = 1; i < MAP2_WIDTH; i++)
		  {
			 *(temp++) = perl->perlinNoise_2D((double)(map2_x+i*MAP2_GRID_WIDTH)/300, 
											  (double)(map2_y+j*MAP2_GRID_WIDTH)/300)*200;
		  } 
		
		*(temp++) = getHeight( map2_x + MAP2_GRID_WIDTH*MAP2_WIDTH, map2_y + j*MAP2_GRID_WIDTH);
	 }
   for (i = 0; i <= MAP2_WIDTH; i++)
	 {
		*(temp++) = getHeight( map2_x + i*MAP2_GRID_WIDTH, map2_y + MAP2_GRID_WIDTH*MAP2_WIDTH);
	 }
   
   double temp2;
   temp = zmap_1;
   for (i = 0; i <= MAP1_WIDTH; i++)
	 {
		*(temp++) = getHeight( map1_x + i*MAP1_GRID_WIDTH, map1_y);
	 }
   
   for (j = 1; j < MAP1_WIDTH; j++)
	 {
		*(temp++) = getHeight( map1_x, map1_y + j*MAP1_GRID_WIDTH);
		
		for (i = 1; i < MAP1_WIDTH; i++)
		  {
			 *(temp++) = perl->perlinNoise_2D((double)(map1_x+i*MAP1_GRID_WIDTH)/300,
											  (double)(map1_y+j*MAP1_GRID_WIDTH)/300)*200;
		  }
		
		*(temp++) = getHeight( map1_x + MAP1_GRID_WIDTH*MAP1_WIDTH, map1_y + j*MAP1_GRID_WIDTH);
	 }
   for (i = 0; i <= MAP1_WIDTH; i++)
	 {
		*(temp++) = getHeight( map1_x + i*MAP1_GRID_WIDTH, map1_y + MAP1_GRID_WIDTH*MAP1_WIDTH);
	 }
   
   delete(perl);
}


void Landscape::draw()
{
   float light_position[] = { 0.0, 0.0, 1.0, 0.0 };
   /* move viewport & perspective setting into the Display.cpp, maybe */
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -viewport_ratio, viewport_ratio, 3.0, 10000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   
   glPushMatrix();
   
   /* Load camera matrix here            */
//   gluLookAt(0.0, -500.0, -200, 
//			 0.0, 100.0, getHeight(map1_x + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2, map1_y + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2), 0.0, 0.0, -1.0);   
//   glTranslatef(0.0, -300.0, -200.0);
//   glRotatef(135.0, 1.0, 0.0, 0.0);
   glTranslatef(0.0, 0.0, distance);
   glRotatef(90+tilt, 1.0, 0.0, 0.0);
   glRotatef(angle, 0.0, 0.0, 1.0);
   glTranslatef(0.0, 0.0, - getHeight(map1_x + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2, map1_y + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2)+2);
   
   /* Position sun                       */
   glPushMatrix();
   glRotatef(sun_pos, 0.0, 1.0, 0.0);
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);
   glPopMatrix();
   
   /* Draw sky                           */
   /* - Check for detail settings        */

   
   /* Draw terrain                       */
   /*  - Check for detail settings       */
   /*  - Cull map regions                */
   
   if (!display->noTexture) {
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D, sandstoneTex_id);
   }
   glPushMatrix();
   //   glTranslatef(0.0, 0.0, -500);
   glCallList(listId_1);
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
   
   if (!display->noTexture) {
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D, grassTex_id);   
   }
   glPushMatrix();
   
   glTranslatef(-map1_shift_x*MAP1_GRID_WIDTH, map1_shift_y*MAP1_GRID_WIDTH, 0.0);
   
   glCallList(listId_2);

   glTranslatef(-map2_shift_x*MAP2_GRID_WIDTH, map2_shift_y*MAP2_GRID_WIDTH, 0.0);
   
   glCallList(listId_3);

   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
   
   /* Draw objects                       */
   /*  - Check for 1st-person/3rd-person */
   /*  - Check for detail settings       */

   if (!display->noTexture) {
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D, playerTex_id);
   }
   glPushMatrix();
   glTranslatef(0.0, 0.0, getHeight(map1_x + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2, map1_y + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2)-2.5);
   glRotatef( Perlin::perlinNoise_2D(0, counter/5)*50, 1.0, 0.0, 0.0);
   glCallList(playerId);
   glPopMatrix();
   
   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
   
   counter += 0.1;
}

void Landscape::setViewport(int x, int y, int w, int h)
{
   viewport_x = x;
   viewport_y = y;
   viewport_w = w;
   viewport_h = h;
   viewport_ratio = (float)h/(float)w;

}

double Landscape::interpolate(double a, double b, double x)
{
   return (1-x)*a + x*b;
}

/* Gives z coordinate at given (x, y) */
double Landscape::getHeight(int x, int y) 
{
   int tempx, tempy;
   
//   debug->put("%d %d", x, y);
   tempx = x - map1_x;
   tempy = y - map1_y;
	 
   if (tempx > 0 && tempx < MAP1_WIDTH*MAP1_GRID_WIDTH && 
	   tempy > 0 && tempy < MAP1_WIDTH*MAP1_GRID_WIDTH)
	 {           
		int ax = tempx / MAP1_GRID_WIDTH;
		int fx = tempx % MAP1_GRID_WIDTH;
		int ay = tempy / MAP1_GRID_WIDTH;
		int fy = tempy % MAP1_GRID_WIDTH;
		double x1, x2;
		
		if (fx == 0)
		  x1 = zmap_1[ay*(MAP1_WIDTH+1) + ax];
		else
		  {
			 int bx = ax + 1;
			 x1 = interpolate(zmap_1[ay*(MAP1_WIDTH+1) + ax],
							  zmap_1[ay*(MAP1_WIDTH+1) + bx],
							  (double)fx / MAP1_GRID_WIDTH);
		  }
		
		if (fy == 0)
		  return x1;
		else
		  {
			 int by = ay +1;
			 if (fx == 0)
			   x2 = zmap_1[by*(MAP1_WIDTH+1) + ax];
			 else
			   {
				  int bx = ax + 1;
				  x2 = interpolate(zmap_1[by*(MAP1_WIDTH+1) + ax],
								   zmap_1[by*(MAP1_WIDTH+1) + bx],
								   (double)fx / MAP1_GRID_WIDTH);
			   }
			 return interpolate(x1, x2, (double)fy / MAP1_GRID_WIDTH);
		  }
	 }
   else 
	 {
		tempx = x - map2_x;
		tempy = y - map2_y;
		
		   if (tempx > 0 && tempx < MAP2_WIDTH*MAP2_GRID_WIDTH &&
			   tempy > 0 && tempy < MAP2_WIDTH*MAP2_GRID_WIDTH)
		  {

			 int ax = tempx / MAP2_GRID_WIDTH;
			 int fx = tempx % MAP2_GRID_WIDTH;
			 int ay = tempy / MAP2_GRID_WIDTH;
			 int fy = tempy % MAP2_GRID_WIDTH;
			 double x1, x2;
			 
			 if (fx == 0)
			   x1 = zmap_2[ay*(MAP2_WIDTH+1) + ax];
			 else
			   {
				  int bx = ax + 1;
				  x1 = interpolate(zmap_2[ay*(MAP2_WIDTH+1) + ax], 
								   zmap_2[ay*(MAP2_WIDTH+1) + bx], 
								   (double)fx / MAP2_GRID_WIDTH);
			   }
			 
			 if (fy == 0)
			   return x1;
			 else
			   {
				  int by = ay +1;
				  if (fx == 0)
					x2 = zmap_2[by*(MAP2_WIDTH+1) + ax];
				  else
					{
					   int bx = ax + 1;
					   x2 = interpolate(zmap_2[by*(MAP2_WIDTH+1) + ax],
										zmap_2[by*(MAP2_WIDTH+1) + bx],
										(double)fx / MAP2_GRID_WIDTH);
					}
				  return interpolate(x1, x2, (double)fy / MAP2_GRID_WIDTH);
			   }
		  }
		else
		  {
			 tempx = x - map3_x;
			 tempy = y - map3_y;
//			 debug->put("%d %d", tempx, tempy);
			 
			 if (tempx >= 0 && tempx <= MAP3_WIDTH*MAP3_GRID_WIDTH &&
				 tempy >= 0 && tempy <= MAP3_WIDTH*MAP3_GRID_WIDTH)
			   {
//				  debug->put("%d %d", tempx, MAP3_GRID_WIDTH);
				  int ax = tempx / MAP3_GRID_WIDTH;
				  int fx = tempx % MAP3_GRID_WIDTH;
				  int ay = tempy / MAP3_GRID_WIDTH;
				  int fy = tempy % MAP3_GRID_WIDTH;
				  double x1, x2;
				  
//				  debug->put("%d %d %d %d %d %d", x, y, ax, fx, ay, fy);
				  if (fx == 0)
					x1 = zmap_3[ay*(MAP3_WIDTH+1) + ax];
				  else
					{
					   int bx = ax + 1;
					   x1 = interpolate(zmap_3[ay*(MAP3_WIDTH+1) + ax],
										zmap_3[ay*(MAP3_WIDTH+1) + bx],
										(double)fx / MAP3_GRID_WIDTH);
					}
				  if (fy == 0)
					{
//					   debug->put("%f", x1);
					   return x1;
					}
				  else
					{
					   int by = ay +1;
					   if (fx == 0)
						 x2 = zmap_3[by*(MAP3_WIDTH+1) + ax];
					   else
						 {
							int bx = ax + 1;
							x2 = interpolate(zmap_3[by*(MAP3_WIDTH+1) + ax],
											 zmap_3[by*(MAP3_WIDTH+1) + bx],
											 (double)fx / MAP3_GRID_WIDTH);
						 }
					   return interpolate(x1, x2, (double)fy / MAP3_GRID_WIDTH);
					}
			   }
			 else
			   {
				  error->put(ERROR_WARNING, "getHeight() values out of range");
				  return 0;
			   }
		  }
	 }
}
   
/* The following functions calculate the vertices and normals */
void Landscape::initMap_1Mesh()
{
   double *temp;
   int k = 0;
   
   int tempx, tempy;
   temp = map_1Mesh.vertices;
   for (int j= - MAP1_WIDTH/2, tempy = 0; j<=MAP1_WIDTH/2; j++, tempy++) {
	  for (int i= - MAP1_WIDTH/2, tempx = 0; i<=MAP1_WIDTH/2; i++, tempx++)
		{
		   *(temp++) = i*MAP1_GRID_WIDTH;
		   *(temp++) = j*MAP1_GRID_WIDTH;
		   *(temp++) = zmap_1[tempy*(MAP1_WIDTH+1) + tempx];
//		   *(temp++) = random() % 20;
		}
   }
   
   temp = map_1Mesh.vertices;
   for (int j = 0; j < MAP1_WIDTH; j++) { 
	  for (int i = 0; i < MAP1_WIDTH; i++)
		{
		   /* This is somewhat optimized way to calculate normals for tris with two constant
			* sides. If the constant sides are A, normal = ( A(Z3-Z1), A(Z2-Z1), A*A )
			*/
		   map_1Mesh.face_normals[k++].set(-MAP1_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   -MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+2)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);

		   map_1Mesh.face_normals[k++].set(MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*(*(temp+5) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);
		   temp += 3;

		}
	  temp += 3;
   }
   
   temp = map_1Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_1Mesh.face_normals[0].x;
   *(temp++) = map_1Mesh.face_normals[0].y;
   *(temp++) = map_1Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP1_WIDTH; i++)
	 {
		*(temp++) = map_1Mesh.face_normals[i*2-2].x + 
		  map_1Mesh.face_normals[i*2-1].x + 
		  map_1Mesh.face_normals[i*2].x;
		*(temp++) = map_1Mesh.face_normals[i*2-2].y + 
		  map_1Mesh.face_normals[i*2-1].y + 
		  map_1Mesh.face_normals[i*2].y;
		*(temp++) = map_1Mesh.face_normals[i*2-2].z + 
		  map_1Mesh.face_normals[i*2-1].z + 
		  map_1Mesh.face_normals[i*2].z;
		//      Debug("%f %f %f", map_1Mesh.normals[k-1].x, map_1Mesh.normals[k-1].y, map_1Mesh.normals[k-1].z);
	 }
   
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2-2].x + map_1Mesh.face_normals[MAP1_WIDTH*2-1].x;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2-2].y + map_1Mesh.face_normals[MAP1_WIDTH*2-1].y;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2-2].z + map_1Mesh.face_normals[MAP1_WIDTH*2-1].z;
   
   
   for (int j = 1; j < MAP1_WIDTH; j++)
	 {
		
		*(temp++) = map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)].x +
		  map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)+1].x +
		  map_1Mesh.face_normals[(j)*(MAP1_WIDTH*2)].x;
		*(temp++) = map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)].y +
		  map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)+1].y +
		  map_1Mesh.face_normals[(j)*(MAP1_WIDTH*2)].y;
		
		*(temp++) = map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)].z +
		  map_1Mesh.face_normals[(j-1)*(MAP1_WIDTH*2)+1].z +
		  map_1Mesh.face_normals[(j)*(MAP1_WIDTH*2)].z;
		
		for (int i = 1; i < MAP1_WIDTH; i++)
		  {
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2-1)].x +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2)].x +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2+1)].x +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-2)].x +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-1)].x +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2)].x;
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2-1)].y +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2)].y +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2+1)].y +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-2)].y +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-1)].y +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2)].y;
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2-1)].z +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2)].z +
			   map_1Mesh.face_normals[(j-1)*MAP1_WIDTH*2 + (i*2+1)].z +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-2)].z +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2-1)].z +
			   map_1Mesh.face_normals[(j)*MAP1_WIDTH*2 + (i*2)].z;
		  }
		
		*(temp++) = map_1Mesh.face_normals[(j)*MAP1_WIDTH*2-1].x +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-2].x +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-1].x;
		
		*(temp++) = map_1Mesh.face_normals[(j)*MAP1_WIDTH*2-1].y +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-2].y +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-1].y;
		
		*(temp++) = map_1Mesh.face_normals[(j)*MAP1_WIDTH*2-1].z +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-2].z +
		  map_1Mesh.face_normals[(j+1)*MAP1_WIDTH*2-1].z;
		
	 }
   
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)].x +
	 map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+1].x;
   
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)].y +
	 map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+1].y;
   
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)].z +
	 map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+1].z;
   
   for (int i = 1; i < MAP1_WIDTH; i++)
	 {
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2-1)].x +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2)].x +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2+1)].x;
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2-1)].y +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2)].y +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2+1)].y;
		
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2-1)].z +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2)].z +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i*2+1)].z;
		
	 }
   
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].x;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].y;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].z;
   

   double length;
   
   for (int i=0; i<map_1Mesh.numVertices*3; )
	 {
		length = sqrt(map_1Mesh.normals[i]*map_1Mesh.normals[i] + 
					  map_1Mesh.normals[i+1]*map_1Mesh.normals[i+1] +
					  map_1Mesh.normals[i+2]*map_1Mesh.normals[i+2]);
		map_1Mesh.normals[i++] /= length;
		map_1Mesh.normals[i++] /= length;
		map_1Mesh.normals[i++] /= length;
		
//		Debug("%f %f %f", map_1Mesh.normals[i-3], map_1Mesh.normals[i-2], map_1Mesh.normals[i-1]);
		
	 }
   
}

void Landscape::initMap_2Mesh()
{
   double *temp;
   int tempx, tempy;
   
   temp = map_2Mesh.vertices;
   for (int j= - MAP2_WIDTH/2, tempy = 0; j<=MAP2_WIDTH/2; j++, tempy++) {
	  for (int i= - MAP2_WIDTH/2, tempx = 0; i<=MAP2_WIDTH/2; i++, tempx++)
		{
		   *(temp++) = i*MAP2_GRID_WIDTH;
		   *(temp++) = j*MAP2_GRID_WIDTH;
//		   *(temp++) = perl->perlinNoise_2D((double)i/4, (double)j/4)*500;
		   *(temp++) = zmap_2[tempy*(MAP2_WIDTH+1) + tempx];
		}
   }
   
   
   int k = 0;
   
   temp = map_2Mesh.vertices;
   for (int j = 0; j < MAP2_WIDTH; j++) {
	  for (int i = 0; i < MAP2_WIDTH; i++)
		{
		   map_2Mesh.face_normals[k++].set(-MAP2_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   -MAP2_GRID_WIDTH*(*(temp+(MAP2_WIDTH+1)*3+2) - *(temp+2)),
										   MAP2_GRID_WIDTH*MAP2_GRID_WIDTH);

//		   debug->put("%f %f %f", map_2Mesh.face_normals[k-1].x, map_2Mesh.face_normals[k-1].y, map_2Mesh.face_normals[k-1].z);
		   map_2Mesh.face_normals[k++].set(MAP2_GRID_WIDTH*(*(temp+(MAP2_WIDTH+1)*3+2) - *(temp+(MAP2_WIDTH+1)*3+5)),
										   MAP2_GRID_WIDTH*(*(temp+5) - *(temp+(MAP2_WIDTH+1)*3+5)),
										   MAP2_GRID_WIDTH*MAP2_GRID_WIDTH);
		   temp += 3;
//		   debug->put("%f %f %f", map_2Mesh.face_normals[k-1].x, map_2Mesh.face_normals[k-1].y, map_2Mesh.face_normals[k-1].z);

		}
	  temp += 3;
   }
   
   temp = map_2Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_2Mesh.face_normals[0].x;
   *(temp++) = map_2Mesh.face_normals[0].y;
   *(temp++) = map_2Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP2_WIDTH; i++)
	 {
		*(temp++) = map_2Mesh.face_normals[i*2-2].x + 
		  map_2Mesh.face_normals[i*2-1].x + 
		  map_2Mesh.face_normals[i*2].x;
		*(temp++) = map_2Mesh.face_normals[i*2-2].y + 
		  map_2Mesh.face_normals[i*2-1].y + 
		  map_2Mesh.face_normals[i*2].y;
		*(temp++) = map_2Mesh.face_normals[i*2-2].z + 
		  map_2Mesh.face_normals[i*2-1].z + 
		  map_2Mesh.face_normals[i*2].z;
		//      Debug("%f %f %f", map_2Mesh.normals[k-1].x, map_2Mesh.normals[k-1].y, map_2Mesh.normals[k-1].z);
	 }
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2-2].x + map_2Mesh.face_normals[MAP2_WIDTH*2-1].x;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2-2].y + map_2Mesh.face_normals[MAP2_WIDTH*2-1].y;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2-2].z + map_2Mesh.face_normals[MAP2_WIDTH*2-1].z;
   
   
   for (int j = 1; j < MAP2_WIDTH; j++)
	 {
		
		*(temp++) = map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)].x +
		  map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)+1].x +
		  map_2Mesh.face_normals[(j)*(MAP2_WIDTH*2)].x;
		*(temp++) = map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)].y +
		  map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)+1].y +
		  map_2Mesh.face_normals[(j)*(MAP2_WIDTH*2)].y;
		
		*(temp++) = map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)].z +
		  map_2Mesh.face_normals[(j-1)*(MAP2_WIDTH*2)+1].z +
		  map_2Mesh.face_normals[(j)*(MAP2_WIDTH*2)].z;
		
		for (int i = 1; i < MAP2_WIDTH; i++)
		  {
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2-1)].x +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2)].x +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2+1)].x +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-2)].x +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-1)].x +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2)].x;
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2-1)].y +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2)].y +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2+1)].y +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-2)].y +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-1)].y +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2)].y;
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2-1)].z +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2)].z +
			   map_2Mesh.face_normals[(j-1)*MAP2_WIDTH*2 + (i*2+1)].z +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-2)].z +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2-1)].z +
			   map_2Mesh.face_normals[(j)*MAP2_WIDTH*2 + (i*2)].z;
		  }
		
		*(temp++) = map_2Mesh.face_normals[(j)*MAP2_WIDTH*2-1].x +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-2].x +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-1].x;
		
		*(temp++) = map_2Mesh.face_normals[(j)*MAP2_WIDTH*2-1].y +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-2].y +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-1].y;
		
		*(temp++) = map_2Mesh.face_normals[(j)*MAP2_WIDTH*2-1].z +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-2].z +
		  map_2Mesh.face_normals[(j+1)*MAP2_WIDTH*2-1].z;
		
	 }
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)].x +
	 map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+1].x;
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)].y +
	 map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+1].y;
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)].z +
	 map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+1].z;
   
   for (int i = 1; i < MAP2_WIDTH; i++)
	 {
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2-1)].x +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2)].x +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2+1)].x;
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2-1)].y +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2)].y +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2+1)].y;
		
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2-1)].z +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2)].z +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i*2+1)].z;
		
	 }
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].x;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].y;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].z;
   
   
   double length;
   
   for (int i=0; i<map_2Mesh.numVertices*3; )
	 {
		length = sqrt(map_2Mesh.normals[i]*map_2Mesh.normals[i] +
					  map_2Mesh.normals[i+1]*map_2Mesh.normals[i+1] +
					  map_2Mesh.normals[i+2]*map_2Mesh.normals[i+2]);
		map_2Mesh.normals[i++] /= length;
		map_2Mesh.normals[i++] /= length;
		map_2Mesh.normals[i++] /= length;
		
		//      Debug("%f %f %f", map_2Mesh.normals[i-3], map_2Mesh.normals[i-2], map_2Mesh.normals[i-1]);
		
	 }
   
}


void Landscape::initMap_3Mesh()
{
   double *temp;
   int tempx, tempy;
   
   temp = map_3Mesh.vertices;
   for (int j= - MAP3_WIDTH/2, tempy = 0; j<=MAP3_WIDTH/2; j++, tempy++) {
	  for (int i= - MAP3_WIDTH/2, tempx = 0; i<=MAP3_WIDTH/2; i++, tempx++)
		{
		   *(temp++) = i*MAP3_GRID_WIDTH;
		   *(temp++) = j*MAP3_GRID_WIDTH;
		   *(temp++) = zmap_3[tempy*(MAP3_WIDTH+1) + tempx];
		}
   }
   
   
   int k = 0;
   
   temp = map_3Mesh.vertices;
   for (int j = 0; j < MAP3_WIDTH; j++) {
	  for (int i = 0; i < MAP3_WIDTH; i++)
		{
		   map_3Mesh.face_normals[k++].set(-MAP3_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   -MAP3_GRID_WIDTH*(*(temp+(MAP3_WIDTH+1)*3+2) - *(temp+2)),
										   MAP3_GRID_WIDTH*MAP3_GRID_WIDTH);

//		   debug->put("%f %f %f", map_3Mesh.face_normals[k-1].x, map_3Mesh.face_normals[k-1].y, map_3Mesh.face_normals[k-1].z);
		   map_3Mesh.face_normals[k++].set(MAP3_GRID_WIDTH*(*(temp+(MAP3_WIDTH+1)*3+2) - *(temp+(MAP3_WIDTH+1)*3+5)),
										   MAP3_GRID_WIDTH*(*(temp+5) - *(temp+(MAP3_WIDTH+1)*3+5)),
										   MAP3_GRID_WIDTH*MAP3_GRID_WIDTH);
		   temp += 3;
//		   debug->put("%f %f %f", map_3Mesh.face_normals[k-1].x, map_3Mesh.face_normals[k-1].y, map_3Mesh.face_normals[k-1].z);

		}
	  temp += 3;
   }
   
   temp = map_3Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_3Mesh.face_normals[0].x;
   *(temp++) = map_3Mesh.face_normals[0].y;
   *(temp++) = map_3Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP3_WIDTH; i++)
	 {
		*(temp++) = map_3Mesh.face_normals[i*2-2].x + 
		  map_3Mesh.face_normals[i*2-1].x + 
		  map_3Mesh.face_normals[i*2].x;
		*(temp++) = map_3Mesh.face_normals[i*2-2].y + 
		  map_3Mesh.face_normals[i*2-1].y + 
		  map_3Mesh.face_normals[i*2].y;
		*(temp++) = map_3Mesh.face_normals[i*2-2].z + 
		  map_3Mesh.face_normals[i*2-1].z + 
		  map_3Mesh.face_normals[i*2].z;
		//      Debug("%f %f %f", map_3Mesh.normals[k-1].x, map_3Mesh.normals[k-1].y, map_3Mesh.normals[k-1].z);
	 }
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2-2].x + map_3Mesh.face_normals[MAP3_WIDTH*2-1].x;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2-2].y + map_3Mesh.face_normals[MAP3_WIDTH*2-1].y;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2-2].z + map_3Mesh.face_normals[MAP3_WIDTH*2-1].z;
   
   
   for (int j = 1; j < MAP3_WIDTH; j++)
	 {
		
		*(temp++) = map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)].x +
		  map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)+1].x +
		  map_3Mesh.face_normals[(j)*(MAP3_WIDTH*2)].x;
		*(temp++) = map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)].y +
		  map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)+1].y +
		  map_3Mesh.face_normals[(j)*(MAP3_WIDTH*2)].y;
		
		*(temp++) = map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)].z +
		  map_3Mesh.face_normals[(j-1)*(MAP3_WIDTH*2)+1].z +
		  map_3Mesh.face_normals[(j)*(MAP3_WIDTH*2)].z;
		
		for (int i = 1; i < MAP3_WIDTH; i++)
		  {
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2-1)].x +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2)].x +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2+1)].x +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-2)].x +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-1)].x +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2)].x;
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2-1)].y +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2)].y +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2+1)].y +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-2)].y +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-1)].y +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2)].y;
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2-1)].z +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2)].z +
			   map_3Mesh.face_normals[(j-1)*MAP3_WIDTH*2 + (i*2+1)].z +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-2)].z +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2-1)].z +
			   map_3Mesh.face_normals[(j)*MAP3_WIDTH*2 + (i*2)].z;
		  }
		
		*(temp++) = map_3Mesh.face_normals[(j)*MAP3_WIDTH*2-1].x +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-2].x +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-1].x;
		
		*(temp++) = map_3Mesh.face_normals[(j)*MAP3_WIDTH*2-1].y +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-2].y +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-1].y;
		
		*(temp++) = map_3Mesh.face_normals[(j)*MAP3_WIDTH*2-1].z +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-2].z +
		  map_3Mesh.face_normals[(j+1)*MAP3_WIDTH*2-1].z;
		
	 }
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)].x +
	 map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+1].x;
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)].y +
	 map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+1].y;
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)].z +
	 map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+1].z;
   
   for (int i = 1; i < MAP3_WIDTH; i++)
	 {
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2-1)].x +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2)].x +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2+1)].x;
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2-1)].y +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2)].y +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2+1)].y;
		
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2-1)].z +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2)].z +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i*2+1)].z;
		
	 }
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].x;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].y;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].z;
   
   double length;
   
   for (int i=0; i<map_3Mesh.numVertices*3; )
	 {
		length = sqrt(map_3Mesh.normals[i]*map_3Mesh.normals[i] +
					  map_3Mesh.normals[i+1]*map_3Mesh.normals[i+1] +
					  map_3Mesh.normals[i+2]*map_3Mesh.normals[i+2]);
		map_3Mesh.normals[i++] /= length;
		map_3Mesh.normals[i++] /= length;
		map_3Mesh.normals[i++] /= length;
		
//		debug->put("%f %f %f", map_3Mesh.normals[i-3], map_3Mesh.normals[i-2], map_3Mesh.normals[i-1]);
		
	 }
   
}

/* These construct the display lists */
void Landscape::makeMap_1()
{
   float color[4] = { 0.7, 0.7, 0.0, 1.0 };
 
   /* Check for already existing display list */
   if (listId_1 != -1)
	 glDeleteLists(listId_1, 1);

   glVertexPointer(3, GL_DOUBLE, 0, map_1Mesh.vertices);
   glNormalPointer(GL_DOUBLE, 0, map_1Mesh.normals);

   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   
   /* Generate new list ID */
   listId_1 = glGenLists(1);
   glNewList(listId_1, GL_COMPILE);

   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

   for (int j=0;j<MAP1_WIDTH;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (int i=0;i<MAP1_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP1_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP1_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   glEndList();

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   
}

void Landscape::makeMap_2()
{
   float color[4] = { 0.0, 1.0, 0.0, 1.0  };
   
   /* Check for already existing display list */
   if (listId_2 != -1)
	 glDeleteLists(listId_2, 1);
   
   glVertexPointer(3, GL_DOUBLE, 0, map_2Mesh.vertices);
   glNormalPointer(GL_DOUBLE, 0, map_2Mesh.normals);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   
   /* Generate new list ID */
   listId_2 = glGenLists(1);
   glNewList(listId_2, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
   
   int j, i, north = 0, south = 0, west = 0, east = 0;
   
   if (map1_shift_x > 0)
	 east = 1;
   else if (map1_shift_x < 0)
	 west = 1;
   
   if (map1_shift_y > 0)
	 north = 1;
   else if (map1_shift_y < 0)
	 south = 1;
   
   for (j=0;j<(MAP2_WIDTH/2 - MAP2_GAP/2)-north;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP2_WIDTH+1;i++)
		  {
             glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP2_WIDTH/2 - MAP2_GAP/2 + MAP2_GAP)+south;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP2_WIDTH/2 - MAP2_GAP/2)+1-west;i++)
		  {
             glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
             glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP2_GAP-1+east+west;i<MAP2_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
		
	 }

   for (;j<MAP2_WIDTH;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP2_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
	 }
   glEndList();
   
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

}

void Landscape::makeMap_3()
{
   float color[4] = { 0.0, 1.0, 0.0, 1.0 };
   
   /* Check for already existing display list */
   if (listId_3 != -1)
	 glDeleteLists(listId_3, 1);
   
   glVertexPointer(3, GL_DOUBLE, 0, map_3Mesh.vertices);
   glNormalPointer(GL_DOUBLE, 0, map_3Mesh.normals);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   
   /* Generate new list ID */
   listId_3 = glGenLists(1);
   glNewList(listId_3, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
   
   int j, i, north = 0, south = 0, west = 0, east = 0;
   
   if (map2_shift_x > 0)
	 east = 1;
   else if (map2_shift_x < 0)
	 west = 1;
   
   if (map2_shift_y > 0)
	 north = 1;
   else if (map2_shift_y < 0)
	 south = 1;
   
   for (j=0;j<(MAP3_WIDTH/2 - MAP3_GAP/2)-north;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP3_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP3_WIDTH/2 - MAP3_GAP/2 + MAP3_GAP)+south;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP3_WIDTH/2 - MAP3_GAP/2)+1-west;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP3_GAP-1+east+west;i<MAP3_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
		
	 }
   
   for (;j<MAP3_WIDTH;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP3_WIDTH+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   glEndList();
   
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
}
   
   
void Landscape::makeMap_4()
{
}

void
Landscape::setMap_1(char *map)
{
   
}

void
Landscape::setMap_2(char *map)
{
   
}

void
Landscape::setMap_3(char *map)
{
   
}

/* Some initial bugs for moving around (shifting the maps) */
void
Landscape::shiftMap_1(int side)
{
   int x, y;
   
   switch (side)
	 {
	  case DIR_NORTH:
		map1_y += MAP1_GRID_WIDTH;
		map1_shift_y++;
		if (map1_shift_y == 5)
		  {
			 map1_shift_y = 0;
			 shiftMap_2(side);
		  }
			 
		for (y = 0; y < MAP1_WIDTH-1; y++)
		  for (x = 0; x < MAP1_WIDTH; x++)
			{
//			   MAP1(x, y) = MAP1(x, y+1);
			}
//		for (x = 0; x < MAP1_WIDTH; x++)
//		  MAP1(x, 0) = slice[x];
		break;
		
	  case DIR_EAST:
		map1_x += MAP1_GRID_WIDTH;
		map1_shift_x++;
		if (map1_shift_x == 5)
		  {
			 map1_shift_x = 0;
			 shiftMap_2(side);
		  }
		
		for (x = 0; x < MAP1_WIDTH-1; x++)
		  for (y = 0; y < MAP1_WIDTH; y++)
			{
//			   MAP1(x, y) = MAP1(x+1, y);
			}
//		for (y = 0; y < MAP1_WIDTH; y++)
//		  MAP1(MAP1_WIDTH-1, y) = slice[y];
		break;
		
	  case DIR_SOUTH:
		map1_y -= MAP1_GRID_WIDTH;
		map1_shift_y--;
		if (map1_shift_y == -5)
		  {
			 map1_shift_y = 0;
			 shiftMap_2(side);
		  }
			 
        for (y = MAP1_WIDTH-1; y > 0; y--)
		  for (x = 0; x < MAP1_WIDTH; x++)
		  {
//			 MAP1(x, y) = MAP1(x, y+1);
		  }
//		for (x = 0; x < MAP1_WIDTH; x++)
//		  MAP1(x, MAP1_WIDTH-1) = slice[x];
		break;
		
	  case DIR_WEST:
		map1_x -= MAP1_GRID_WIDTH;
		map1_shift_x--;
		if (map1_shift_x == -5)
		  {
			 map1_shift_x = 0;
			 shiftMap_2(side);
		  }
			 
		for (x = MAP1_WIDTH-1; x > 0; x--)
		  for (y = 0; y < MAP1_WIDTH; y++)
			{
//			   MAP1(x, y) = MAP1(x-1, y);
			}
//		for (y = 0; y < MAP1_WIDTH; y++)
//		  MAP1(0, y) = slice[y];
		break;

	  default:
		error->put(ERROR_FATAL, "invalid first argument to updateMap_1Slice()");
		break;
	 }

   makeHeightMaps();
   initMap_1Mesh();
   makeMap_1();
   makeMap_2();
}

void
Landscape::shiftMap_2(int side)
{
   int x, y;
   
   switch (side)
	 {
	  case DIR_NORTH:
		map2_y += MAP2_GRID_WIDTH;
		map2_shift_y++;
		if (map2_shift_y == 5)
		  {
			 map2_shift_y = 0;
			 shiftMap_3(side);
		  }
			 
		for (y = 0; y < MAP2_WIDTH-1; y++)
		  for (x = 0; x < MAP2_WIDTH; x++)
			{
//			   MAP2(x, y) = MAP2(x, y+1);
			}
//		for (x = 0; x < MAP2_WIDTH; x++)
//		  MAP2(x, 0) = slice[x];
		break;
		
	  case DIR_EAST:
		map2_x += MAP2_GRID_WIDTH;
		map2_shift_x++;
		if (map2_shift_x == 5)
		  {
			 map2_shift_x = 0;
			 shiftMap_3(side);
		  }
		
		for (x = 0; x < MAP2_WIDTH-1; x++)
		  for (y = 0; y < MAP2_WIDTH; y++)
			{
//			   MAP2(x, y) = MAP2(x+1, y);
			}
//		for (y = 0; y < MAP2_WIDTH; y++)
//		  MAP2(MAP2_WIDTH-1, y) = slice[y];
		break;
		
	  case DIR_SOUTH:
		map2_y -= MAP2_GRID_WIDTH;
		map2_shift_y--;
		if (map2_shift_y == -5)
		  {
			 map2_shift_y = 0;
			 shiftMap_3(side);
		  }
			 
        for (y = MAP2_WIDTH-1; y > 0; y--)
		  for (x = 0; x < MAP2_WIDTH; x++)
		  {
//			 MAP2(x, y) = MAP2(x, y+1);
		  }
//		for (x = 0; x < MAP2_WIDTH; x++)
//		  MAP2(x, MAP2_WIDTH-1) = slice[x];
		break;
		
	  case DIR_WEST:
		map2_x -= MAP2_GRID_WIDTH;
		map2_shift_x--;
		if (map2_shift_x == -5)
		  {
			 map2_shift_x = 0;
			 shiftMap_3(side);
		  }
			 
		for (x = MAP2_WIDTH-1; x > 0; x--)
		  for (y = 0; y < MAP2_WIDTH; y++)
			{
//			   MAP2(x, y) = MAP2(x-1, y);
			}
//		for (y = 0; y < MAP2_WIDTH; y++)
//		  MAP2y(0, y) = slice[y];
		break;

	  default:
		error->put(ERROR_FATAL, "invalid first argument to updateMap_1Slice()");
		break;
	 }

   makeHeightMaps();
   initMap_2Mesh();
   makeMap_2();
   makeMap_3();
   
}

void
Landscape::shiftMap_3(int side)
{
   switch (side)
	 {
	  case DIR_NORTH:
		map3_y += MAP3_GRID_WIDTH;
		break;
	  case DIR_SOUTH:
		map3_y -= MAP3_GRID_WIDTH;
		break;
	  case DIR_EAST:
		map3_x += MAP3_GRID_WIDTH;
		break;
	  case DIR_WEST:
		map3_x -= MAP3_GRID_WIDTH;
		break;
	 }

   makeHeightMaps();
   initMap_3Mesh();
   makeMap_3();
}

void Landscape::addObject(int oid)
{
}

void Landscape::removeObject(int oid)
{
}
