// mcTerrainGen.h: interface for the mcTerrainGen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCTERRAINGEN_H__A9897833_C1C6_11D3_8175_0060080624AC__INCLUDED_)
#define AFX_MCTERRAINGEN_H__A9897833_C1C6_11D3_8175_0060080624AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcTexGen.hpp"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glu.h>

class mcTerrainGen : public mcTexGen  
{
public:
					mcTerrainGen	();
	virtual			~mcTerrainGen	();

	virtual int		getPixel		(double x, double y);
	virtual void	getPixels		(GLubyte *image, double nwx, double nwy, double sex, double sey, int width, int height);

private:

	int r1, g1, b1, r2, g2, b2;
	int r3, g3, b3, r4, g4, b4;

};

#endif // !defined(AFX_MCTERRAINGEN_H__A9897833_C1C6_11D3_8175_0060080624AC__INCLUDED_)
