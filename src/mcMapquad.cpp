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


Mapquad::Mapquad(Mapquad *parent, int level, int top_x, int top_y)
{

  m_parent = parent;
  m_child1 = NULL;
  m_child2 = NULL;
  m_child3 = NULL;
  m_child4 = NULL;
  m_level = level;
  m_mapReceived = false;
  m_top_x = top_x;
  m_top_y = top_y;
  m_mid_x = top_x + quad_sizes[level]/2*32;
  m_mid_y = top_y + quad_sizes[level]/2*32;
  
  m_refCount = 0;
  m_mapRequested = 0;
  m_currentLod = -1;

  m_lodSwitch = new ssgSelector();
  m_trans = new ssgTransform();
  
  m_block = NULL;

  sgVec3 pos;
  pos[0] = (float)top_x;
  pos[1] = (float)top_y;
  pos[2] = 0.0f;
  
  m_trans->setTransform( pos );
//  lod_switch->select(0);
  
  if (level == NUM_LEVELS - 1)
    {
      lod_indices = new int[LOD_LEVELS];
      for (int i = 0; i < LOD_LEVELS; i++)
	lod_indices[i] = -2;
    }
}

Mapquad::~Mapquad()
{
  if (m_parent != NULL)
    m_parent->decRef();
 
  delete m_block;
  
  if (m_level == NUM_LEVELS - 1 )
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
  return m_refCount++;
}

int
Mapquad::decRef()
{
  if (--m_refCount < 1)
      this->cleanUp();
  return m_refCount;
}

void
Mapquad::cleanUp()
{
  if (m_level != 0)
    delete this;
  else
	  error->put(mcError::ERROR_WARNING, "mapquad: Refusing to delete root map.");
}

Mapquad *
Mapquad::getChild1()
{
  if (this->m_child1 == NULL)
    {
      this->m_child1 = new Mapquad (this,
				  this->m_level + 1,
				  this->m_top_x,
				  m_top_y);
      this->incRef();
    }
  return this->m_child1;
}

Mapquad *
Mapquad::getChild2()
{
  if (this->m_child2 == NULL)
    {
      this->m_child2 = new Mapquad (this,
				  this->m_level + 1,
				  m_mid_x,
				  m_top_y);
      this->incRef();
    }
  return this->m_child2;
}

Mapquad *
Mapquad::getChild3()
{
  if (this->m_child3 == NULL)
    {
      this->m_child3 = new Mapquad (this,
				  this->m_level + 1,
				  this->m_top_x,
				  m_mid_y);
      this->incRef();
    }
  return this->m_child3;
}

Mapquad *
Mapquad::getChild4()
{
  if (this->m_child4 == NULL)
    {
      this->m_child4 = new Mapquad (this,
				  this->m_level + 1,
				  m_mid_x,
				  m_mid_y);
      this->incRef();
    }
  return this->m_child4;
}

bool
Mapquad::isMapReceived ()
{
  return m_mapReceived;
}

Mapquad *
Mapquad::tryMapquad(int level, int x, int y)
{
  if (m_level == level)
    return this;
   
  if (x < m_mid_x)
    {
      if (y < m_mid_y)
	{
	  if (m_child1 != NULL)
	    return m_child1->tryMapquad (level, x, y);
	  else
	    return NULL;
	}
      else
	{
	  if (m_child3 != NULL)
	    return m_child3->tryMapquad (level, x, y);
	  else
	    return NULL;
	}
    }
  else
    {
      if (y < m_mid_y)
	{
	  if (m_child2 != NULL)
	    return m_child2->tryMapquad (level, x, y);
	  else
	    return NULL;
	}
      else
	{
	  if (m_child4 != NULL)
	    return m_child4->tryMapquad (level, x, y);
	  else
	    return NULL;
	}
    }
}

