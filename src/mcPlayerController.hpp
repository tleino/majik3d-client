// mcPlayerController.h: interface for the mcPlayerController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCPLAYERCONTROLLER_H__C662A4C5_C611_11D3_A108_0080296AA54F__INCLUDED_)
#define AFX_MCPLAYERCONTROLLER_H__C662A4C5_C611_11D3_A108_0080296AA54F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcController.hpp"
#include "mcPlayer.hpp"

class mcPlayerController : public mcController  
{
public:
	mcPlayerController();
	virtual ~mcPlayerController();

	void moveForward() {}
	void moveBackward() {}
	void turnLeft() {}
	void turnRight() {}
private:
	Player *m_player;
};

#endif // !defined(AFX_MCPLAYERCONTROLLER_H__C662A4C5_C611_11D3_A108_0080296AA54F__INCLUDED_)
