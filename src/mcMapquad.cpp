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

#include "mcMapquad.hpp"
#include "mcLandscape.hpp"
#include "mcSocket.hpp"
#include "mcProtocol.hpp"
#include "mcDebug.hpp"
#include "mcError.hpp"

#include <iostream.h>

#define  NUM_LEVELS         13  
#define  LOD_LEVELS          5
#define  LEVEL_0_SIZE    65536

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
  cout << "new'd: " << this << endl;
  this->parent = parent;
  this->child1 = NULL;
  this->child2 = NULL;
  this->child3 = NULL;
  this->child4 = NULL;
  this->level = level;
  this->top_x = top_x;
  this->top_y = top_y;
  this->mapReceived = false;
  
  this->mid_x = top_x + quad_sizes[level]/2*32;
  this->mid_y = top_y + quad_sizes[level]/2*32;
  
  this->reference_count = 0;
  this->map_requested = 0;
  this->current_lod = -1;

  lod_switch = new ssgSelector();
  trans = new ssgTransform();

	block = NULL;

  sgVec3 pos;
  pos[0] = (float)top_x;
  pos[1] = (float)top_y;
  pos[2] = 0.0f;
  
  trans->setTransform( pos );
//  lod_switch->select(0);
  
  if (level == NUM_LEVELS - 1)
    {
      lod_indices = new int[LOD_LEVELS];
      for (int i = 0; i < LOD_LEVELS; i++)
	lod_indices[i] = -2;
    }



  /* kludgetusta  */

  		state    = new ssgSimpleState ;
			state -> setTexture ( "gfx/bumpnoise.rgb") ;
			state -> enable     ( GL_TEXTURE_2D ) ;
			state -> enable     ( GL_LIGHTING ) ;

			state -> setShadeModel ( GL_SMOOTH );

			state -> enable ( GL_COLOR_MATERIAL ) ;
			state -> disable ( GL_CULL_FACE      ) ;
			state -> setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;
			state -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
			state -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
			state -> setShininess ( 0 ) ;
			state -> setOpaque () ;
			state -> disable ( GL_BLEND ) ;


}

Mapquad::~Mapquad()
{
  if (parent != NULL)
    parent->decRef();

  if (block)
	  delete block;
  
  if (level == NUM_LEVELS - 1 )
    {
      for (int i = 0; i < NUM_VISIBLE_LEVELS; i++)
	{
	  if (map_data[i].terrain != NULL)
	    delete map_data[i].terrain;
		  
	  if (map_data[i].height != NULL)
	    delete map_data[i].height;
	}
    }
}

int
Mapquad::incRef()
{
  return reference_count++;
}

