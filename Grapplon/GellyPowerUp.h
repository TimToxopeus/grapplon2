#pragma once
#include "powerup.h"

class CGellyPowerUp :
	public CPowerUp
{
public:
	CGellyPowerUp(void);
	~CGellyPowerUp(void);

	void CollideWith(CBaseObject* pOther);

};
