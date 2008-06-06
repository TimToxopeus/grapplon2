#include "GellyPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"

CGellyPowerUp::CGellyPowerUp(void)
{
	m_ePowerUpType = GELLY;
	m_pImage->SetAnimation(3);

}

CGellyPowerUp::~CGellyPowerUp(void)
{
}

void CGellyPowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookJellyPowerUp();

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/powerup_pickup.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();

	}

	CPowerUp::CollideWith(pOther, pos);
}