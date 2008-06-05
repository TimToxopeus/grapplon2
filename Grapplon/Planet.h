#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"
#include <time.h>

class CAnimatedTexture;
class CParticleEmitter;
class CPlayerObject;

class CPlanet : public CBaseObject
{
private:
//	PlanetaryData *data;
	CParticleEmitter *m_pEmitter;
	float m_fRespawnTime;

	CAnimatedTexture *m_pOrbit;
	CAnimatedTexture *m_pGlow;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();

	CPlanet* m_pOrbitOwner;
	float m_fOrbitAngle;
	float m_fOrbitLength;
	float m_fOrbitSpeed;
	float m_fRotation;
	bool m_bIsInOrbit;
	dJointID orbitJoint;

	float m_fDamageMult;
	int m_iTempRadius;
	//void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();

};
