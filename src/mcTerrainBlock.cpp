#include "mcTerrainBlock.hpp"
#include "mcPerlin.hpp"
#include "mcLandscape.hpp"
#include "mcTerrainHeightGen.hpp"

#include "mcScene.hpp"

inline float getHeight(float x, float y)
{
	return scene->getLandscape()->getHOT(x, y);
	//return 2000.0f*(float)perlin->perlinNoise_2D(x/1500.0, y/1500.0);
};

TerrainBlock::TerrainBlockHash TerrainBlock::blockHash;

TerrainBlock::TerrainBlock(WORD x, WORD y)
{
	m_x = x/(16*32);
	m_y = y/(16*32);

	quadrants[0].left	= 0;
	quadrants[0].top	= ((DIM+1)*(DIM+1)-1)/2;
	quadrants[0].right	= DIM;
	quadrants[1].left	= DIM;
	quadrants[1].top	= ((DIM+1)*(DIM+1)-1)/2;
	quadrants[1].right	= (DIM+1)*(DIM+1)-1;
	quadrants[2].left	= (DIM+1)*(DIM+1)-1;
	quadrants[2].top	= ((DIM+1)*(DIM+1)-1)/2;
	quadrants[2].right	= (DIM)*(DIM+1);
	quadrants[3].left	= (DIM)*(DIM+1);
	quadrants[3].top	= ((DIM+1)*(DIM+1)-1)/2;
	quadrants[3].right	= 0;

	m_next = NULL;
	blockHash.put(m_x, m_y, this);

	vertices = new sgVec3[(DIM+1)*(DIM+1)];
	v_index = new unsigned short[1000];
	texcoords = new sgVec2[(DIM+1)*(DIM+1)];
	t_index = v_index;
	colours = new sgVec4[1];
	sgSetVec4 (colours[0], 1.0, 1.0, 1.0, 1.0);
	c_index = new unsigned short[1000];
	memset(c_index, 0, sizeof(unsigned short)*1000);
	num_colours = 1;
	
	type |= SSG_TYPE_VTABLE ;
	gltype = GL_TRIANGLE_STRIP ;
	indexed = TRUE ;

	const float UVBias = 1.0/(DIM);

	for(int j=0;j<DIM+1;j++)
		for(int i=0;i<DIM+1;i++)
		{
			sgVec3 loc;
			loc[0] = (float)i/(float)(DIM) * BLOCK_SIZE;
			loc[1] = (float)j/(float)(DIM) * BLOCK_SIZE;
//			loc[0] = rand()*1000;
//			loc[1] = rand()*1000;
			loc[2] = getHeight(x + (float)i/(DIM) * BLOCK_SIZE,y + (float)j/(DIM) * BLOCK_SIZE);

//				loc /= 10.0;
//				loc[0] -= 0.9;
//				loc[1] -= 0.9;
			
			vertex& v = myVertices[i + j * (DIM+1)];

			sgCopyVec3 (vertices[i + j * (DIM+1)], loc);

			sgVec2 UV;
				
			UV[0] = /*UVBias+*/(float)i/DIM;//*(1.0-UVBias*2);
			UV[1] = /*UVBias+*/(float)j/DIM;//*(1.0-UVBias*2);

			sgCopyVec2 (texcoords[i + j * (DIM+1)], UV);

			v.error = 0;
			v.marked = false;
			v.enabled = false;
			v.left = 0 - 1;
			v.right = 0 -1;

			list[i + j * (DIM+1)] = i + j * (DIM+1);
		}
	numSelectedVertices = (DIM+1)*(DIM+1);

	makeDeps ( 0, DIM/2, DIM/2, 0, 1 );

	calculateErrors();

//	recalcBSphere();
	/*
	TerrainBlock *b;

	b = blockHash.get(m_x+1, m_y);
	if (b)
		exchangeBorderVertices(*b, DIR_EAST);
	b = blockHash.get(m_x-1, m_y);
	if (b)
		exchangeBorderVertices(*b, DIR_WEST);
	b = blockHash.get(m_x, m_y+1);
	if (b)
		exchangeBorderVertices(*b, DIR_SOUTH);
	b = blockHash.get(m_x, m_y-1);
	if (b)
		exchangeBorderVertices(*b, DIR_NORTH);
*/

};


TerrainBlock::~TerrainBlock()
{
	blockHash.remove(m_x, m_y);
}

