// mcPlayerController.cpp: implementation of the mcPlayerController class.
//
//////////////////////////////////////////////////////////////////////

#include "mcPlayerController.hpp"

#include <sg.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mcPlayerController::mcPlayerController()
{

}

mcPlayerController::~mcPlayerController()
{

}

void
mcPlayerController::moveForward()
{
	sgCoord temppos;

	temppos.xyz[0] += sin((temppos.hpr[0]-180.0f)*
		SG_DEGREES_TO_RADIANS)*10.0f;
	temppos.xyz[1] -= cos((temppos.hpr[0]-180.0f)*
		SG_DEGREES_TO_RADIANS)*10.0;

/*
	sock->writePacket(	debug->string("50 %f %f %f",
						(float) temppos.xyz[0],
						(float) temppos.xyz[1],
						(float) temppos.hpr[0])); 
						*/
}