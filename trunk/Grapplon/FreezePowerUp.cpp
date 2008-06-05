#include "FreezePowerUp.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "GameSettings.h"
#include "AnimatedTexture.h"

CFreezePowerUp::CFreezePowerUp(void)
{
	m_ePowerUpType = FREEZE;
	m_pImage->SetAnimation(1);

}

CFreezePowerUp::~CFreezePowerUp(void)
{
}

void CFreezePowerUp::CollideWith(CBaseObject* pOther)
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
	}

	CPowerUp::CollideWith(pOther);
}