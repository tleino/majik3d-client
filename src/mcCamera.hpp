// mcCamera.h: interface for the mcCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCCAMERA_H__5F1592D3_D29D_11D3_A116_0080296AA54F__INCLUDED_)
#define AFX_MCCAMERA_H__5F1592D3_D29D_11D3_A116_0080296AA54F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ssg.h>

class Object;

class mcCamera
{
public:
	
	enum Mode
	{
		FREE,
		FIRSTPERSON,
		BEHIND
	};

	mcCamera();
	virtual ~mcCamera();

	void	setTarget	(Object *);
	void	update		(float);
	void	setMode		(Mode);

	void	pitchUp		(float p)	{ m_pitch += p; }
	void	pitchDown	(float p)	{ m_pitch -= p; }
	void	setPitch	(float p)	{ m_pitch = p; }
	float	getPitch	()			{ return m_pitch; }

	void	getPosition	(sgCoord& c) { sgCopyVec3(c.xyz, m_position.xyz); sgCopyVec3(c.hpr, m_position.hpr); }

private:

	float			m_pitch;
	sgCoord			m_position;
	Object*			m_target;
	Mode			m_mode;
};

#endif // !defined(AFX_MCCAMERA_H__5F1592D3_D29D_11D3_A116_0080296AA54F__INCLUDED_)
