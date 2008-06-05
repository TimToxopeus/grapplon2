#pragma once
#include "powerup.h"

class CHealthPowerUp :
	public CPowerUp
{
public:
	CHealthPowerUp(void);
	~CHealthPowerUp(void);

	void CollideWith(CBaseObject* pOther);

};
