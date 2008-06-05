#pragma once

#include "BaseObject.h"

class CBaseMovableObject : public CBaseObject
{
protected:
	float m_fVelocityForward;

public:
	CBaseMovableObject();
	virtual ~CBaseMovableObject();

	virtual void Update( float fTime );
};
