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
	m_pImage->SetAnimation(0);
}

CSpeedUpPowerUp::~CSpeedUpPowerUp(void)
{
}

void CSpeedUpPowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookSpeedPowerUp();

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/powerup_pickup.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();

	}

	CPowerUp::CollideWith(pOther, pos);
}