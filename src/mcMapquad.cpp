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
#include "mcDebug.hpp"
#include "mcError.hpp"
#include "mcTerrainBlock.hpp"
#include "mcScene.hpp"

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

//ssgSimpleState *Mapquad::state = NULL;
static ssgEntity *kukkaModel = NULL;

Mapquad Mapquad::root_map(0, 0, 0, 0);

void
Mapquad::triangulate(int x, int y)
{
	root_map.resetBlocks();
	root_map.selectLOD(4, x, y);
	root_map.selectLOD(3, x, y);
	root_map.selectLOD(2, x, y);
	root_map.selectLOD(1, x, y);
	root_map.selectLOD(0, x, y);
	root_map.exchangeBorders();
	root_map.triangulateBlocks();
}

void Mapquad::clean(int x, int y)
{
	root_map.cleanRecursive(x, y);
}

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
  this->mapReceived = false;
  
  this->mid_x = top_x + quad_sizes[level]/2*32;
  this->mid_y = top_y + quad_sizes[level]/2*32;
  
  this->reference_count = 0;
  this->map_requested = 0;
  this->current_lod = -1;

  lod_switch = new ssgSelector();
  trans = new ssgTransform();
  trans->ref();

	block = NULL;

  sgVec3 pos;
  pos[0] = (float)top_x;
  pos[1] = (float)top_y;
  pos[2] = 0.0f;
  
  trans->setTransform( pos );
//  lod_switch->select(0);
  
  if (level == NUM_LEVELS - 1)
    {
	  incRef();
      lod_indices = new int[LOD_LEVELS];
      for (int i = 0; i < LOD_LEVELS; i++)
	lod_indices[i] = -2;
    }

  /* kludgetusta  */
/*
  if (!state)
  {
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
  */
}

