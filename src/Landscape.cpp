#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)

#define MAP1_WIDTH       20
#define MAP1_GRID_WIDTH  10

#define MAP2_WIDTH       20
#define MAP2_GAP          6
#define MAP2_RATIO        5
#define MAP2_GRID_WIDTH  MAP2_RATIO*MAP1_GRID_WIDTH  

#define MAP3_WIDTH       20
#define MAP3_GAP          6
#define MAP3_RATIO        5
#define MAP3_GRID_WIDTH  MAP3_RATIO*MAP2_GRID_WIDTH  


Landscape::Landscape()
{
   first = NULL;
   
   #ifdef DEBUG
	 debug->put("Landscape constructor");
   #endif
}

Landscape::~Landscape()
{
   if(first != NULL)
	 {
	    delete first;
		first = NULL;
	 }
   #ifdef DEBUG
	 debug->put("Landscape destructor");
   #endif
}

void Landscape::init()
{
   float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   float light_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   float light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
   
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);

   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   
   glShadeModel(GL_SMOOTH);
   
   setViewport(0, 0, display->width, display->height);
   
   map_1Mesh.numVertices = (MAP1_WIDTH+1)*(MAP1_WIDTH+1);
   map_1Mesh.vertices  =  new float[3*map_1Mesh.numVertices];
   map_1Mesh.face_normals = new P3D[(MAP1_WIDTH)*(MAP1_WIDTH)*2];
   map_1Mesh.normals = new float[3*map_1Mesh.numVertices];

   map_2Mesh.numVertices = (MAP2_WIDTH+1)*(MAP2_WIDTH+1);
   map_2Mesh.vertices  =  new float[3*map_2Mesh.numVertices];
   map_2Mesh.face_normals = new P3D[(MAP2_WIDTH)*(MAP2_WIDTH)*2];
   map_2Mesh.normals = new float[3*map_2Mesh.numVertices];
   
   map_3Mesh.numVertices = (MAP3_WIDTH+1)*(MAP3_WIDTH+1);
   map_3Mesh.vertices  =  new float[3*map_2Mesh.numVertices];
   map_3Mesh.face_normals = new P3D[(MAP3_WIDTH)*(MAP3_WIDTH)*2];
   map_3Mesh.normals = new float[3*map_3Mesh.numVertices];
   
   
   listId_1 = -1;
   listId_2 = -1;
   listId_3 = -1;
   listId_4 = -1;
   
   initMap_1Mesh();
   makeMap_1();

   initMap_2Mesh();
   makeMap_2();
   
   initMap_3Mesh();
   makeMap_3();
   
   angle = 0;
}

void Landscape::drawLandscape()
{
   float light_position[] = { 0.0, 0.7071, 0.7071, 0.0 };
   
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -viewport_ratio, viewport_ratio, 5.0, 100000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   
   /* Load camera matrix here            */
   glTranslatef(0.0, 0.0, -15000.0);
   glRotatef(110.0, 1.0, 0.0, 0.0);
   glRotatef(angle, 0.0, 0.0, 1.0);
   
   /* Position sun                       */
   glPushMatrix();
   glRotatef(angle, 0.0, 1.0, 0.0);
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);
   glPopMatrix();
   
   /* Draw sky                           */
   /* - Check for detail settings        */

   
   /* Draw terrain                       */
   /*  - Check for detail settings       */
   /*  - Cull map regions                */

   glCallList(listId_3);

   glPushMatrix();
//   glTranslatef(0.0, 0.0, -250);
   glCallList(listId_2);
   glPopMatrix();

   glPushMatrix();
//   glTranslatef(0.0, 0.0, -500);
   glCallList(listId_1);
   glPopMatrix();
   
   /* Draw objects                       */
   /*  - Check for 1st-person/3rd-person */
   /*  - Check for detail settings       */
   
   
   angle += 2;
}

void Landscape::setViewport(int x, int y, int w, int h)
{
   viewport_x = x;
   viewport_y = y;
   viewport_w = w;
   viewport_h = h;
   viewport_ratio = (float)h/(float)w;

}

