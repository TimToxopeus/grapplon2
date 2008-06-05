#include "BaseMovableObject.h"
#include <math.h>

CBaseMovableObject::CBaseMovableObject()
{
	m_fVelocityForward = 50.0f;
}

CBaseMovableObject::~CBaseMovableObject()
{
}

void CBaseMovableObject::Update( float fTime )
{

	/*

	// Wrap angle
	if ( m_fAngle > 360.0f )
		m_fAngle -= 360.0f;
	if ( m_fAngle < 0.0f )
		m_fAngle += 360.0f;

	// Move forward
	SetPosition(
		GetX() + cos((m_fAngle - 90.0f)*(3.14f/180.0f)) * (m_fVelocityForward * fTime),
		GetY() + sin((m_fAngle - 90.0f)*(3.14f/180.0f)) * (m_fVelocityForward * fTime)
		);
	*/
	CBaseObject::Update( fTime );


}