Mapquad *
Mapquad::getMapquad(int level, int x, int y)
{
  if (this->m_level == level)
    return this;
  
  if (x < m_mid_x)
    {
      if (y < m_mid_y)
	return this->getChild1()->getMapquad (level, x, y);
      else
	return this->getChild3()->getMapquad (level, x, y);
    }
   else
     {
       if (y < m_mid_y)
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
  m_mapReceived = true;
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
	 if (m_level == NUM_LEVELS - 1)
	 {
		 if (m_block)
			m_block->reset();
	 }
	 else
	 {
		if (m_child1)	m_child1->resetBlocks();
		if (m_child2)	m_child2->resetBlocks();
		if (m_child3)	m_child3->resetBlocks();
		if (m_child4)	m_child4->resetBlocks();
	 }
}


void 
Mapquad::exchangeBorders()
{
	 if (m_level == NUM_LEVELS - 1)
	 {
		 if (m_block) 
			 m_block->exchangeBorderVertices();
	 }
	 else
	 {
		if (m_child1) m_child1->exchangeBorders();
		if (m_child2) m_child2->exchangeBorders();
		if (m_child3) m_child3->exchangeBorders();
		if (m_child4) m_child4->exchangeBorders();
	 }
}

void
Mapquad::triangulateBlocks()
{
	 if (m_level == NUM_LEVELS - 1)
	 {
		 if (m_block)
		 {
			m_block->triangulateBlock();
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
		if (m_child1)	m_child1->triangulateBlocks();
		if (m_child2)	m_child2->triangulateBlocks();
		if (m_child3)	m_child3->triangulateBlocks();
		if (m_child4)	m_child4->triangulateBlocks();
	 }
}

void
Mapquad::selectLOD(int level, int x, int y)
{
	 if (m_level == NUM_LEVELS - 1)
	 {
		int dist = (int) sqrt( (x-m_mid_x)*(x-m_mid_x) + (y-m_mid_y)*(y-m_mid_y) );

		if (dist<2500)
		{
			int newBlock = 0;
			if (!m_block)
			{
				newBlock = 1;
				m_block = new TerrainBlock (m_top_x, m_top_y );

//				sgSphere sp;
//				sp.setCenter(mid_x, mid_y, scene->getLandscape()->getHOT(mid_x, mid_y) );
//				sp.setRadius( sqrt( (mid_x-top_x)*(mid_x-top_x) + (mid_y-top_y)*(mid_y-top_y)) );
		
				ssgSimpleState *state    = new ssgSimpleState ;
				state->setTexture ( scene->getLandscape()->getTextureHandle(m_level, m_top_x, m_top_y) );
				state -> enable     ( GL_TEXTURE_2D ) ;
				state -> enable     ( GL_LIGHTING ) ;

//				if (config->nosmooth)
//					state -> setShadeModel ( GL_FLAT ) ;
//				else
					state -> setShadeModel ( GL_SMOOTH );

				state -> enable ( GL_COLOR_MATERIAL ) ;
				state -> enable ( GL_CULL_FACE      ) ;
				state -> disable ( GL_LIGHTING );
				state -> setColourMaterial ( GL_NONE ) ;
				state -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
				state -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
				state -> setShininess ( 0 ) ;
				state -> setOpaque () ;
				state -> disable ( GL_BLEND ) ;

//				m_block->setState(state);
//				m_block->setTraversalMaskBits ( SSGTRAV_CULL );
//				lod_switch->addKid ( m_block );
				m_trans->addKid ( m_lodSwitch );
				scene->getLandscape()->getTerrain()->addKid ( m_trans );
			}

			m_lodSwitch->select(1);
			m_block->collectVertices ( level,  dist/2);
//			m_block->triangulateBlock();

			if (newBlock)
				while ((rand() % 10))
				{
					if (!kukkaModel)
						kukkaModel = ssgLoadAC ("kukka.ac");

					ssgTransform *kukka = new ssgTransform ();
					sgCoord tmpPos;

					tmpPos.xyz[0] = rand() % 512;
					tmpPos.xyz[1] = rand() % 512;
					tmpPos.xyz[2] = scene->getHOT(m_top_x+tmpPos.xyz[0], m_top_y+tmpPos.xyz[1]);
					tmpPos.hpr[1] = 0.0f;
					tmpPos.hpr[2] = 0.0f;

					kukka->addKid (kukkaModel);
					kukka->clrTraversalMaskBits (SSGTRAV_ISECT|SSGTRAV_HOT);

					kukka->setTransform (&tmpPos);

					ssgRangeSelector *rangeSelect = new ssgRangeSelector();
					rangeSelect->addKid(kukka);
					float ranges[2] = { 0, 2000 };
					rangeSelect->setRanges ( ranges, 2 );
					
					m_trans->addKid(rangeSelect);
				}
		}
		else if (m_block)
		{
			m_lodSwitch->select(0);

//			trans->removeKid(m_block);
//			delete m_block;
//			m_block = NULL;
		}


	 }
	 else
	 {
		if (m_child1)	m_child1->selectLOD(level, x, y);
		if (m_child2)	m_child2->selectLOD(level, x, y);
		if (m_child3)	m_child3->selectLOD(level, x, y);
		if (m_child4)	m_child4->selectLOD(level, x, y);
	 }
}

void Mapquad::draw()
{
/*	if (	!pointInTriangle(top_x,			top_y, tri)
		&&	!pointInTriangle(top_x+mid_x*2, top_y, tri)
		&&	!pointInTriangle(top_x+mid_x*2, top_y+mid_y*2, tri)
		&&	!pointInTriangle(top_x,			top_y+mid_y*2, tri))
		return;
*/
	if (m_level == MAX_LEVEL)
	{
		if (m_block)
		{
			sgMat4 mtx;
			m_trans->getTransform(mtx);

			glPushMatrix();
			glMultMatrixf((float *)mtx);

			m_block->draw();

			glPopMatrix();
		}
	}
	else
	{
		if(m_child1) m_child1->draw();
		if(m_child2) m_child2->draw();
		if(m_child3) m_child3->draw();
		if(m_child4) m_child4->draw();
	}
}

void
Mapquad::selectLOD(int lod)
{ 
//	if (lod != 0)
//		return;

	if (!m_block)
	{
		m_block = new TerrainBlock (m_top_x, m_top_y );
//		lod_switch->addKid ( m_block );
		m_trans->addKid ( m_lodSwitch );
		scene->getLandscape()->getTerrain()->addKid ( m_trans );
	}

	m_block->reset();
	
	m_block->collectVertices ( 4, (lod+1)*(lod+1)*(lod+1) );
	m_block->collectVertices ( 3, (lod+1)*(lod+1)*(lod+1) );
	m_block->collectVertices ( 2, (lod+1)*(lod+1)*(lod+1) );
	m_block->collectVertices ( 1, (lod+1)*(lod+1)*(lod+1) );
	m_block->collectVertices ( 0, (lod+1)*(lod+1)*(lod+1) );

	m_block->triangulateBlock();
						     
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
	temp = temp->m_parent;
      
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
