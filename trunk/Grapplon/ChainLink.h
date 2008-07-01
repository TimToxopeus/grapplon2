#pragma once

#include "BaseObject.h"
#include <ode/ode.h>

class CPlayerObject;

class CChainLink : public CBaseObject
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
