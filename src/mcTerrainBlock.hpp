#ifndef __TERRAINBLOCK_HPP__
#define __TERRAINBLOCK_HPP__

#include <math.h>
#include <string.h>
#include <plib/sg.h>
#include <plib/ssg.h>

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
			TerrainBlock				(WORD x, WORD y);
			~TerrainBlock				();
	void	reset						();
	void	exchangeBorderVertices		();
	void	triangulateBlock			();
	void	collectVertices				(unsigned short, unsigned int);

private:

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
		bool enabled;

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
		DIR_SOME  = (0xf << 12),

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

	vertex& getVertexFromNeighbour	(Index mask, Index i);
	vertex& getVertex				(Index i, bool neighbour = false);
	vertex& getVertexFromCoords		(Index x, Index y);
	Index	getVertexIndex			(short x, short y, bool border);
	
	// this should be made static
	void	makeDeps				(Index level, Index x, Index y, int segment, int dir);
	void	makeDeps				();

	void	calculateErrors			();

	inline void addVertex		(Index i)
	{
		v_index[listCounter++] = i;
	}

	inline void swap ()
	{
		list[listCounter++] = list[listCounter-3];
	}

	void	triangulateQuadrant	(Index iL, Index iT, Index iR, Index level);
	
	void	resolveDependencies	(vertex&);
	DWORD	getID				() { return (m_x << 16) | m_y;	}

//	QSet	set;
//	srVector3 coords[(DIM+1)*(DIM+1)];
//	int		err[(DIM+1)*(DIM+1)];
//	bool	marked[(DIM+1)*(DIM+1)];
//	Index	list[(DIM+1)*(DIM+1)];

private:
	vertex	myVertices[(DIM+1)*(DIM+1)];
	Index	list[(DIM+1)*(DIM+1)*3];
	Index	listCounter;
	quadrant quadrants[4];
	int		numSelectedVertices;
	VertexBuffer myBuffer;
//	vertex	vertices[DIM*DIM];
	int getError(float, float);

	friend class TerrainBlockHash;
	friend class VertexBuffer;


	int previousLevel;

	Index	m_x;
	Index	m_y;
	vertex nullVertex;
	TerrainBlock	*m_next;

	static TerrainBlockHash blockHash;
};

#endif /* __TERRAINBLOCK_HPP__ */
