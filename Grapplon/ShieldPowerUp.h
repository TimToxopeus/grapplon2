#pragma once
#include "powerup.h"

class CShieldPowerUp :
	public CPowerUp
{
public:
	CShieldPowerUp(void);
	~CShieldPowerUp(void);

	void CollideWith(CBaseObject* pOther);

};
