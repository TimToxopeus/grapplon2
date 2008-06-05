#pragma once

#include "ActiveObject.h"
#include "ParticleEmitter.h"

#include <vector>
#include <string>

class CParticleSystemManager : public IActiveObject
{
private:
	static CParticleSystemManager *m_pInstanceNear, *m_pInstanceFar;
	CParticleSystemManager(float depth);
	virtual ~CParticleSystemManager();

	std::vector<CParticleEmitter *> m_vEmitters;

	std::string ReadLine( FILE *pFile );
	CParticle *ReadParticle( std::string szEmitterScript, std::string szParticleName, CParticleEmitter *pEmitter );
	CParticleBehaviour ReadParticleBehaviour( std::string szEmitterScript, std::string szParticleBehaviourName );

public:
	static CParticleSystemManager *InstanceNear() { if ( !m_pInstanceNear ) m_pInstanceNear = new CParticleSystemManager(0.0f); return m_pInstanceNear; }
	static void DestroyNear() { if ( m_pInstanceNear ) { delete m_pInstanceNear; m_pInstanceNear = 0; } }
	static CParticleSystemManager *InstanceFar() { if ( !m_pInstanceFar ) m_pInstanceFar = new CParticleSystemManager(-4.0f); return m_pInstanceFar; }
	static void DestroyFar() { if ( m_pInstanceFar ) { delete m_pInstanceFar; m_pInstanceFar = 0; } }

	void Update( float fTime );
	void Render();

	CParticleEmitter *LoadEmitter( std::string szEmitterScript );
};
