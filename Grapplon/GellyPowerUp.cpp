#include "GellyPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "AnimatedTexture.h"

CGellyPowerUp::CGellyPowerUp(void)
{
	m_ePowerUpType = GELLY;
	m_pImage->SetAnimation(3);

}

CGellyPowerUp::~CGellyPowerUp(void)
{
}

void CGellyPowerUp::CollideWith(CBaseObject* pOther)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookJellyPowerUp();
	}

	CPowerUp::CollideWith(pOther);
}