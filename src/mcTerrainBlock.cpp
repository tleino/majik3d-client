
#include "mcTerrainBlock.hpp"
#include "mcPerlin.hpp"


inline float getHeight(float x, float y)
{
//	return 0.0f;
	return 2000.0f*(float)perlin->perlinNoise_2D(x/1500.0, y/1500.0);
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
			
			vertex& v = vertices[i + j * (DIM+1)];

			sgCopyVec3 (v.coord, loc);

			sgVec2 UV;
			UV[0] = (float)i/(DIM);
			UV[1] = (float)j/(DIM);
			sgCopyVec2 (v.UV, UV);

			v.error = 0;
			v.marked = false;
			v.left = -1;
			v.right = -1;

			list[i + j * (DIM+1)] = i + j * (DIM+1);
		}
	numSelectedVertices = (DIM+1)*(DIM+1);

	makeDeps ( 0, DIM/2, DIM/2, 0, 1 );

	calculateErrors();

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

inline void
TerrainBlock::exchangeBorderVertices(TerrainBlock& b, direction dir)
{
	int x = 0;
	int y = 0;
	int dx = 0;
	int dy = 0;

	switch (dir)
	{
	case DIR_NORTH:

		y = DIM;
		dx = 1;
		break;

	case DIR_SOUTH:

		y = 0;
		dx = 1;
		break;

	case DIR_WEST:

		x = 0;
		dy = 1;
		break;

	case DIR_EAST:

		x = DIM;
		dy = 1;
		break;

	default:
		assert(false);
		break;
	}

	for (int i = 0; i < DIM+1; i++)
	{
		vertex &t = getVertex	( dx * i + x, dy * i + y);
		vertex &o = b.getVertex ( dx * i + x, dy * i + y);

		if (t.error > o.error)
			o.error = t.error;
		else
			t.error = o.error;
	}
}



void
TerrainBlock::draw()
{
	ssgVTable::draw();
//	draw_geometry();
}

void 
TerrainBlock::draw_geometry()
{
	int i;
/*
	glEnable (GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_COORD_ARRAY);

	glColor3f(0.0, 1.0, 0.0);

	glVertexPointer(3, GL_FLOAT, sizeof(vertex), &vertices[0].coord);
	glTexCoordPointer(3, GL_FLOAT, sizeof(vertex), &vertices[0].UV);

	glDrawElements(GL_TRIANGLE_STRIP, numSelectedVertices, GL_UNSIGNED_SHORT, &list);

	glDisable(GL_VERTEX_ARRAY);
*/	
	

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

/*

	glBegin(GL_LINES);

	glColor3f ( 0.0, 1.0, 0.0 );
	glNormal3f ( 0.0, 0.0, 1.0 );

	for (i = 0; i < numSelectedVertices; i++)
	{
//			glColor4fv;
//			glNormal3fv;
//			glTexCoord2fv;
		if (vertices[list[i]].left != 0xffff)
		{
			glVertex3fv   ( vertices[list[i]].coord ) ;
			glVertex3fv   ( vertices[vertices[list[i]].left].coord ) ;
		}

		if (vertices[list[i]].right != 0xffff)
		{
			glVertex3fv   ( vertices[list[i]].coord ) ;
			glVertex3fv   ( vertices[vertices[list[i]].right].coord ) ;

		}

	}
	
	glEnd ();
*/
}

inline void
TerrainBlock::calculateErrors()
{
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		vertex &v = vertices[i];

		if (v.left == 0xffff || v.right == 0xffff)
			v.error = 0;
		else
		{
			if ((&getVertex(v.left) != &nullVertex) && (&getVertex(v.right) != &nullVertex))
				v.error = (unsigned int)fabs(0.5*(getVertex(v.left).coord[2] + getVertex(v.right).coord[2]) - v.coord[2]);
			else
				v.error = 0;
		}
	}
}