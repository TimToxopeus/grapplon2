#pragma once

#include "ParticleBehaviour.h"
#include "Particle.h"

#include "Vector.h"

#include <string>
#include <vector>
#include <map>

enum EmitterType
{
	NONE,
	LINE,
	ARC
};

struct ParticleFactoryEntry
{
	CParticle *m_pParticle;
	unsigned int m_iChance;
};

class CParticleEmitter
{
private:
	// Non-changing information
	EmitterType m_eType;
	float m_fTypeParameter;
	unsigned int m_iMaxParticles;
	unsigned int m_iLifespan;
	unsigned int m_iSpawnrate;
	float m_fRadius;
	std::vector<ParticleFactoryEntry> m_vParticleFactory;
	std::map<std::string, CParticleBehaviour> m_vBehaviours;

	// Changing information
	bool m_bActive;
	unsigned int m_iCurParticles;
	unsigned int m_iAge;
	float m_fParticleSpawnTime;
	
	Vector m_vPosition, m_vDirection;

	CParticle *m_pFirst;

public:
	CParticleEmitter( EmitterType eType, float fTypeParameter, unsigned int iMaxParticles, unsigned int iLifespan, unsigned int iSpawnrate, float fRadius );
	~CParticleEmitter();

	void AddToFactory( CParticle *pParticle, unsigned int iChance );
	void AddBehaviour( std::string szName, CParticleBehaviour pBehaviour );
	CParticleBehaviour GetBehaviour( std::string szName );

	void SpawnParticle();
	void Update( float fTime );
	void Render();

	void SetPosition( Vector vPosition ) { m_vPosition = vPosition; }
	void SetDirection( Vector vDirection ) { m_vDirection = vDirection; }
	Vector GetPosition() { return m_vPosition; }
	Vector GetDirection() { return m_vDirection; }

	bool IsAlive();
	void ChangeLifespan( unsigned int iLifespan, bool resetAge = true );
	void ToggleSpawn( bool b1 = false, bool b2 = true ) { if ( b1 ) { m_bActive = b2; } else { m_bActive = !m_bActive; } }
	bool IsActive() { return m_bActive; }
};
