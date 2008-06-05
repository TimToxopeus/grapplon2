#include "SpeedUpPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "AnimatedTexture.h"

CSpeedUpPowerUp::CSpeedUpPowerUp(void)
{
	m_ePowerUpType = SPEED_UP;
	m_pImage->SetAnimation(0);
}

CSpeedUpPowerUp::~CSpeedUpPowerUp(void)
{
}

void CSpeedUpPowerUp::CollideWith(CBaseObject* pOther)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookSpeedPowerUp();
	}

	CPowerUp::CollideWith(pOther);
}