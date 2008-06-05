#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;

class CChainLink : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	CAnimatedTexture* m_pFrozenImage;

public:
	CChainLink( CPlayerObject *pOwner );
	~CChainLink();

	virtual void Update( float fTime );
	virtual void Render();

};