Mapquad::~Mapquad()
{
	if (block)
	{
		lod_switch->removeKid(block);
		trans->removeKid(lod_switch);
		scene->getLandscape()->getTerrain()->removeKid(trans);
	}

/*  
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
	*/
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
  if (parent != NULL)
  {
	  if (parent->child1 == this) parent->child1 = NULL;
	  if (parent->child2 == this) parent->child2 = NULL;
	  if (parent->child3 == this) parent->child3 = NULL;
	  if (parent->child4 == this) parent->child4 = NULL;

	  parent->decRef();
  }
 
  if (level != 0)
    delete this;
  else
	  error->put(mcError::ERROR_WARNING, "mapquad: Refusing to delete root map.");
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
Mapquad::exchangeBorders()
{
	 if (level == NUM_LEVELS - 1)
	 {
		 if (block)
		 {
			block->exchangeBorderVertices();
		 }

	 }
	 else
	 {
		if (child1)
			child1->exchangeBorders();
		if (child2)
			child2->exchangeBorders();
		if (child3)
			child3->exchangeBorders();
		if (child4)
			child4->exchangeBorders();
	 }

}

void
Mapquad::triangulateBlocks()
{
	 if (level == NUM_LEVELS - 1)
	 {
		 if (block)
		 {
			block->triangulateBlock();
		 }
/*
		 ssgEntity *kid;
		while (kid = getNextKid();
		{
			ssgEntity *kidinKid;
			while (kidinKid = kid->getNextKid();
			{
				sgVec3 pos = kidinKid->


			}

		}
*/
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
		int dist = (int) sqrt( (x-mid_x)*(x-mid_x) + (y-mid_y)*(y-mid_y) );

		if (dist<5500)
		{
			int newBlock = 0;
			if (!block)
			{
				newBlock = 1;
				block = new TerrainBlock (top_x, top_y );

//				sgSphere sp;
//				sp.setCenter(mid_x, mid_y, scene->getLandscape()->getHOT(mid_x, mid_y) );
//				sp.setRadius( sqrt( (mid_x-top_x)*(mid_x-top_x) + (mid_y-top_y)*(mid_y-top_y)) );
		
				ssgSimpleState *state    = new ssgSimpleState ;
				state->setTexture ( scene->getLandscape()->getTextureHandle(level, top_x, top_y) );
				state -> enable     ( GL_TEXTURE_2D ) ;
				state -> enable     ( GL_LIGHTING ) ;

//				if (config->nosmooth)
//					state -> setShadeModel ( GL_FLAT ) ;
//				else
					state -> setShadeModel ( GL_SMOOTH );

				state -> disable ( GL_COLOR_MATERIAL ) ;
				state -> enable ( GL_CULL_FACE      ) ;
				state -> disable ( GL_LIGHTING );
				state -> setColourMaterial ( GL_NONE ) ;
				state -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
				state -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
				state -> setShininess ( 0 ) ;
				state -> setOpaque () ;
				state -> disable ( GL_BLEND ) ;

				block->setState(state);
//				block->setTraversalMaskBits ( SSGTRAV_CULL );
				lod_switch->addKid ( block );
				trans->addKid ( lod_switch );
				scene->getLandscape()->getTerrain()->addKid ( trans );
			}

			lod_switch->select(1);
			block->collectVertices ( level,  dist/2);
//			block->triangulateBlock();

			if (newBlock)
				while ((rand() % 10))
				{
					if (!kukkaModel)
						kukkaModel = ssgLoadAC ("firtreelod3.ac");

					ssgTransform *kukka = new ssgTransform ();
					sgCoord tmpPos;

					tmpPos.xyz[0] = top_x + rand() % 512;
					tmpPos.xyz[1] = top_y + rand() % 512;
					tmpPos.xyz[2] = scene->getLandscape()->getHOT(tmpPos.xyz[0], tmpPos.xyz[1]);
					tmpPos.hpr[1] = 0.0f;
					tmpPos.hpr[2] = 0.0f;

					kukka->addKid (kukkaModel);
					kukka->clrTraversalMaskBits (SSGTRAV_ISECT|SSGTRAV_HOT);

					kukka->setTransform (&tmpPos, 2.5f*2, 2.5f*2, 5.0f*2);

					ssgRangeSelector *rangeSelect = new ssgRangeSelector();
					rangeSelect->addKid(kukka);
					float ranges[2] = { 0, 100000 };
					rangeSelect->setRanges ( ranges, 2 );
					
					scene->getLandscape()->getTerrain()->addKid(rangeSelect);
					
					
				}
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
/*
int pnpoly(int npol, float *xp, float *yp, float x, float y)
{
  int i, j, c = 0;
  for (i = 0, j = npol-1; i < npol; j = i++) {
    if ((((yp[i]<=y) && (y<yp[j])) ||
         ((yp[j]<=y) && (y<yp[i]))) &&
        (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
      c = !c;
  }
  return c;
}
*/

int pnpoly(int npol, sgVec2* p, float x, float y)
{
  int i, j, c = 0;
  for (i = 0, j = npol-1; i < npol; j = i++) {
    if ((((p[i][1]<=y) && (y<p[j][1])) ||
         ((p[j][1]<=y) && (y<p[i][1]))) &&
        (x < (p[j][0] - p[i][0]) * (y - p[i][1]) / (p[j][1] - p[i][1]) + p[i][0]))
      c = !c;
  }
  return c;
}

/*
bool pointInTriangleTest(srVec2& tri[3], float x, float y)
{
	for (int i=0;i<3;i++)
	

}
*/

bool intersectionTest(sgVec2 tri[3], int x0, int y0, int x1, int y1)
{
	for (int i=0; i < 2; i++)
	{
		if ((tri[i][0] > x0) && (tri[i][0] < x1) && (tri[i][1] > y0) && (tri[i][1] < y1))
			return true;
	}

	if (pnpoly(3, tri, x0, y0)) return true;
	if (pnpoly(3, tri, x0, y1)) return true;
	if (pnpoly(3, tri, x1, y1)) return true;
	if (pnpoly(3, tri, x1, y0)) return true;
/*
	if (pointInTriangleTest(tri, x0, y0)) return true;
	if (pointInTriangleTest(tri, x0, y1)) return true;
	if (pointInTriangleTest(tri, x1, y1)) return true;
	if (pointInTriangleTest(tri, x1, y0)) return true;
*/	
	return false;
}

//hack
void Mapquad::cleanRecursive(int x, int y)
{
	incRef();

	if (!child1 && !child2 && !child3 && !child4)
	{
		if (sqrt((mid_x-x)*(mid_x-x)+(mid_y-y)*(mid_y-y)) > 1000.0)
		{
			decRef();
		}
	}

	if (child1) child1->cleanRecursive(x, y);
	if (child2) child2->cleanRecursive(x, y);
	if (child3) child3->cleanRecursive(x, y);
	if (child4) child4->cleanRecursive(x, y);

	decRef();
}

void Mapquad::draw(sgVec2 tri[3])
{
	// whee
	scene->getLandscape()->getTerrain()->removeAllKids();
	root_map.drawRecursive(tri);
}

void Mapquad::drawRecursive(sgVec2 tri[3])
{
	// can be optimized with pass always flag etc
	if (!intersectionTest(tri, top_x, top_y, top_x+mid_x*2, top_y+mid_y*2))
		return;

	if (level == MAX_LEVEL)
	{
		scene->getLandscape()->getTerrain()->addKid(trans);
	}
	else
	{
		if(child1) child1->drawRecursive(tri);
		if(child2) child2->drawRecursive(tri);
		if(child3) child3->drawRecursive(tri);
		if(child4) child4->drawRecursive(tri);
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
//		scene->getLandscape()->getTerrain()->addKid ( trans );
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
/*
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
*/
/*
  void
  Mapquad::printStats()
  {
  for (int i = 0; i < 13; i++)
  cout << "level " << i << " maps: " << stats[i] << endl;   
  }
*/
