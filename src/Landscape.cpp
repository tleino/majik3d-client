#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3

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
   float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   float light_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   float light_specular[] = { 0.3, 0.3, 0.3, 1.0 };
   float lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   
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

   makeHeightMaps();
   
   initMap_1Mesh();
   makeMap_1();

   initMap_2Mesh();
   makeMap_2();
   
   initMap_3Mesh();
   makeMap_3();
   
   angle = 0;
   
   glEnable(GL_FOG);
   
	 {
		GLfloat fogColor[4] =
		  {0.5, 0.5, 0.5, 1.0};
		
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.005);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glClearColor(0.0, 0.0, 0.0, 1.0);
	 }
   
   glFogf(GL_FOG_START, 200.0);
   glFogf(GL_FOG_END, 2500.0);

}


void Landscape::makeHeightMaps()
{
   zmap_1 = new float[(MAP1_WIDTH+1)*(MAP1_WIDTH+1)];
   zmap_2 = new float[(MAP2_WIDTH+1)*(MAP2_WIDTH+1)];
   zmap_3 = new float[(MAP3_WIDTH+1)*(MAP3_WIDTH+1)];

   map1_x = map1_y = MAP3_WIDTH*(MAP3_GRID_WIDTH/2) - MAP1_WIDTH*(MAP1_GRID_WIDTH/2);
   map2_x = map2_y = MAP3_WIDTH*(MAP3_GRID_WIDTH/2) - MAP2_WIDTH*(MAP2_GRID_WIDTH/2);
   map3_x = map3_y = 0;
   
   
   int i, j;
   float *temp;
   Perlin *perl = new Perlin;
   
   temp = zmap_3;
   
   for (j = 0; j <= MAP3_WIDTH; j++) 
	 {
		for (i = 0; i <= MAP3_WIDTH; i++)
		  {
			 *(temp++) = perl->perlinNoise_2D((float)(map3_x+i*MAP3_GRID_WIDTH)/300, 
											  (float)(map3_y+j*MAP3_GRID_WIDTH)/300)*200;
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
			 *(temp++) = perl->perlinNoise_2D((float)(map2_x+i*MAP2_GRID_WIDTH)/300, 
											  (float)(map2_y+j*MAP2_GRID_WIDTH)/300)*200;
		  } 
		
		*(temp++) = getHeight( map2_x + MAP2_GRID_WIDTH*MAP2_WIDTH, map2_y + j*MAP2_GRID_WIDTH);
	 }
   for (i = 0; i <= MAP2_WIDTH; i++)
	 {
		*(temp++) = getHeight( map2_x + i*MAP2_GRID_WIDTH, map2_y + MAP2_GRID_WIDTH*MAP2_WIDTH);
	 }
   
   float temp2;
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
			 *(temp++) = perl->perlinNoise_2D((float)(map1_x+i*MAP1_GRID_WIDTH)/300,
											  (float)(map1_y+j*MAP1_GRID_WIDTH)/300)*200;
		  }
		
		*(temp++) = getHeight( map1_x + MAP1_GRID_WIDTH*MAP1_WIDTH, map1_y + j*MAP1_GRID_WIDTH);
	 }
   for (i = 0; i <= MAP1_WIDTH; i++)
	 {
		*(temp++) = getHeight( map1_x + i*MAP1_GRID_WIDTH, map1_y + MAP1_GRID_WIDTH*MAP1_WIDTH);
	 }
   
   delete(perl);
}


void Landscape::drawLandscape()
{
   float light_position[] = { 0.0, 0.0, 1.0, 0.0 };
   
   glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -viewport_ratio, viewport_ratio, 5.0, 100000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   glPushMatrix();
   
   /* Load camera matrix here            */
   gluLookAt(0.0, -500.0, -200, 
			 0.0, 100.0, getHeight(map1_x + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2, map1_y + (MAP1_WIDTH*MAP1_GRID_WIDTH)/2), 0.0, 0.0, -1.0);   
//   glTranslatef(0.0, -300.0, -200.0);
//   glRotatef(135.0, 1.0, 0.0, 0.0);
   glRotatef(angle/2, 0.0, 0.0, 1.0);
   
   /* Position sun                       */
   glPushMatrix();
   glRotatef(1.23*angle, 0.0, 1.0, 0.0);
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
   
   glPopMatrix();
   
   // angle += 2;
}

void Landscape::setViewport(int x, int y, int w, int h)
{
   viewport_x = x;
   viewport_y = y;
   viewport_w = w;
   viewport_h = h;
   viewport_ratio = (float)h/(float)w;

}

float Landscape::interpolate(float a, float b, float x)
{
   return (1-x)*a + x*b;
}


