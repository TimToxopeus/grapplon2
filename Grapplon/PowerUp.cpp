#include "PowerUp.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"
#include "ODEManager.h"
#include "Renderer.h"
#include "Universe.h"


CPowerUp::CPowerUp()
{
	m_eType = POWERUP;
	m_bIsGrabable = false;

	m_pImage = new CAnimatedTexture("media/scripts/texture_powerups.txt");

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 30.0f);
	SetMass( 40.0f );
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	SetPosition(-10000, -10000);

}

void CPowerUp::Render()
{
	CBaseObject::Render();
}

void CPowerUp::CollideWith(CBaseObject *pOther, Vector &pos)
{
	if(pOther->getType() == SHIP)
	{
		CODEManager::Instance()->m_pUniverse->RemovePowerUp(this);
	}
}

void CPowerUp::Respawn()
{
	int x, y;

	CRenderer *pRenderer = CRenderer::Instance();
	CUniverse* universe = CODEManager::Instance()->m_pUniverse;
	
	do
	{
		x = rand()%((int) universe->m_fWidth*2)  - (int) universe->m_fWidth;
		y = rand()%((int) universe->m_fHeight*2) - (int) universe->m_fHeight;
	} while ( pRenderer->ObjectsInRange( x, y, 100 ) );

	Vector v = Vector( (float)x, (float)y, 0.0f );
	SetPosition( v );
	Vector n;
	Vector vel((float) (rand() % 10), (float) (rand() % 10), 0);
	m_oPhysicsData.m_pOwner->SetLinVelocity(vel);
	SetForce(n);
	this->m_bIsGrabable = true;
	this->m_oPhysicsData.m_bHasCollision = true;

	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/powerup_spawn.wav", RT_SOUND);
	if ( pSound )
		pSound->Play();
}