
#include "mcTerrainBlock.hpp"
#include "mcPerlin.hpp"
#include "mcLandscape.hpp"


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

	const float UVBias = 1.0/Landscape::TERRAIN_RESOLUTION;

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
				
			UV[0] = UVBias+(float)i/(DIM)*(1.0-UVBias*2);
			UV[1] = UVBias+(float)j/(DIM)*(1.0-UVBias*2);

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
TerrainBlock::draw()
{
	ssgVTable::draw();
//	draw_geometry();
}
*/


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

inline void
TerrainBlock::calculateErrors()
{
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		vertex &v = myVertices[i];

		if (v.left == 0xffff || v.right == 0xffff)
			v.error = rand() % 10;
		else
		{
//			if ((&getVertex(v.left) != &nullVertex) && (&getVertex(v.right) != &nullVertex))
//				v.error = (unsigned int)fabs(0.5*(getVertex(v.left).coord[2] + getVertex(v.right).coord[2]) - v.coord[2]);
//			else
				v.error = rand() % 10;
		}
	}
}



