#include "Particle.h"

CParticle::CParticle( CParticleEmitter *pParent, std::string szSpriteScript )
{
	m_pParent = pParent;
	m_szSpriteScript = szSpriteScript;
	m_pSprite = NULL;
	if ( m_szSpriteScript != "" )
		m_pSprite = new CAnimatedTexture( m_szSpriteScript );

	m_iAge = 0;
	m_iSize = 2;

	m_pPrev = NULL;
	m_pNext = NULL;
}

CParticle::~CParticle()
{
	if ( m_pSprite )
		delete m_pSprite;
}

CParticle *CParticle::Clone()
{
	CParticle *pNewParticle = new CParticle( m_pParent, m_szSpriteScript );

	pNewParticle->m_szName = m_szName;

	pNewParticle->m_colour1 = m_colour1;
	pNewParticle->m_colour2 = m_colour2;
	pNewParticle->m_iLifespan = m_iLifespan;
	pNewParticle->m_iAge = 0;
	pNewParticle->m_iSize = m_iSize;

	for ( unsigned int i = 0; i<m_vBehaviourStyles.size(); i++ )
		pNewParticle->m_vBehaviourStyles.push_back( m_vBehaviourStyles[i] );

	return pNewParticle;
}

SDL_Color CParticle::GetColor()
{
	Vector delta = m_colour1 - m_colour2;
	float age = 1.0f - ((float)m_iAge / (float)m_iLifespan);
	Vector finalColor = m_colour1 + (delta * age);

	SDL_Color c;
	c.r = (Uint8)finalColor[0];
	c.g = (Uint8)finalColor[1];
	c.b = (Uint8)finalColor[2];
	return c;
}

void CParticle::Update( float fTime )
{
	Vector moveDir;
	for ( unsigned int i = 0; i<m_vBehaviourStyles.size(); i++ )
		moveDir += m_vBehaviourStyles[i].ComputeDirection( m_vDirection ) * fTime;
	m_vPosition += moveDir;

	if ( m_pSprite )
		m_pSprite->UpdateFrame( fTime );

	m_iAge += (unsigned int)(fTime * 1000.0f);
}
