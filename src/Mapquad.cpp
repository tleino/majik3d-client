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

#include "Mapquad.hpp"
#include "Landscape.hpp"
#include "Socket.hpp"
#include "Protocol.hpp"
#include "Debug.hpp"

#include <iostream.h>

#define  NUM_LEVELS         13  
#define  LOD_LEVELS          5
#define  LEVEL_0_SIZE    65536

//int Mapquad::stats[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const
int quad_sizes[NUM_LEVELS] =
{
   65536,
   32768,
   16384,
   8196,
   4096,
   2048,
   1024,
   512,
   256,
   128,
   64,
   32,
   16
};

Mapquad::Mapquad(Mapquad *parent, int level, int top_x, int top_y)
{
   this->parent = parent;
   this->child1 = NULL;
   this->child2 = NULL;
   this->child3 = NULL;
   this->child4 = NULL;
   this->level = level;
   this->top_x = top_x;
   this->top_y = top_y;
   
   this->mid_x = top_x + quad_sizes[level]/2*16;
   this->mid_y = top_y + quad_sizes[level]/2*16;
   
   this->reference_count = 0;
   this->map_requested = 0;
   
   terrain_map = NULL;
 
   lod_switch = new ssgSelector() ;
   trans = new ssgTransform() ;

   sgVec3 pos;
   pos[0] = (float)top_x;
   pos[1] = (float)top_y;
   pos[2] = 0.0f;

   trans->setTransform( pos );
   lod_switch->select(0);
//   landscape->terrain->addKid ( trans );
   
   if (level == NUM_LEVELS - 1)
	{
//	   sock->writePacket("%d %d %d %d", CMD_MAP, level, top_x, top_y);
	   lod_indices = new int[LOD_LEVELS];
	   for (int i = 0; i < LOD_LEVELS; i++)
		 lod_indices[i] = -2;
	}

//   stats[level]++;
   
//   cout << "created new map, level: " << level << " top_x: " << top_x << " top_y: " << top_y 
//	 << " mid_x: " << mid_x << " mid_y: " << mid_y << endl;
}

Mapquad::~Mapquad()
{
   cout << "deleting level " << level << " map at ( " << top_x << ", " << top_y << " )" << endl;
   
   if (parent != NULL)
	 parent->decRef();
   
   if (terrain_map != NULL)
	 delete terrain_map;
   
//   stats[level]--;
}

int
Mapquad::incRef()
{
//   cout << "level " << level << " map at ( " << top_x << ", " << top_y << " ): "
//	 << "increasing reference to " << reference_count + 1 << endl;
   return reference_count++;
}

int
Mapquad::decRef()
{
//   cout << "level " << level << " map at ( " << top_x << ", " << top_y << " ): "
//	 << "decreasing reference to " << reference_count - 1 << endl;
   
   if (--reference_count < 1)
	 {
		this->cleanUp();
		return 0;
	 }
   return reference_count;
}

void
Mapquad::cleanUp()
{
   if (level != 0)
	 delete this;
   else
	 cout << "refusing to delete root map" << endl;
}

Mapquad *
Mapquad::getChild1()
{
   if (this->child1 == NULL)
	 {
		this->child1 = new Mapquad (this,
									this->level + 1,
									this->top_x,
									this->top_y);
		this->incRef();
	 }
   return this->child1;
}

Mapquad *
Mapquad::getChild2()
{
   if (this->child2 == NULL)
	 {
		this->child2 = new Mapquad (this,
									this->level + 1,
									this->mid_x,
									this->top_y);
		this->incRef();
	 }
   return this->child2;
}

Mapquad *
Mapquad::getChild3()
{
   if (this->child3 == NULL)
	 {
		this->child3 = new Mapquad (this,
									this->level + 1,
									this->top_x,
									this->mid_y);
		this->incRef();
	 }
   return this->child3;
}

Mapquad *
Mapquad::getChild4()
{
   if (this->child4 == NULL)
	 {
		this->child4 = new Mapquad (this,
									this->level + 1,
									this->mid_x,
									this->mid_y);
		this->incRef();
	 }
   return this->child4;
}

Mapquad *
Mapquad::tryMapquad(int level, int x, int y)
{
   if (this->level == level)
	 return this;
   
   if (x < this->mid_x)
	 {
		if (y < this->mid_y)
		  {
			 if (this->child1 != NULL)
			   return this->child1->tryMapquad (level, x, y);
			 else
			   return NULL;
		  }
		else
		  {
			 if (this->child3 != NULL)
			   return this->child3->tryMapquad (level, x, y);
			 else
			   return NULL;
		  }
	 }
   else
	 {
		if (y < this->mid_y)
		  {
			 if (this->child2 != NULL)
			   return this->child2->tryMapquad (level, x, y);
			 else
			   return NULL;
		  }
		else
		  {
			 if (this->child4 != NULL)
			   return this->child4->tryMapquad (level, x, y);
			 else
			   return NULL;
		  }
	 }
}

Mapquad *
Mapquad::getMapquad(int level, int x, int y)
{
   if (this->level == level)
	 return this;
   
   if (x < this->mid_x)
	 {
		if (y < this->mid_y)
		  return this->getChild1()->getMapquad (level, x, y);
		else
		  return this->getChild3()->getMapquad (level, x, y);
	 }
   else
	 {
		if (y < this->mid_y)
		  return this->getChild2()->getMapquad (level, x, y);
		else
		  return this->getChild4()->getMapquad (level, x, y);     
	 }      
}

char *
Mapquad::getMap()
{
   return terrain_map;
}

