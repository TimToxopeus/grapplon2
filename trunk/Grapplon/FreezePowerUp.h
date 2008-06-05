#pragma once
#include "powerup.h"

class CFreezePowerUp :
	public CPowerUp
{
public:
	CFreezePowerUp();
	virtual ~CFreezePowerUp();

private:

	void CollideWith(CBaseObject* pOther);


};
