// mcCamera.cpp: implementation of the mcCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "mcCamera.hpp"
#include "mcObject.hpp"
#include "mcScene.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mcCamera::mcCamera()
{
	m_target = NULL;
	m_mode = FREE;
	m_pitch = 0.0;
	m_maxpitch = 60.0;
	m_minpitch = -50.0;
	sgSetVec3 ( m_position.xyz, 0.0, 0.0, 0.0 );
	sgSetVec3 ( m_position.hpr, 0.0, 0.0, 0.0 );
} 

mcCamera::~mcCamera()
{

}

void
mcCamera::pitch(float p)
{
	m_pitch += p;
	if (m_pitch > m_maxpitch)
		m_pitch = m_maxpitch;
	if (m_pitch < m_minpitch)
		m_pitch = m_minpitch;
}

void
mcCamera::setTarget(Object *target)
{
	m_target = target;
}

void
mcCamera::update(float t)
{
	if (!m_target)
		return;

	sgCoord tempPos;

	sgCoord targetPos;
	targetPos = m_target->getPos ();

	float M;
	M = 5.0 * t;
	if (M > 1.0)
		M = 1.0;

	switch(m_mode) // switchit sukkaa
	{
	case FREE:
		break;

	case FIRSTPERSON:

		m_position.xyz[0] = targetPos.xyz[0];
		m_position.xyz[1] = targetPos.xyz[1];
		m_position.xyz[2] = targetPos.xyz[2] + m_target->getRadius();;

		m_position.hpr[0] = targetPos.hpr[0];
		m_position.hpr[1] = m_pitch;tempPos.hpr[1];
		m_position.hpr[2] = 0.0f;

		break;

	case BEHIND:
   
		tempPos.xyz[0] = targetPos.xyz[0] -20*sin((targetPos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);
		tempPos.xyz[1] = targetPos.xyz[1] +20*cos((targetPos.hpr[0]-180.0f)*SG_DEGREES_TO_RADIANS);

		tempPos.xyz[2] = scene->getHOT(tempPos.xyz[0], tempPos.xyz[1]) + 5.0;

		sgVec2 loc1;
		sgVec2 loc2;

		sgSetVec2(loc1, tempPos.xyz[0],		tempPos.xyz[1]);
		sgSetVec2(loc2, targetPos.xyz[0],	targetPos.xyz[1]);

		tempPos.hpr[0] = targetPos.hpr[0];
		tempPos.hpr[1] = 90.0 - SG_RADIANS_TO_DEGREES*atan2(sgDistanceVec2(loc1, loc2), (targetPos.xyz[2] + m_target->getLenY() - tempPos.xyz[2]) );
		tempPos.hpr[2] = 0.f;

		m_position.xyz[0] += M * (tempPos.xyz[0] - m_position.xyz[0]);
		m_position.xyz[1] += M * (tempPos.xyz[1] - m_position.xyz[1]);
		m_position.xyz[2] += M * (tempPos.xyz[2] - m_position.xyz[2]);
		m_position.hpr[0] += M * (tempPos.hpr[0] - m_position.hpr[0]);
		m_position.hpr[1] += M * (tempPos.hpr[1] - m_position.hpr[1]);
		m_position.hpr[2] += M * (tempPos.hpr[2] - m_position.hpr[2]);		

		break;

	default:
		assert(false);
		break;
	}

	ssgSetCamera ( &m_position ) ;
}

void
mcCamera::setMode(Mode m)
{
	m_mode = m;
}