void
Mapquad::setMap(char *terrain_map)
{  
   this->terrain_map = terrain_map;
     
//   cout << "setMap for map " << level << "," << top_x << "," << top_y << endl;
   this->setSubMap(NUM_LEVELS - 1 - level, terrain_map);

/*   
   char *submap1, *submap2, *submap3, *submap4;
   
   divideMap( terrain_map, *submap1, *submap2, *submap3, *submap4 );
   
   int lod = NUM_LEVELS - 1 - level;
   
   getChild1() -> setSubMap ( lod, submap1 );
   getChild2() -> setSubMap ( lod, submap2 );
   getChild3() -> setSubMap ( lod, submap3 );
   getChild4() -> setSubMap ( lod, submap4 );
*/		 
}

void
Mapquad::setSubMap(int lod, char *submap)
{	 
//   cout << "setSunMap for map " << level << "," << top_x << "," << top_y << "with lod " << lod << endl;

   if (level == NUM_LEVELS - 1)
	 {
 		lod_indices[lod] = lod_switch->getNumKids();

		lod_switch->addKid ( landscape->createTileLOD (level, top_x, top_y, MAP_SIZE / pow(2, lod ), terrain_map) );

//		selectLOD ( lod );

		if (lod_switch->getNumKids() == 1)
		  {
			 landscape->terrain->addKid ( trans );
			 
			 trans->addKid ( lod_switch );
			 	 
		  }
		
		/* Add some trees */
		
		if (rand() % 3) {
		   scene->addSpecial (top_x, top_y, "tree.ac", true);
		}
		
		if ((300 > (float) top_x-256.0f && 300 < (float) top_x+256.0f) &&
			(300 > (float) top_y-256.0f && 300 < (float) top_y+256.0f)) {
		   scene->addSpecial(300, 300, "snowman.ac", false);
		}
		
		return;
	 }
   
   submap1 = NULL;
   submap2 = NULL;
   submap3 = NULL;
   submap4 = NULL;
   
   divideMap ( lod, submap ); //, submap1, submap2, submap3, submap4 );
   
//   cout << "pok: " << submap1 << endl;
//   lod--;

   getChild1() -> setSubMap ( lod, submap1 );
   getChild2() -> setSubMap ( lod, submap2 );
   getChild3() -> setSubMap ( lod, submap3 );
   getChild4() -> setSubMap ( lod, submap4 );
}

void
Mapquad::selectLOD(int lod)
{  
//   cout << "map " << level << "," << top_x << "," << top_y << ": got selectLOD with lod: " << lod << endl;

   if (lod == -1)
 	 {
		lod_switch->select(0);
		return;
	 }
		
   if ( lod_indices[lod] > -1 ) {
	  lod_switch->selectStep ( lod_indices[lod] );
//	  lod_switch->select(1);
//	  cout << "..set kid: " << lod_indices[lod] << endl;
   }
   else {
	  Mapquad *temp;
	  temp = this;
	  
	  for (int i = 0; i < lod; i++)
		temp = temp->parent;
	  
	  if (temp->map_requested) {
//		 cout << "..waiting for map" << endl;
		 return;
	  }
	  else
		{
//		   cout << "..requesting map for level: " << level - lod << " xy: " << top_x << "," << top_y << endl;
//		   lod_indices[lod] = -1;
		   temp->map_requested = 1;
		   FILE *fp;
		   char buf[256];
		   fp = fopen(debug->string("cache/%01x%05x%05x.map", level-lod, top_x, top_y), "r");
		   if (fp) {
			  fgets (buf, 256, fp);
			  fclose (fp);
			  Mapquad::root_map->getMapquad(level-lod, top_x, top_y)->setMap(buf);
		   } else {
			  sock->writePacket( "%d %d %d %d", CMD_MAP, level - lod, top_x, top_y );
		   }
		}
   }
}

void
Mapquad::divideMap(int lod, char *submap) //, char *submap1, char *submap2, char *submap3, char *submap4)
{
//   cout << "submap: " << submap << endl;
   int submap_size = MAP_SIZE / (2*(lod - (NUM_LEVELS - level) + 2 ));
  // cout << "submap_size" << submap_size << endl;
   submap1 = new char[submap_size*submap_size+1];
   submap2 = new char[submap_size*submap_size+1];
   submap3 = new char[submap_size*submap_size+1];
   submap4 = new char[submap_size*submap_size+1];

   for (int j=0; j < submap_size; j++) {
	  for (int i=0; i < submap_size; i++) {
		 submap1[j * submap_size + i] = submap[j * submap_size*2 + i];
		 submap2[j * submap_size + i] = submap[j * submap_size*2 + i + submap_size];
		 submap3[j * submap_size + i] = submap[(j+submap_size) * submap_size*2 + i];
		 submap4[j * submap_size + i] = submap[(j+submap_size) * submap_size*2 + i + submap_size];
	  }
   }
   
//   cout << "submap1: " << submap1 << endl;
//   cout << "submap2: " << submap2 << endl;
//   cout << "submap3: " << submap3 << endl;
//   cout << "submap4: " << submap4 << endl;
   
   submap1[submap_size*submap_size] = '\0';
   submap2[submap_size*submap_size] = '\0';
   submap3[submap_size*submap_size] = '\0';
   submap4[submap_size*submap_size] = '\0';

}

void
Mapquad::show()
{
//   lod->select(1);
}

void
Mapquad::hide()
{
  // lod->select(0);
}


/*
void
Mapquad::printStats()
{
   for (int i = 0; i < 13; i++)
	 cout << "level " << i << " maps: " << stats[i] << endl;   
}
  */ 
   
