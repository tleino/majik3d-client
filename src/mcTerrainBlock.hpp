#ifndef __TERRAINBLOCK_HPP__
#define __TERRAINBLOCK_HPP__

#include <math.h>
#include <string.h>
#include <sg.h>
#include <ssg.h>

#ifdef WIN32
#pragma inline_recursion(on)
#endif

#ifndef WIN32
typedef unsigned int DWORD;
typedef unsigned short WORD;
#endif

class TerrainBlock : public ssgVTable
{
public:

	typedef unsigned short Index;


	struct quadrant
	{
		Index left;
		Index top;
		Index right;
	};

	struct vertex
	{
//		sgVec3 coord;
//		sgVec2 UV;
		Index left;
		Index right;
		bool marked;

		unsigned int error;
	};

	enum dimensions
	{
		LEVELS = 4,
		DIM = 16,
		NUM_VERTICES = (DIM+1)*(DIM+1),
		BLOCK_SIZE = 512
	};

	enum direction
	{
		DIR_NONE  = (0x0 << 12),
		DIR_EAST  = (0x1 << 12),
		DIR_NORTH = (0x2 << 12),
		DIR_SOUTH = (0x4 << 12),
		DIR_WEST  = (0x8 << 12),

		DISABLED = 0xffff
	};


	class TerrainBlockHash
	{
	public:
		TerrainBlockHash()
		{
			m_nodes		= 0;
			m_hashSize	= 1;
			m_hash		= new TerrainBlock*[m_hashSize];
			memset (m_hash,0,sizeof(TerrainBlock*)*m_hashSize);
		}

		~TerrainBlockHash()
		{
			delete[] m_hash;
		}

		void put (WORD x, WORD y, TerrainBlock *n)
		{
			m_nodes++;
			
			if ((m_nodes) >= m_hashSize)		// getting full?
				rehash(m_hashSize*2);

			DWORD hval		= getHashVal(x,y) & (m_hashSize-1);
			n->m_next			= m_hash[hval];
			m_hash[hval]		= n;
		}

		bool remove (DWORD x, DWORD y)
		{
			DWORD hval	= getHashVal(x,y)&(m_hashSize-1);
			TerrainBlock*	p		= NULL;
			TerrainBlock*	n		= m_hash[hval];
			while (n)
			{
				if (n->m_x == x && n->m_y == y)
				{
					if (p) 
						p->m_next = n->m_next; 
					else
						m_hash[hval] = n->m_next;
					m_nodes--;
					assert (m_nodes >= 0);
					return true;
				}
				p = n;
				n = n->m_next;
			};
			return false;


		}

		inline TerrainBlock *get (WORD x, WORD y)
		{
			TerrainBlock* n = m_hash[getHashVal(x,y)&(m_hashSize-1)];
			if (!n)
				return NULL;

			DWORD id = (x<<16)|y;
			do{
				if (n->getID() == id)
					return n;
				n = n->m_next;
			} while (n);

			return NULL;
		}
		
	private:
		static inline DWORD getHashVal (DWORD x, DWORD y) 
		{ 
			return (x*(2*3*5*7*11*13*887)+y);
		}

		inline void rehash (DWORD nSize)
		{
			TerrainBlock** nt = new TerrainBlock*[nSize];
			memset (nt,0,sizeof(TerrainBlock*)*nSize);

			for (DWORD i = 0; i < m_hashSize; i++)
			{
				TerrainBlock* n = m_hash[i];
				while (n)
				{
					TerrainBlock* next = n->m_next;
					DWORD hval = getHashVal(n->m_x,n->m_y)&(nSize-1);
					n->m_next = nt[hval];
					nt[hval] = n;
					n = next;
				};
			}
			delete[] m_hash;
			m_hash		= nt;
			m_hashSize	= nSize;
		}

		TerrainBlock **m_hash;							// hash table
		DWORD		m_hashSize;						// always power of two
		DWORD		m_nodes;						// number of nodes
	};


	class VertexBuffer
	{
	public:
		VertexBuffer()
		{
			buf[0] = DISABLED;
			buf[1] = DISABLED;
			ptr = 0;
		}

		inline void push (Index i)
		{
			buf[ptr] = i;
		}

		inline void toggle()
		{
			ptr = ptr == 0 ? 1 : 0;
		}

		inline bool contains ( Index i )
		{
			if (buf[0] == i ||  buf[1] == i)
				return true;
			else
				return false;
		}

		inline Index getSecondMostRecent()
		{
			return buf[!ptr];
		}

	private:
		Index buf[2];
		Index ptr;
	};

	TerrainBlock(WORD x, WORD y);
	~TerrainBlock();

	inline vertex& getVertexFromNeighbour(Index mask, Index i)
	{
		Index x = (mask & DIR_WEST ? -1 : 0) + (mask & DIR_EAST ? 1 : 0);
		Index y = (mask & DIR_NORTH ? 1 : 0) + (mask & DIR_SOUTH ? -1 : 0);

		TerrainBlock *b = blockHash.get( m_x + x, m_y + y);

		if (b)
			return b->getVertex(i);
		else
			return nullVertex;
	}