void Landscape::initMap_1Mesh()
{
   float *temp;
   Perlin *perl = new Perlin;
   
   temp = map_1Mesh.vertices;
   for (int j= - MAP1_WIDTH/2; j<=MAP1_WIDTH/2; j++) {
	  for (int i= - MAP1_WIDTH/2; i<=MAP1_WIDTH/2; i++) 
		{
		   *(temp++) = i*MAP1_GRID_WIDTH;
		   *(temp++) = j*MAP1_GRID_WIDTH;
		   *(temp++) = perl->perlinNoise_2D((float)i/100, (float)j/100)*200;
		   
		}
   }

   delete(perl);   
   
   int k = 0;

   temp = map_1Mesh.vertices;
   for (int j = 0; j < MAP1_WIDTH; j++) {
	  for (int i = 0; i < MAP1_WIDTH; i++)
		{
		   map_1Mesh.face_normals[k++].set(MAP1_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+2)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);
//		   Debug("%f %f %f", map_1Mesh.face_normals[k-1].x, map_1Mesh.face_normals[k-1].y, map_1Mesh.face_normals[k-1].z);
		   map_1Mesh.face_normals[k++].set(MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*(*(temp+5) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);
		   temp += 3;
//		   Debug("%f %f %f", map_1Mesh.face_normals[k-1].x, map_1Mesh.face_normals[k-1].y, map_1Mesh.face_normals[k-1].z);
		}
   }

   temp = map_1Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_1Mesh.face_normals[0].x;
   *(temp++) = map_1Mesh.face_normals[0].y;
   *(temp++) = map_1Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP1_WIDTH; i++)
	 {
		*(temp++) = map_1Mesh.face_normals[i-1].x + map_1Mesh.face_normals[i].x + map_1Mesh.face_normals[i+1].x;
		*(temp++) = map_1Mesh.face_normals[i-1].y + map_1Mesh.face_normals[i].y + map_1Mesh.face_normals[i+1].y;
		*(temp++) = map_1Mesh.face_normals[i-1].z + map_1Mesh.face_normals[i].z + map_1Mesh.face_normals[i+1].z;
		//		Debug("%f %f %f", map_1Mesh.normals[k-1].x, map_1Mesh.normals[k-1].y, map_1Mesh.normals[k-1].z);
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
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*(i)].x + 
			   map_1Mesh.face_normals[(j-1)*(i+1)].x +
			   map_1Mesh.face_normals[(j-1)*(i+2)].x +
			   map_1Mesh.face_normals[(j)*(i-1)].x +
			   map_1Mesh.face_normals[(j)*(i)].x +
			   map_1Mesh.face_normals[(j)*(i+1)].x;
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*(i)].y +
			   map_1Mesh.face_normals[(j-1)*(i+1)].y +
			   map_1Mesh.face_normals[(j-1)*(i+2)].y +
			   map_1Mesh.face_normals[(j)*(i-1)].y +
			   map_1Mesh.face_normals[(j)*(i)].y +
			   map_1Mesh.face_normals[(j)*(i+1)].y;
			 
			 *(temp++) = map_1Mesh.face_normals[(j-1)*(i)].z +
			   map_1Mesh.face_normals[(j-1)*(i+1)].z +
			   map_1Mesh.face_normals[(j-1)*(i+2)].z +
			   map_1Mesh.face_normals[(j)*(i-1)].z +
			   map_1Mesh.face_normals[(j)*(i)].z +
			   map_1Mesh.face_normals[(j)*(i+1)].z;
			 
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
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i)].x +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+1)].x +
		  map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+2)].x;
		
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i)].y +
		            map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+1)].y +
		            map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+2)].y;
		
		*(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i)].z +
		            map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+1)].z +
		            map_1Mesh.face_normals[MAP1_WIDTH*2*(MAP1_WIDTH-1)+(i+2)].z;
		
	 }

   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].x;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].y;
   *(temp++) = map_1Mesh.face_normals[MAP1_WIDTH*(MAP1_WIDTH*2)-1].z;

   float length;
   
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
   float *temp;
   Perlin *perl = new Perlin;
   
   temp = map_2Mesh.vertices;
   for (int j= - MAP2_WIDTH/2; j<=MAP2_WIDTH/2; j++) {
	  for (int i= - MAP2_WIDTH/2; i<=MAP2_WIDTH/2; i++)
		{
		   *(temp++) = i*MAP2_GRID_WIDTH;
		   *(temp++) = j*MAP2_GRID_WIDTH;
		   *(temp++) = perl->perlinNoise_2D((float)i/20, (float)j/20)*300;
		   
		}
   }
   
   delete(perl);
   
   int k = 0;
   
   temp = map_2Mesh.vertices;
   for (int j = 0; j < MAP2_WIDTH; j++) {
	  for (int i = 0; i < MAP2_WIDTH; i++)
		{
		   map_2Mesh.face_normals[k++].set(MAP2_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   MAP2_GRID_WIDTH*(*(temp+(MAP2_WIDTH+1)*3+2) - *(temp+2)),
										   MAP2_GRID_WIDTH*MAP2_GRID_WIDTH);
		   //         Debug("%f %f %f", map_2Mesh.face_normals[k-1].x, map_2Mesh.face_normals[k-1].y, map_2Mesh.face_normals[k-1].z);
		   map_2Mesh.face_normals[k++].set(MAP2_GRID_WIDTH*(*(temp+(MAP2_WIDTH+1)*3+2) - *(temp+(MAP2_WIDTH+1)*3+5)),
										   MAP2_GRID_WIDTH*(*(temp+5) - *(temp+(MAP2_WIDTH+1)*3+5)),
										   MAP2_GRID_WIDTH*MAP2_GRID_WIDTH);
		   temp += 3;
		   //         Debug("%f %f %f", map_2Mesh.face_normals[k-1].x, map_2Mesh.face_normals[k-1].y, map_2Mesh.face_normals[k-1].z);
		}
   }
   
   temp = map_2Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_2Mesh.face_normals[0].x;
   *(temp++) = map_2Mesh.face_normals[0].y;
   *(temp++) = map_2Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP2_WIDTH; i++)
	 {
		*(temp++) = map_2Mesh.face_normals[i-1].x + map_2Mesh.face_normals[i].x + map_2Mesh.face_normals[i+1].x;
		*(temp++) = map_2Mesh.face_normals[i-1].y + map_2Mesh.face_normals[i].y + map_2Mesh.face_normals[i+1].y;
		*(temp++) = map_2Mesh.face_normals[i-1].z + map_2Mesh.face_normals[i].z + map_2Mesh.face_normals[i+1].z;
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
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*(i)].x +
			   map_2Mesh.face_normals[(j-1)*(i+1)].x +
			   map_2Mesh.face_normals[(j-1)*(i+2)].x +
			   map_2Mesh.face_normals[(j)*(i-1)].x +
			   map_2Mesh.face_normals[(j)*(i)].x +
			   map_2Mesh.face_normals[(j)*(i+1)].x;
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*(i)].y +
			   map_2Mesh.face_normals[(j-1)*(i+1)].y +
			   map_2Mesh.face_normals[(j-1)*(i+2)].y +
			   map_2Mesh.face_normals[(j)*(i-1)].y +
			   map_2Mesh.face_normals[(j)*(i)].y +
			   map_2Mesh.face_normals[(j)*(i+1)].y;
			 
			 *(temp++) = map_2Mesh.face_normals[(j-1)*(i)].z +
			   map_2Mesh.face_normals[(j-1)*(i+1)].z +
			   map_2Mesh.face_normals[(j-1)*(i+2)].z +
			   map_2Mesh.face_normals[(j)*(i-1)].z +
			   map_2Mesh.face_normals[(j)*(i)].z +
			   map_2Mesh.face_normals[(j)*(i+1)].z;
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
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i)].x +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+1)].x +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+2)].x;
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i)].y +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+1)].y +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+2)].y;
		
		*(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i)].z +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+1)].z +
		  map_2Mesh.face_normals[MAP2_WIDTH*2*(MAP2_WIDTH-1)+(i+2)].z;
		
	 }
   
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].x;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].y;
   *(temp++) = map_2Mesh.face_normals[MAP2_WIDTH*(MAP2_WIDTH*2)-1].z;
   
   float length;
   
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
   float *temp;
   Perlin *perl = new Perlin;
   
   temp = map_3Mesh.vertices;
   for (int j= - MAP3_WIDTH/2; j<=MAP3_WIDTH/2; j++) {
	  for (int i= - MAP3_WIDTH/2; i<=MAP3_WIDTH/2; i++)
		{
		   *(temp++) = i*MAP3_GRID_WIDTH;
		   *(temp++) = j*MAP3_GRID_WIDTH;
		   *(temp++) = perl->perlinNoise_2D((float)i/4, (float)j/4)*400;
		   
		}
   }
   
   delete(perl);
   
   int k = 0;
   
   temp = map_3Mesh.vertices;
   for (int j = 0; j < MAP3_WIDTH; j++) {
	  for (int i = 0; i < MAP3_WIDTH; i++)
		{
		   map_3Mesh.face_normals[k++].set(MAP3_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   MAP3_GRID_WIDTH*(*(temp+(MAP3_WIDTH+1)*3+2) - *(temp+2)),
										   MAP3_GRID_WIDTH*MAP3_GRID_WIDTH);
		   //         Debug("%f %f %f", map_3Mesh.face_normals[k-1].x, map_3Mesh.face_normals[k-1].y, map_3Mesh.face_normals[k-1].z);
		   map_3Mesh.face_normals[k++].set(MAP3_GRID_WIDTH*(*(temp+(MAP3_WIDTH+1)*3+2) - *(temp+(MAP3_WIDTH+1)*3+5)),
										   MAP3_GRID_WIDTH*(*(temp+5) - *(temp+(MAP3_WIDTH+1)*3+5)),
										   MAP3_GRID_WIDTH*MAP3_GRID_WIDTH);
		   temp += 3;
		   //         Debug("%f %f %f", map_3Mesh.face_normals[k-1].x, map_3Mesh.face_normals[k-1].y, map_3Mesh.face_normals[k-1].z);
		}
   }
   
   temp = map_3Mesh.normals;
   
   k = 0;
   
   *(temp++) = map_3Mesh.face_normals[0].x;
   *(temp++) = map_3Mesh.face_normals[0].y;
   *(temp++) = map_3Mesh.face_normals[0].z;
   
   for (int i = 1; i < MAP3_WIDTH; i++)
	 {
		*(temp++) = map_3Mesh.face_normals[i-1].x + map_3Mesh.face_normals[i].x + map_3Mesh.face_normals[i+1].x;
		*(temp++) = map_3Mesh.face_normals[i-1].y + map_3Mesh.face_normals[i].y + map_3Mesh.face_normals[i+1].y;
		*(temp++) = map_3Mesh.face_normals[i-1].z + map_3Mesh.face_normals[i].z + map_3Mesh.face_normals[i+1].z;
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
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*(i)].x +
			   map_3Mesh.face_normals[(j-1)*(i+1)].x +
			   map_3Mesh.face_normals[(j-1)*(i+2)].x +
			   map_3Mesh.face_normals[(j)*(i-1)].x +
			   map_3Mesh.face_normals[(j)*(i)].x +
			   map_3Mesh.face_normals[(j)*(i+1)].x;
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*(i)].y +
			   map_3Mesh.face_normals[(j-1)*(i+1)].y +
			   map_3Mesh.face_normals[(j-1)*(i+2)].y +
			   map_3Mesh.face_normals[(j)*(i-1)].y +
			   map_3Mesh.face_normals[(j)*(i)].y +
			   map_3Mesh.face_normals[(j)*(i+1)].y;
			 
			 *(temp++) = map_3Mesh.face_normals[(j-1)*(i)].z +
			   map_3Mesh.face_normals[(j-1)*(i+1)].z +
			   map_3Mesh.face_normals[(j-1)*(i+2)].z +
			   map_3Mesh.face_normals[(j)*(i-1)].z +
			   map_3Mesh.face_normals[(j)*(i)].z +
			   map_3Mesh.face_normals[(j)*(i+1)].z;
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
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i)].x +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+1)].x +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+2)].x;
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i)].y +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+1)].y +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+2)].y;
		
		*(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i)].z +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+1)].z +
		  map_3Mesh.face_normals[MAP3_WIDTH*2*(MAP3_WIDTH-1)+(i+2)].z;
		
	 }
   
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].x;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].y;
   *(temp++) = map_3Mesh.face_normals[MAP3_WIDTH*(MAP3_WIDTH*2)-1].z;
   
   float length;
   
   for (int i=0; i<map_3Mesh.numVertices*3; )
	 {
		length = sqrt(map_3Mesh.normals[i]*map_3Mesh.normals[i] +
					  map_3Mesh.normals[i+1]*map_3Mesh.normals[i+1] +
					  map_3Mesh.normals[i+2]*map_3Mesh.normals[i+2]);
		map_3Mesh.normals[i++] /= length;
		map_3Mesh.normals[i++] /= length;
		map_3Mesh.normals[i++] /= length;
		
		//      Debug("%f %f %f", map_3Mesh.normals[i-3], map_3Mesh.normals[i-2], map_3Mesh.normals[i-1]);
		
	 }
   
}


