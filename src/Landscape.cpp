#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)

#define MAP1_WIDTH       20
#define MAP1_GRID_WIDTH  10

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
   float light_position[] = { 0.0, 0.0, -1.0, 0.0 };
   
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);

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

   listId_1 = -1;
   listId_2 = -1;
   listId_3 = -1;
   listId_4 = -1;
   
   initMap_1Mesh();
   makeMap_1();

   angle = 0;
}

void Landscape::drawLandscape()
{
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -viewport_ratio, viewport_ratio, 5.0, 1000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   
   /* Load camera matrix here            */
   glTranslatef(0.0, 0.0, -500.0);
   glRotatef(angle, 0.0, 1.0, 0.0);
   glRotatef(110.0, 1.0, 0.0, 0.0);

   /* Position sun                       */
   
   /* Draw sky                           */
   /* - Check for detail settings        */

   
   /* Draw terrain                       */
   /*  - Check for detail settings       */
   /*  - Cull map regions                */
   glCallList(listId_1);

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
		   *(temp++) = perl->perlinNoise_2D((float)i/4, (float)j/4)*20;
		   
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

void Landscape::makeMap_1()
{
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
}

void Landscape::makeMap_3()
{
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
