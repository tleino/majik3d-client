// mcCameraController.h: interface for the mcCameraController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCCAMERACONTROLLER_H__C662A4C4_C611_11D3_A108_0080296AA54F__INCLUDED_)
#define AFX_MCCAMERACONTROLLER_H__C662A4C4_C611_11D3_A108_0080296AA54F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcController.hpp"

class mcCameraController : public mcController  
{
public:
	mcCameraController();
	virtual ~mcCameraController();

	void	pitchUp		(float p)	{ m_pitch += p; }
	void	pitchDown	(float p)	{ m_pitch -= p; }
	void	setPitch	(float p)	{ m_pitch = p; }
	float	getPitch	()			{ return m_pitch; }
private:
	float m_pitch;
};

#endif // !defined(AFX_MCCAMERACONTROLLER_H__C662A4C4_C611_11D3_A108_0080296AA54F__INCLUDED_)
