#include "FreezePowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"

CFreezePowerUp::CFreezePowerUp(void)
{
	m_ePowerUpType = FREEZE;
	m_pImage->SetAnimation(1);

}

CFreezePowerUp::~CFreezePowerUp(void)
{
}

void CFreezePowerUp::CollideWith(CBaseObject* pOther, Vector &pos)
{
	if(pOther->getType() == SHIP)
	{
		std::vector<PhysicsData*>& players = CODEManager::Instance()->m_vPlayers;
		std::vector<PhysicsData*>::iterator it = players.begin();
		for(it; it != players.end(); it++){
			if((*it)->m_pOwner != pOther){
				dynamic_cast<CPlayerObject*>((*it)->m_pOwner)->m_fFreezeTime = SETS->FREEZE_TIME;
			}
		}

		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/freeze_object.wav", RT_SOUND);
		if ( pSound )
			pSound->Play();
	}

	CPowerUp::CollideWith(pOther, Vector(pos));
}