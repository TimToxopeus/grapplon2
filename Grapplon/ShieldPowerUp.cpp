#include "ShieldPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "AnimatedTexture.h"

CShieldPowerUp::CShieldPowerUp(void)
{
	m_ePowerUpType = GELLY;
	m_pImage->SetAnimation(4);

}

CShieldPowerUp::~CShieldPowerUp(void)
{
}

void CShieldPowerUp::CollideWith(CBaseObject* pOther)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookShieldPowerUp();
	}

	CPowerUp::CollideWith(pOther);
}