/*

void 
TerrainBlock::draw_geometry()
{
	ssgVTable::draw_geometry();

	int i;

	glEnable (GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_COORD_ARRAY);

	glColor3f(0.0, 1.0, 0.0);

	glVertexPointer(3, GL_FLOAT, sizeof(vertex), &vertices[0].coord);
	glTexCoordPointer(3, GL_FLOAT, sizeof(vertex), &vertices[0].UV);

	glDrawElements(GL_TRIANGLE_STRIP, numSelectedVertices, GL_UNSIGNED_SHORT, &list);

	glDisable(GL_VERTEX_ARRAY);
*/	
	
/*
	glBegin(GL_TRIANGLE_STRIP);

	glColor3f ( 0.0, 1.0, 0.0 );
	glNormal3f ( 0.0, 0.0, 1.0 );

	for (i = 0; i < numSelectedVertices; i++)
	{
//			glColor4fv;
//			glNormal3fv;
		glTexCoord2fv ( vertices[list[i]].UV );
		glVertex3fv   ( vertices[list[i]].coord ) ;
	}
	
	glEnd ();
*/
//	visualiseBSphere();


/*
	glBegin(GL_LINES);

	glColor3f ( 0.0, 1.0, 0.0 );
	glNormal3f ( 0.0, 0.0, 1.0 );

	for (int i = 0; i < num_vertices; i++)
	{
//			glColor4fv;
//			glNormal3fv;
//			glTexCoord2fv;
		if (myVertices[v_index[i]].left != 0xffff)
		{
			glVertex3fv   ( vertices[v_index[i]] ) ;
			glVertex3fv   ( vertices[myVertices[v_index[i]].left] ) ;
		}

		if (myVertices[v_index[i]].right != 0xffff && )
		{
			glVertex3fv   ( vertices[v_index[i]] ) ;
			glVertex3fv   ( vertices[myVertices[v_index[i]].right] ) ;

		}

	}
	
	glEnd ();

}
*/
/*
void
TerrainBlock::setSphere( sgSphere& sp)
{
	boundSphere.empty();

	boundSphere.extend ( &sp );
};

void
TerrainBlock::recalcBSphere()
{
	emptyBSphere () ;

	extendBSphere ( & boundSphere ) ;
	dirtyBSphere () ; 
	bsphere_is_invalid = FALSE ;
}
*/

TerrainBlock::vertex& TerrainBlock::getVertexFromNeighbour(Index mask, Index i)
{
	Index x = (mask & DIR_WEST ? -1 : 0) + (mask & DIR_EAST ? 1 : 0);
	Index y = (mask & DIR_NORTH ? 1 : 0) + (mask & DIR_SOUTH ? -1 : 0);

	TerrainBlock *b = blockHash.get( m_x + x, m_y + y);

	if (b)
		return b->getVertex(i, true);
	else
		return nullVertex;
}

TerrainBlock::vertex& TerrainBlock::getVertexFromCoords (Index x, Index y)
{
	assert(!(x>DIM||y>DIM));

	return myVertices[ x + (DIM+1) * y];
}

TerrainBlock::vertex& TerrainBlock::getVertex (Index i, bool neighbour)
{
	if (i == 0xffff)
		return nullVertex;

	if (neighbour)
	{
		if (i & DIR_WEST)
			return getVertexFromNeighbour(DIR_WEST, i ^ DIR_WEST );
		else if (i & DIR_EAST)
			return getVertexFromNeighbour(DIR_EAST, i ^ DIR_EAST );
		else if (i & DIR_NORTH)
			return getVertexFromNeighbour(DIR_NORTH, i ^ DIR_NORTH );
		else if (i & DIR_SOUTH)
			return getVertexFromNeighbour(DIR_SOUTH, i ^ DIR_SOUTH );
		else
			return myVertices[i];
	}
	else if (i & DIR_SOME)
		return nullVertex;
	else
		return myVertices[i];
}

