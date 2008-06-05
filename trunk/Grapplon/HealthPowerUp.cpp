#include "HealthPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "AnimatedTexture.h"

CHealthPowerUp::CHealthPowerUp(void)
{
	m_ePowerUpType = HEALTH;
	m_pImage->SetAnimation(2);

}

CHealthPowerUp::~CHealthPowerUp(void)
{
}

void CHealthPowerUp::CollideWith(CBaseObject* pOther)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookHealthPowerUp();
	}

	CPowerUp::CollideWith(pOther);
}