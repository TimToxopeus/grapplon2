#include "ShieldPowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"

CShieldPowerUp::CShieldPowerUp(void)
{
	m_ePowerUpType = GELLY;
	m_pImage = new CAnimatedTexture("media/scripts/texture_powerup_shield.txt");

}

CShieldPowerUp::~CShieldPowerUp(void)
{
}

void CShieldPowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP)
	{
		dynamic_cast<CPlayerObject*>(pOther)->TookShieldPowerUp();

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/powerup_pickup.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();
	}

	CPowerUp::CollideWith(pOther, pos);
}