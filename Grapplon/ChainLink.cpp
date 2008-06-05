#include <ode/ode.h>
//#include <ode/mass.h>

#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_LENGTH 50.0f
#define LINK_MASS 0.1f

CChainLink::CChainLink( CPlayerObject *pOwner )
	: m_pOwner(pOwner)
{
	m_eType = CHAINLINK;
	m_pImage = new CAnimatedTexture("media/scripts/texture_chain.txt");
	SetDepth( -1.1f );
	m_pFrozenImage = new CAnimatedTexture("media/scripts/texture_chain_frozen.txt");

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 0.0f);
	m_oPhysicsData.m_fAirDragConst = 0.5f;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	SetMass( 0.01f );

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );

}

CChainLink::~CChainLink()
{
	delete m_pImage;
}

void CChainLink::Update( float fTime )
{
	CBaseMovableObject::Update(fTime);
}

void CChainLink::Render()
{
	CBaseMovableObject::Render();
	if(m_pOwner->m_fFreezeTime > 0.0f)
	{
		SDL_Rect size, target;

		size = m_pFrozenImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pFrozenImage, m_fAngle);
	}
}