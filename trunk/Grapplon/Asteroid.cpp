#include "Asteroid.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"
#include "Renderer.h"
#include "Universe.h"
#include "GameSettings.h"

CAsteroid::CAsteroid(PlanetaryData &data) 
	: 	CPlanet(data), 	m_pThrowingPlayer(NULL), m_fThrowTime(0), m_iWallBounces(0), m_bIsGrabable(true), m_fRespawnTime(0.0f), 
		m_fBounceToggleTime(0.0f), m_eAsteroidState(NORMAL), m_bTempChangedThisFrame(false), m_fTemperatureTime(0)
{

	m_pImage->SetFrame(4);

	m_fTempTime = SETS->TEMP_TIME;

	m_eType = ASTEROID;
	m_oPhysicsData.m_bAffectedByTemperature = true;
}

CAsteroid::~CAsteroid(){}

void CAsteroid::Render()
{
	int frame = (int) (4.5f * -m_fTemperatureTime / m_fTempTime) + 4;
	m_pImage->SetFrame(frame);
	CBaseObject::Render();
}

void CAsteroid::OnPlanetCollide(CBaseObject *pOther)
{
	if(!this->m_bIsGrabable) return;
	
	if(m_eAsteroidType == LARGE)
	{
		Explode();				// Todo Split
	}
	else
	{
		Explode();
	}

}

void CAsteroid::LeaveField()
{
	m_fRespawnTime = 2.0f;
	m_fInvincibleTime = 2.0f;
	m_bIsGrabable = false;
	//TODO: speel animatie voor 1 seconden;
}

void CAsteroid::Explode()
{
	Vector zeroVec;

	this->SetLinVelocity(zeroVec);
	this->SetAngVelocity(zeroVec);
	this->SetForce(zeroVec);

	m_fRespawnTime = 2.0f;
	m_fInvincibleTime = 2.0f;
	m_bIsGrabable = false;
	//TODO: speel animatie van explosie voor 1 seconden
}

void CAsteroid::ReposAtArea(RespawnArea& area)
{
	int x;
	int y;

	CRenderer *pRenderer = CRenderer::Instance();
	do
	{
		x = rand()%(area.x2 - area.x1) + (area.x1);
		y = rand()%(area.y2 - area.y1) + (area.y1);
	} while ( pRenderer->ObjectsInRange( x, y, (int) m_oPhysicsData.m_fRadius) );

	SetPosition( Vector((float)x, (float)y, 0) );
}

void CAsteroid::ReposAtOrbit()
{
	int angle;
	Vector pos;
	Vector hingePos = m_pOrbitOwner->GetPosition();

	CRenderer *pRenderer = CRenderer::Instance();
	do
	{
		angle = rand()%360;
		pos = hingePos + Vector::FromAngleLength(static_cast<float>(angle), m_fOrbitLength);
	} while ( pRenderer->ObjectsInRange( static_cast<int>(pos[0]), static_cast<int>(pos[1]), (int) m_oPhysicsData.m_fRadius) );

	SetPosition( pos );

	// Create joint
	dJointAttach( orbitJoint, m_pOrbitOwner->GetBody(), this->GetBody() );
	dJointSetHingeAnchor(orbitJoint, hingePos[0], hingePos[1], 0.0f);
	m_bIsInOrbit = true;

}

void CAsteroid::Respawn()
{

	int stochast = rand()%100;
	int sum = 0;
	bool isRepossed = false;
	CODEManager* ode = CODEManager::Instance();

	for(unsigned int i = 0; i < ode->m_pUniverse->m_vRespawnAreas.size(); i++)
	{
		if(isRepossed) continue;
		sum += ode->m_pUniverse->m_vRespawnAreas[i].chance;
		if(stochast < sum) { ReposAtArea(ode->m_pUniverse->m_vRespawnAreas[i]); isRepossed = true; }
	}

	if(!isRepossed) ReposAtOrbit();

	Vector n;
	m_oPhysicsData.m_pOwner->SetLinVelocity(n);
	m_oPhysicsData.m_pOwner->SetAngVelocity(n);
	SetForce(n);

	m_fThrowTime = -1;
	m_fTemperatureTime = 0.0f;
	m_iMilliSecsInOrbit = 0;
	m_iWallBounces = 0;
	m_pHoldingPlayer = NULL;
	m_pThrowingPlayer = NULL;
	SetDepth(-1.0);
	SetAlpha(1.0);

}

void CAsteroid::Update( float fTime )
{
	if(m_fBounceToggleTime > 0.0001f){
		m_fBounceToggleTime -= fTime;
	}

	if(m_fRespawnTime > 0.0001f)					// Still respawning
	{
		m_fRespawnTime -= fTime;

		if(m_fRespawnTime < 0.0001f) m_bIsGrabable = true;						// Timer over, grabable
		if(m_fRespawnTime < 1.0f && m_fRespawnTime + fTime > 1.0f)	Respawn();	// Respawn at 1 seconds mark
	}

	if(!m_bTempChangedThisFrame && m_eAsteroidState == NORMAL && m_fTemperatureTime != 0.0f)
	{
		float newTemp = m_fTemperatureTime + (m_fTemperatureTime > 0.0f ? -fTime : fTime);
		if(newTemp * m_fTemperatureTime < 0.0f)		// Changed sign
			m_fTemperatureTime = 0.0f;
		else
			m_fTemperatureTime = newTemp;
	}

	m_bTempChangedThisFrame = false;				// Set false for next frame

	if(m_fTemperatureTime <= -m_fTempTime && m_eAsteroidState != FROZEN)
	{
		m_eAsteroidState = FROZEN;
	} 
	else if(m_fTemperatureTime > -m_fTempTime && m_fTemperatureTime < m_fTempTime && m_eAsteroidState != NORMAL)
	{
		m_eAsteroidState = NORMAL;
	}
	else if(m_fTemperatureTime >= m_fTempTime && m_eAsteroidState != ON_FIRE)
	{
		m_eAsteroidState = ON_FIRE;
	}

	CPlanet::Update( fTime );
}


void CAsteroid::IncreaseTemp(float tempTimeIncrease)
{
	m_bTempChangedThisFrame = true;

	m_fTemperatureTime += tempTimeIncrease;
	
	if(m_fTemperatureTime < -m_fTempTime)
		m_fTemperatureTime = -m_fTempTime;
	else if(m_fTemperatureTime > m_fTempTime)
		m_fTemperatureTime = m_fTempTime;

}


void CAsteroid::CollideWith(CBaseObject *pOther)
{

	if(!m_bIsInOrbit && m_pHoldingPlayer == NULL && (pOther->getType() == ORDINARY || pOther->getType() == SUN || pOther->getType() == ICE || pOther->getType() == BROKEN))
	{
		OnPlanetCollide(pOther);
	}


}