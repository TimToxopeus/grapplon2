#include "HealthPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"

CHealthPowerUp::CHealthPowerUp(void)
{
	m_ePowerUpType = HEALTH;
	m_pImage = new CAnimatedTexture("media/scripts/texture_powerup_repair.txt");

}

CHealthPowerUp::~CHealthPowerUp(void)
{
}

void CHealthPowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP && this->m_bIsGrabable)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookHealthPowerUp();

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/powerup_pickup.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();

	}

	CPowerUp::CollideWith(pOther, pos);
}