	inline vertex& getVertex (Index i)
	{
		if (i == 0xffff)
			return nullVertex;

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

	inline vertex& getVertex (Index x, Index y)
	{
		assert(!(x>DIM||y>DIM));

		return myVertices[ x + (DIM+1) * y];
	}

	inline Index getVertexIndex (short x, short y, bool border)
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

	inline void makeDeps(Index level, Index x, Index y, int segment, int dir)
	{
		const Index d = DIM>>(level+1);

		vertex& vN = getVertex(x,	y+d);
		vertex& vW = getVertex(x-d,	y);
		vertex& vE = getVertex(x+d, y);
		vertex& vS = getVertex(x,	y-d);
		vertex& vC = getVertex(x,	y);

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

	void makeDeps()
	{
		for (int l = 1; l < LEVELS; ++l)
		{
			const int inc = DIM>>l;

			for (int j=0;j<DIM+1;j += inc)
			{
				for(int i=0;i<DIM+1;i += inc)
				{	

				}
			}
		}
	}

	inline void calculateErrors();

	inline void exchangeBorderVertices(TerrainBlock&, direction);


	class QSet
	{
	public:
		inline QSet()
		{
			reset();

		}

		inline void reset()
		{
			for(int i=0;i<(DIM+1)*(DIM+1);i++)
				activeVertices[i] = false;

		}
	
		inline bool contains (Index i)
		{
			return activeVertices[i];
		}

		inline void insert (Index i)
		{
			activeVertices[i] = true;
		}
	private:
		bool activeVertices[(DIM+1)*(DIM+1)];
	};


//	void draw();
//	void draw_geometry();

	void render()//srGERD& gerd)
	{
		
	}

/*
	inline void triangulateBlock ()
	{

		renderVertex ( v0 );
		myBuffer.push ( v0 );
		previousLevel = 0;


	}
*/
	inline void addVertex(Index i)
	{
		v_index[listCounter++] = i;
	}

	inline void swap ()
	{
		list[listCounter++] = list[listCounter-3];
//		list[listCounter++] = list[listCounter-3];
	}
/*
	inline void triangulateBlock(Index level, Index i0, Index i1, Index i2, Index i3, bool twoWay)
	{
		const d = DIM>>(level+1);

		if (0) //centerVertexOnNextLevel)
		{
//			triangulateBlock ( level+1, i0, i0+d );
//			triangulateBlock ( );
//			triangulateBlock ( );
//			triangulateBlock ( );
		}
		else
		{

			addVertex ( i0 );
			addVertex ( i1 );
			addVertex ( i0 );
			addVertex ( i2 );

			if (twoWay)
			{
//				addVertex ( corners[2] );
//				swap();
//				renderVertex ( 2back );
//				renderVertex ( oneback );

				addVertex ( i3 );
//				addVertex ( corners[0] );
			}
		}
	}
	*/

	inline void triangulateBlock()
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

	inline void triangulateQuadrant(Index iL, Index iT, Index iR, Index level)
	{
		if (level <= 0)
			return;

		if ( set.contains(iT) )
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

/*
	inline void triangulate(Index level, Index vl, vr, vt)
	{
		

		if (centerVertexOnNextLevel)
			triangulateQuadtreeBlocksOnNextLevel();
		else
			doTheTriangulationOfThisQuadrant();
	}
*/

	inline void reset()
	{
//		calculateErrors();
		set.reset();
//		for (int k =0;k<(DIM+1)*(DIM+1);k++)
//			vertices[k].marked = false;

	}

	inline void collectVertices(unsigned short l, unsigned int errMax)
	{
//		errMax = -10;

//		for (int k =0;k<(DIM+1)*(DIM+1);k++)
//			vertices[k].marked = false;

//		for (int l = LEVELS;l >= 0;l--)
//		{
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
						set.insert ( ind );

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
						set.insert ( ind );

						if (v.left != 0xffff)
							getVertex(v.left).marked = true;

						if (v.right != 0xffff)
							getVertex(v.right).marked = true;

					}
					v.marked = false;
				}
			}
//		}


/*
		numSelectedVertices = 0;
		for (int k=0;k<(DIM+1)*(DIM+1);k++)
		{
			if (set.contains(k))
				list[numSelectedVertices++] = k;
		}
*/
	}
/*
	inline void resolveDependencies ( Index i )
	{ 
		Index dep = getLeft ( i );
		if (!set.contains( dep ))
		{
			set.insert(dep);
			resolveDependencies(dep);
		} 
		dep = getRight(i);
		if (!set.contains( dep ))
		{
			set.insert(dep);
			resolveDependencies(dep);
		}
	}
*/

	DWORD	getID()
	{
		return (m_x << 16) | m_y;	
	}

public:
	vertex	myVertices[(DIM+1)*(DIM+1)];

	QSet	set;
//	srVector3 coords[(DIM+1)*(DIM+1)];
//	int		err[(DIM+1)*(DIM+1)];
//	bool	marked[(DIM+1)*(DIM+1)];
//	Index	list[(DIM+1)*(DIM+1)];
	Index	list[(DIM+1)*(DIM+1)*3];
	Index	listCounter;

	int previousLevel;

	quadrant quadrants[4];
	int		numSelectedVertices;
	VertexBuffer myBuffer;
//	vertex	vertices[DIM*DIM];

private:
	friend class TerrainBlockHash;

	Index	m_x;
	Index	m_y;
	vertex nullVertex;
	TerrainBlock	*m_next;

	static TerrainBlockHash blockHash;
};

#endif /* __TERRAINBLOCK_HPP__ */