TerrainBlock::Index TerrainBlock::getVertexIndex (short x, short y, bool border)
{
	Index flags = 0;

//		border = false; // disabled
	if (border)
	{
		if (x < 0)
		{
			flags |= DIR_WEST;
//				x += (DIM+1);
			x = DIM;
		}
			
		if (x > DIM)
		{
			flags |= DIR_EAST;
//				x -= (DIM+1); 
			x = 0;
		}

		if (y < 0)
		{
			flags |= DIR_SOUTH;
//				y += (DIM+1); 
			y = DIM;
		}
			
		if (y > DIM)
		{
			flags |= DIR_NORTH;
//				y -= (DIM+1); 
			y = 0;
		}
		Index ret = x + (DIM+1) * y;
	
		return ret | flags;
	}

	if (x<0||x>DIM||y<0||y>DIM)
		return DISABLED;

	return x + (DIM+1) * y;
}

int
TerrainBlock::getError(float x, float y)
{
	float h1, h2, h3;

	h1 = scene->getLandscape()->getHOT(x, y);
	h2 = scene->getLandscape()->getHOT(x-16, y);
	h3 = scene->getLandscape()->getHOT(x+16, y);

	return (int)(fabs ((h2+h3)*0.5 - h1)*1000.0); 
}
//kludge mika kludge
void TerrainBlock::calculateErrors()
{
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		vertex &v = myVertices[i];
		v.error = getError(vertices[i][0], vertices[i][1]);
	}
}

void TerrainBlock::resolveDependencies ( vertex& v )
{ 
//	vertex& v;// = getVertex( i );
	if (!v.enabled)
	{
		v.enabled = true;
	}

	if (v.left != DISABLED)
	{
		vertex& left = getVertex ( v.left, true) ;

		if (!left.enabled && &left != &nullVertex)
		{
//			TerrainBlock *b;
			if (v.left & DIR_WEST)
				blockHash.get(m_x-1, m_y)->resolveDependencies(left);
			else if (v.left & DIR_EAST)
				blockHash.get(m_x+1, m_y)->resolveDependencies(left);
			else if (v.left & DIR_NORTH)
				blockHash.get(m_x, m_y+1)->resolveDependencies(left);
			else if (v.left & DIR_SOUTH)
				blockHash.get(m_x, m_y-1)->resolveDependencies(left);
			else resolveDependencies( left );
		}
	}

	if (v.right != DISABLED)
	{
		vertex& right = getVertex ( v.right, true) ;

		if (!right.enabled && &right != &nullVertex)
		{
			if (v.right & DIR_WEST)
				blockHash.get(m_x-1, m_y)->resolveDependencies(right);
			else if (v.right & DIR_EAST)
				blockHash.get(m_x+1, m_y)->resolveDependencies(right);
			else if (v.right & DIR_NORTH)
				blockHash.get(m_x, m_y+1)->resolveDependencies(right);
			else if (v.right & DIR_SOUTH)
				blockHash.get(m_x, m_y-1)->resolveDependencies(right);
			else resolveDependencies( right );
			
		}
	}
}

void TerrainBlock::exchangeBorderVertices()
{
	TerrainBlock *b;
	
	if (b = blockHash.get(m_x+1, m_y) )
	{
		for (int i = 0; i < DIM+1; i++)
		{
			vertex &t = getVertexFromCoords	( DIM, i );
			vertex &o = b->getVertexFromCoords ( 0, i );

			if ( t.enabled && !o.enabled )
				b->resolveDependencies (o);
			else if ( o.enabled && !t.enabled )
				resolveDependencies (t);
		}	
	}

	if (b = blockHash.get(m_x, m_y+1) )
	{
		for (int i = 0; i < DIM+1; i++)
		{
			vertex &t = getVertexFromCoords	( i, DIM );
			vertex &o = b->getVertexFromCoords ( i, 0 );

			if ( t.enabled && !o.enabled )
				b->resolveDependencies (o);
			else if ( o.enabled && !t.enabled )
				resolveDependencies (t);
		}	
	}  
}

void TerrainBlock::triangulateBlock()
{
	listCounter = 0;

	addVertex(0);
	myBuffer.push(0);
	previousLevel = 0;
	for (int i = 0; i < 4; i++)
	{
		if (!(previousLevel % 2))
			myBuffer.toggle();
		else
			addVertex(myBuffer.getSecondMostRecent() );
		addVertex ( quadrants[i].left);
		myBuffer.push(quadrants[i].left);
		previousLevel = 2*(LEVELS)+1;
		triangulateQuadrant(quadrants[i].left, quadrants[i].top, quadrants[i].right, 2*(LEVELS));
	}

	addVertex(0);

	numSelectedVertices = listCounter;
	num_vertices = listCounter;
	num_texcoords = listCounter;

	recalcBSphere();
}

