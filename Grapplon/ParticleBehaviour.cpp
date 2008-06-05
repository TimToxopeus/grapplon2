#include "ParticleBehaviour.h"

CParticleBehaviour::CParticleBehaviour()
{
	m_szName = "NULL";
	m_fVelocity = 0.0f;
	m_fEffect = 0.0f;
}

CParticleBehaviour::CParticleBehaviour( std::string szName, float fEffect, float fVelocity )
{
	m_szName = szName;
	m_fVelocity = fVelocity;
	m_fEffect = fEffect;
}

Vector CParticleBehaviour::ComputeDirection( Vector direction )
{
	return direction.Rotate( m_fEffect ) * m_fVelocity;
}