float Landscape::getHeight(int x, int y)
{
   int tempx, tempy;
   
//   debug->put("%d %d", x, y);
   tempx = x - map1_x;
   tempy = y - map1_y;
	 
   if (tempx > 0 && tempx < MAP1_WIDTH*MAP1_GRID_WIDTH && 
	   tempy > 0 && tempy < MAP1_WIDTH*MAP1_GRID_WIDTH)
	 {           
		return zmap_1[tempy*(MAP1_WIDTH+1) + tempx];
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
			 float x1, x2;
			 
			 if (fx == 0)
			   x1 = zmap_2[ay*(MAP2_WIDTH+1) + ax];
			 else
			   {
				  int bx = ax + 1;
				  x1 = interpolate(zmap_2[ay*(MAP2_WIDTH+1) + ax], 
								   zmap_2[ay*(MAP2_WIDTH+1) + bx], 
								   (float)fx / MAP2_GRID_WIDTH);
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
										(float)fx / MAP2_GRID_WIDTH);
					}
				  return interpolate(x1, x2, (float)fy / MAP2_GRID_WIDTH);
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
				  float x1, x2;
				  
//				  debug->put("%d %d %d %d %d %d", x, y, ax, fx, ay, fy);
				  if (fx == 0)
					x1 = zmap_3[ay*(MAP3_WIDTH+1) + ax];
				  else
					{
					   int bx = ax + 1;
					   x1 = interpolate(zmap_3[ay*(MAP3_WIDTH+1) + ax],
										zmap_3[ay*(MAP3_WIDTH+1) + bx],
										(float)fx / MAP3_GRID_WIDTH);
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
											 (float)fx / MAP3_GRID_WIDTH);
						 }
					   return interpolate(x1, x2, (float)fy / MAP3_GRID_WIDTH);
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
   

void Landscape::initMap_1Mesh()
{
   float *temp;
   
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
   
   
   int k = 0;
   
   temp = map_1Mesh.vertices;
   for (int j = 0; j < MAP1_WIDTH; j++) {
	  for (int i = 0; i < MAP1_WIDTH; i++)
		{
		   map_1Mesh.face_normals[k++].set(-MAP1_GRID_WIDTH*(*(temp+5) - *(temp+2)),
										   -MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+2)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);

//		   debug->put("%f %f %f", map_1Mesh.face_normals[k-1].x, map_1Mesh.face_normals[k-1].y, map_1Mesh.face_normals[k-1].z);
		   map_1Mesh.face_normals[k++].set(MAP1_GRID_WIDTH*(*(temp+(MAP1_WIDTH+1)*3+2) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*(*(temp+5) - *(temp+(MAP1_WIDTH+1)*3+5)),
										   MAP1_GRID_WIDTH*MAP1_GRID_WIDTH);
		   temp += 3;
//		   debug->put("%f %f %f", map_1Mesh.face_normals[k-1].x, map_1Mesh.face_normals[k-1].y, map_1Mesh.face_normals[k-1].z);

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
		*(temp++) = map_1Mesh.face_normals[i*2-2].x/2 + 
		  map_1Mesh.face_normals[i*2-1].x + 
		  map_1Mesh.face_normals[i*2].x;
		*(temp++) = map_1Mesh.face_normals[i*2-2].y/2 + 
		  map_1Mesh.face_normals[i*2-1].y + 
		  map_1Mesh.face_normals[i*2].y;
		*(temp++) = map_1Mesh.face_normals[i*2-2].z/2 + 
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
   int tempx, tempy;
   
   temp = map_2Mesh.vertices;
   for (int j= - MAP2_WIDTH/2, tempy = 0; j<=MAP2_WIDTH/2; j++, tempy++) {
	  for (int i= - MAP2_WIDTH/2, tempx = 0; i<=MAP2_WIDTH/2; i++, tempx++)
		{
		   *(temp++) = i*MAP2_GRID_WIDTH;
		   *(temp++) = j*MAP2_GRID_WIDTH;
//		   *(temp++) = perl->perlinNoise_2D((float)i/4, (float)j/4)*500;
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
		*(temp++) = map_2Mesh.face_normals[i*2-2].x/2 + 
		  map_2Mesh.face_normals[i*2-1].x + 
		  map_2Mesh.face_normals[i*2].x;
		*(temp++) = map_2Mesh.face_normals[i*2-2].y/2 + 
		  map_2Mesh.face_normals[i*2-1].y + 
		  map_2Mesh.face_normals[i*2].y;
		*(temp++) = map_2Mesh.face_normals[i*2-2].z/2 + 
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
		*(temp++) = map_3Mesh.face_normals[i*2-2].x/2 + 
		  map_3Mesh.face_normals[i*2-1].x + 
		  map_3Mesh.face_normals[i*2].x;
		*(temp++) = map_3Mesh.face_normals[i*2-2].y/2 + 
		  map_3Mesh.face_normals[i*2-1].y + 
		  map_3Mesh.face_normals[i*2].y;
		*(temp++) = map_3Mesh.face_normals[i*2-2].z/2 + 
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
   
   float length;
   
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
             glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP2_WIDTH/2 - MAP2_GAP/2 + MAP2_GAP);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP2_WIDTH/2 - MAP2_GAP/2)+1;i++)
		  {
             glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP2_WIDTH+1) + i);
             glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP2_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP2_GAP-1;i<MAP2_WIDTH+1;i++)
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
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
	 }
   
   for (;j<(MAP3_WIDTH/2 - MAP3_GAP/2 + MAP3_GAP);j++)
	 {
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i=0;i<(MAP3_WIDTH/2 - MAP3_GAP/2)+1;i++)
		  {
			 glTexCoord2f(i, 0.0);
			 glArrayElement( (j)*(MAP3_WIDTH+1) + i);
			 glTexCoord2f(i, 1.0);
			 glArrayElement( (j+1)*(MAP3_WIDTH+1) + i);
		  }
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		
		for (i += MAP3_GAP-1;i<MAP3_WIDTH+1;i++)
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

void Landscape::addObject(int oid)
{
}

void Landscape::removeObject(int oid)
{
}