void TerrainBlock::triangulateQuadrant(Index iL, Index iT, Index iR, Index level)
{
	if (level <= 0)
		return;

	if ( myVertices[iT].enabled )
	{
		triangulateQuadrant(iL, (iL+iR)/2, iT, level-1);
		if ( !myBuffer.contains(iT) )
		{
			if ((level + previousLevel) % 2)
				myBuffer.toggle();
			else
				addVertex(myBuffer.getSecondMostRecent() );
			addVertex(iT);
			myBuffer.push(iT);
			previousLevel = level;
		}
		triangulateQuadrant(iT, (iL+iR)/2, iR, level-1);
	}
}

void TerrainBlock::collectVertices(unsigned short l, unsigned int errMax)
{
	const int inc = DIM>>l;
	const int res = LEVELS-l;

	int j, i;
	for (j=0;j<DIM+1;j += inc)
	{
		for(i=0;i<DIM+1;i += inc)
		{ 
			if (! ((i>>res) % 2 != 0 || (j>>res) % 2 != 0))
				continue;
			if ((i>>res) % 2 != 0 && (j>>res) % 2 != 0)
				continue;

			int ind = i + j * (DIM+1);

			vertex& v = myVertices[ind];

			if (v.error >= errMax || v.marked)
			{
//						set.insert ( ind );
				v.enabled = true;

				if (v.left != 0xffff)
					getVertex(v.left).marked = true;

				if (v.right != 0xffff)
					getVertex(v.right).marked = true;

			}
			v.marked = false;
		}
	}

	
	for (j=0;j<DIM+1;j += inc)
	{
		for(int i=0;i<DIM+1;i += inc)
		{ 
			if (! ((i>>res) % 2 != 0 && (j>>res) % 2 != 0))
				continue;

			int ind = i + j * (DIM+1);

			vertex& v = myVertices[ind];

			if (v.error >= errMax || v.marked)
			{
				v.enabled = true;

				if (v.left != 0xffff)
					getVertex(v.left).marked = true;

				if (v.right != 0xffff)
					getVertex(v.right).marked = true;

			}
			v.marked = false;
		}
	}
}

void TerrainBlock::reset()
{
	for (int k =0;k<(DIM+1)*(DIM+1);k++)
		myVertices[k].enabled = false;
}

void TerrainBlock::makeDeps(Index level, Index x, Index y, int segment, int dir)
{
	const Index d = DIM>>(level+1);

	vertex& vN = getVertexFromCoords(x,	y+d);
	vertex& vW = getVertexFromCoords(x-d,	y);
	vertex& vE = getVertexFromCoords(x+d, y);
	vertex& vS = getVertexFromCoords(x,	y-d);
	vertex& vC = getVertexFromCoords(x,	y);

	vN.left = getVertexIndex ( x, y, 0 );
	vN.right = getVertexIndex ( x, y+2*d, (y+d == DIM) );

	vS.left = getVertexIndex  ( x, y-2*d, (y-d == 0) );
	vS.right = getVertexIndex ( x, y, 0 );

	vW.left = getVertexIndex ( x-2*d, y, (x-d == 0) );
	vW.right = getVertexIndex ( x, y, 0 );

	vE.left = getVertexIndex ( x, y, 0 );
	vE.right = getVertexIndex ( x+2*d, y, (x+d == DIM) );

	vC.left = getVertexIndex ( x-d, y+dir*d, 0 );
	vC.right = getVertexIndex ( x+d, y-dir*d, 0 );

//		v.left = x+d + (DIM+1)*(y+d);
//		v.right = x+d + (DIM+1)*(y-d);
//
//		vertex& v = getVertex(x+d, y);
//		v.left = x+d + (DIM+1)*(y+d);
//		v.right = x+d + (DIM+1)*(y-d);

	if (level == LEVELS-1)
		return;

	makeDeps(level+1, x-d/2,	y+d/2, 0, -1);
	makeDeps(level+1, x+d/2,	y+d/2, 1, ((segment == 0 || segment == 3) && level != 0) ? -dir : dir);
	makeDeps(level+1, x-d/2,	y-d/2, 2, ((segment == 0 || segment == 3) && level != 0) ? -dir : dir);
	makeDeps(level+1, x+d/2,	y-d/2, 3, -1);
}