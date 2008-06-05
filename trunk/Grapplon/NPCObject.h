#pragma once

#include "BaseMovableObject.h"

class CNPCObject : public CBaseMovableObject
{
public:
	CNPCObject();
	virtual ~CNPCObject();

	virtual void Update( float fTime );
};