int
Mapquad::decRef()
{
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
    error->put(ERROR_WARNING, "mapquad: Refusing to delete root map.");
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

bool
Mapquad::isMapReceived ()
{
  return mapReceived;
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
  //  return map_data[][][;
  return "haa, olen kartta!\0";
}

void
Mapquad::setMap(Map_data terrain_map)
{  
  mapReceived = true;
  //this->terrain_map = terrain_map;
//  this->setSubMap(NUM_LEVELS - 1 - level, terrain_map);
}

/*
void
Mapquad::setSubMap(int lod, Map_data submap)
{
  assert( lod <= NUM_VISIBLE_LEVELS);

  if (level == NUM_LEVELS - 1)
    {
      map_data[lod] = submap;

      lod_indices[lod] = lod_switch->getNumKids();
      
      lod_switch->addKid ( landscape->createTileLOD (level, top_x, top_y,
						     MAP_SIZE / pow(2, lod ),
						     map_data[lod].terrain ) );
      if (lod_switch->getNumKids() == 1)
	{
	  landscape->terrain->addKid ( trans );
	  
	  trans->addKid ( lod_switch );
	}   
      if (lod == current_lod)
	lod_switch->selectStep ( lod_indices[lod] );
	   
      
      return;
    }

  //  Map_data submap1;
  // Map_data submap2;
  // Map_data submap3;
  // Map_data submap4;
  
  divideMap ( lod, submap, submap1, submap2, submap3, submap4 );
  
  getChild1() -> setSubMap ( lod, submap1 );
  getChild2() -> setSubMap ( lod, submap2 );
  getChild3() -> setSubMap ( lod, submap3 );
  getChild4() -> setSubMap ( lod, submap4 );
}
*/


void
Mapquad::resetBlocks()
{
	 if (level == NUM_LEVELS - 1)
	 {
		 if (block)
			block->reset();
	 }
	 else
	 {
		if (child1)
			child1->resetBlocks();
		if (child2)
			child2->resetBlocks();
		if (child3)
			child3->resetBlocks();
		if (child4)
			child4->resetBlocks();
	 }
}

void
Mapquad::triangulateBlocks()
{
	 if (level == NUM_LEVELS - 1)
	 {
		 if (block)
			block->triangulateBlock();
	 }
	 else
	 {
		if (child1)
			child1->triangulateBlocks();
		if (child2)
			child2->triangulateBlocks();
		if (child3)
			child3->triangulateBlocks();
		if (child4)
			child4->triangulateBlocks();
	 }
}

void
Mapquad::selectLOD(int level, int x, int y)
{
	 if (this->level == NUM_LEVELS - 1)
	 {
		int dist = sqrt( (x-mid_x)*(x-mid_x) + (y-mid_y)*(y-mid_y) );

		if (dist<1500)
		{
			if (!block)
			{
				block = new TerrainBlock (top_x, top_y );
		
				block->setState(state);
				lod_switch->addKid ( block );
				trans->addKid ( lod_switch );
				landscape->terrain->addKid ( trans );
			}

			lod_switch->select(1);
			block->collectVertices ( level,  dist/50);
		}
		else if (block)
		{
			lod_switch->select(0);

//			trans->removeKid(block);
//			delete block;
//			block = NULL;
		}


	 }
	 else
	 {
		if (child1)
			child1->selectLOD(level, x, y);
		if (child2)
			child2->selectLOD(level, x, y);
		if (child3)
			child3->selectLOD(level, x, y);
		if (child4)
			child4->selectLOD(level, x, y);
	 }
}

void
Mapquad::selectLOD(int lod)
{ 
//	if (lod != 0)
//		return;

	if (!block)
	{
		block = new TerrainBlock (top_x, top_y );
		lod_switch->addKid ( block );
		trans->addKid ( lod_switch );
		landscape->terrain->addKid ( trans );
	}

	block->reset();
	
	block->collectVertices ( 4, (lod+1)*(lod+1)*(lod+1) );
	block->collectVertices ( 3, (lod+1)*(lod+1)*(lod+1) );
	block->collectVertices ( 2, (lod+1)*(lod+1)*(lod+1) );
	block->collectVertices ( 1, (lod+1)*(lod+1)*(lod+1) );
	block->collectVertices ( 0, (lod+1)*(lod+1)*(lod+1) );

	block->triangulateBlock();
						     
	/*
   current_lod = lod;
   
  if (lod == -1)
    {
      lod_switch->select(0);
      return;
    }
   
  if ( lod_indices[lod] > -1 )
    {
      lod_switch->selectStep ( lod_indices[lod] );
    }
  else
    {
      Mapquad *temp;
      temp = this;
      
      for (int i = 0; i < lod; i++)
	temp = temp->parent;
      
      if (temp->map_requested)
	{
	  return;
	}
      else
	{
	  temp->map_requested = 1;
	  FILE *fp;
	  char *buf = new char[256];
	  fp = fopen(debug->string("cache/%01x%05x%05x.map", level-lod, top_x,
				   top_y), "r");
	  if (fp)
	    {
	      fgets (buf, 256, fp);
	      fclose (fp);
	      Map_data hop;
	      hop.terrain = buf;
	      Mapquad::root_map->getMapquad(level-lod, top_x,
					    top_y)->setMap(hop);
	    }
	  else
	    {
	      sock->writePacket( "%d %d %d %d", CMD_MAP, level - lod, top_x,
				 top_y );
	    }
	}
    }
	*/
}

/*
void
Mapquad::divideMap(int lod, Map_data& submap, Map_data& submap1, Map_data& submap2, Map_data& submap3, Map_data& submap4) 
{
  int submap_size = MAP_SIZE / (2*(lod - (NUM_LEVELS - level) + 2 ));
  
  submap1.terrain = new char[submap_size*submap_size+1];
  submap2.terrain = new char[submap_size*submap_size+1];
  submap3.terrain = new char[submap_size*submap_size+1];
  submap4.terrain = new char[submap_size*submap_size+1];

  for (int j=0; j < submap_size; j++)
    for (int i=0; i < submap_size; i++)
      {
	submap1.terrain[j * submap_size + i] = submap.terrain[j * submap_size*2 + i];
	submap2.terrain[j * submap_size + i] = submap.terrain[j * submap_size*2 + i + submap_size];
	submap3.terrain[j * submap_size + i] = submap.terrain[(j+submap_size) * submap_size*2 + i];
	submap4.terrain[j * submap_size + i] = submap.terrain[(j+submap_size) * submap_size*2 + i + submap_size];
      }
  
  submap1.terrain[submap_size*submap_size] = '\0';
  submap2.terrain[submap_size*submap_size] = '\0';
  submap3.terrain[submap_size*submap_size] = '\0';
  submap4.terrain[submap_size*submap_size] = '\0'; 
}
*/

void
Mapquad::show()
{
  // lod->select(1);
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
