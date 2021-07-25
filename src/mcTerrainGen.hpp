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

#include "sg.h"

class mcTerrainGen : public mcTexGen  
{
public:
					mcTerrainGen	();
	virtual			~mcTerrainGen	();

	virtual void	getPixel		(unsigned char *ptr, double x, double y);
	virtual void	getPixels		(GLubyte *image, double nwx, double nwy, double sex, double sey, int width, int height);

private:

	sgVec3			c_shadow;
	sgVec3			c_snow;
};

#endif // !defined(AFX_MCTERRAINGEN_H__A9897833_C1C6_11D3_8175_0060080624AC__INCLUDED_)