void Landscape::makeMap_1()
{
   float color[4] = { 1.0, 0.0, 0.0, 1.0 };
 
   /* Check for already existing display list */
   if (listId_1 != -1)
	 glDeleteLists(listId_1, 1);

   glVertexPointer(3, GL_FLOAT, 0, map_1Mesh.vertices);
   glNormalPointer(GL_FLOAT, 0, map_1Mesh.normals);

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
			 glArrayElement( (j)*(MAP1_WIDTH+1) + i);
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
   
   glVertexPointer(3, GL_FLOAT, 0, map_2Mesh.vertices);
   glNormalPointer(GL_FLOAT, 0, map_2Mesh.normals);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   
   /* Generate new list ID */
   listId_2 = glGenLists(1);
   glNewList(listId_2, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

   int j, i;
   
   for (j=0;j<(MAP2_WIDTH/2 - MAP2_GAP/2);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP2_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP2_WIDTH/2 - MAP2_GAP/2 + MAP2_GAP);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP2_WIDTH/2 - MAP2_GAP/2)+1;i++)
		  {
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP2_GAP-1;i<MAP2_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
		
	 }

   for (;j<MAP2_WIDTH;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP2_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
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
   float color[4] = { 0.0, 0.0, 1.0, 1.0 };
   
   /* Check for already existing display list */
   if (listId_3 != -1)
	 glDeleteLists(listId_3, 1);
   
   glVertexPointer(3, GL_FLOAT, 0, map_3Mesh.vertices);
   glNormalPointer(GL_FLOAT, 0, map_3Mesh.normals);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glEnableClientState( GL_NORMAL_ARRAY );
   
   /* Generate new list ID */
   listId_3 = glGenLists(1);
   glNewList(listId_3, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

   int i, j;

   for (j=0;j<(MAP3_WIDTH/2 - MAP3_GAP/2);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP3_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP3_WIDTH/2 - MAP3_GAP/2 + MAP3_GAP);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP3_WIDTH/2 - MAP3_GAP/2)+1;i++)
		  {
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP3_GAP-1;i<MAP3_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
		
	 }
   
   for (;j<MAP3_WIDTH;j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<MAP3_WIDTH+1;i++)
		  {
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
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

void Landscape::addObject(int oid)
{
}

void Landscape::removeObject(int oid)
{
}
