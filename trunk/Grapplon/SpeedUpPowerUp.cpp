#include "SpeedUpPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"

CSpeedUpPowerUp::CSpeedUpPowerUp(void)
{
	m_ePowerUpType = SPEED_UP;
	m_pImage = new CAnimatedTexture("media/scripts/texture_powerup_speedup.txt");
}

CSpeedUpPowerUp::~CSpeedUpPowerUp(void)
{
}

void CSpeedUpPowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP && this->m_bIsGrabable)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookSpeedPowerUp();

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Powerup_Speed.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();

	}

	CPowerUp::CollideWith(pOther, pos);
}