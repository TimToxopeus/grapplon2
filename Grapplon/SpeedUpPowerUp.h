#pragma once
#include "PowerUp.h"

class CSpeedUpPowerUp :
	public CPowerUp
{
public:
	CSpeedUpPowerUp(void);
	~CSpeedUpPowerUp(void);

	void CollideWith(CBaseObject* pOther);

};
