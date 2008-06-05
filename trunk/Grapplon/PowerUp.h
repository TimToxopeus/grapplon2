#pragma once

#include "BaseObject.h"

enum PowerUpType { FREEZE, SPEED_UP, HEALTH, HOOK_LENGTH , GELLY, SHIELD };

class CPowerUp : public CBaseObject
{
public:
	
	CPowerUp();
	PowerUpType GetPowerupType() { return m_ePowerUpType; }
	bool m_bIsGrabable;

	virtual void CollideWith( CBaseObject *pOther);
	virtual void Render();
	virtual void Respawn();

protected:
	PowerUpType m_ePowerUpType;

};
