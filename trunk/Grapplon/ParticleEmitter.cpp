#include "ParticleEmitter.h"
#include "Renderer.h"
#include "LogManager.h"

CParticleEmitter::CParticleEmitter( EmitterType eType, float fTypeParameter, unsigned int iMaxParticles, unsigned int iLifespan, unsigned int iSpawnrate, float fRadius )
{
	m_eType = eType;
	m_fTypeParameter = fTypeParameter;
	m_iMaxParticles = iMaxParticles;
	m_iLifespan = iLifespan;
	m_iSpawnrate = iSpawnrate;
	m_fRadius = fRadius;

	m_iCurParticles = 0;
	m_iAge = 0;
	m_fParticleSpawnTime = 0.0f;

	m_vDirection = Vector( 0, 1, 0 );
	m_bActive = true;

	m_pFirst = NULL;
}

CParticleEmitter::~CParticleEmitter()
{
	while ( m_pFirst )
	{
		CParticle *pParticle = m_pFirst->m_pNext;
		delete m_pFirst;
		m_pFirst = pParticle;
	}

	for ( unsigned int i = 0; i<m_vParticleFactory.size(); i++ )
	{
		delete m_vParticleFactory[i].m_pParticle;
	}
	m_vParticleFactory.clear();
}

void CParticleEmitter::AddToFactory( CParticle *pParticle, unsigned int iChance )
{
	ParticleFactoryEntry e;
	e.m_pParticle = pParticle;
	e.m_iChance = iChance;
	m_vParticleFactory.push_back( e );
}

void CParticleEmitter::AddBehaviour( std::string szName, CParticleBehaviour pBehaviour )
{
	m_vBehaviours[szName] = pBehaviour;
}

CParticleBehaviour CParticleEmitter::GetBehaviour( std::string szName )
{
	std::map<std::string, CParticleBehaviour>::iterator it;
	it = m_vBehaviours.find( szName );
	if ( it == m_vBehaviours.end() )
		return CParticleBehaviour( "NULL", 0.0f, 0.0f );;
	return (*it).second;
}

void CParticleEmitter::SpawnParticle()
{
	if ( m_iCurParticles >= m_iMaxParticles )
		return;

	if ( !m_bActive )
		return;

	int random = (int)(rand()%100 + 1);
	for ( unsigned int i = 0; i < m_vParticleFactory.size(); i++ )
	{
		random -= m_vParticleFactory[i].m_iChance;
		if ( random <= 0 )
		{
			int timeLeft = m_iLifespan - m_iAge;
			CParticle *pOwner = m_vParticleFactory[i].m_pParticle;
			if ( !pOwner )
			{
				CLogManager::Instance()->LogMessage( "Michael maakt weer een zooitje van zijn particle namen!" );
				continue;
			}
			CParticle *pNewParticle = pOwner->Clone();
			pNewParticle->m_iLifespan = rand()%pNewParticle->m_iLifespan;
			CParticle *pTemp = NULL;

			if ( timeLeft > 0 && (int)pNewParticle->m_iLifespan > timeLeft )
			{
				delete pNewParticle;
				continue;
			}

			if ( m_pFirst )
			{
				pTemp = m_pFirst;
				while ( pTemp->m_pNext )
					pTemp = pTemp->m_pNext;
				pNewParticle->m_pPrev = pTemp;
				pTemp->m_pNext = pNewParticle;
			}
			else
			{
				m_pFirst = pNewParticle;
			}

			if ( m_eType == LINE )
			{
				if ( m_fTypeParameter == 1 )
					pNewParticle->m_vDirection = m_vDirection;
				else
					pNewParticle->m_vDirection = (m_vDirection * -1.0f);
			}
			else
			{
				float angle = 0.0f;
				if ( m_eType == ARC )
				{
					angle = (float)(rand()%(int)m_fTypeParameter);
					angle -= (m_fTypeParameter / 2.0f);
				}
				else
					angle = (float)(rand()%360);
				pNewParticle->m_vDirection = m_vDirection.Rotate( angle );
			}
			pNewParticle->m_vPosition = m_vPosition + (pNewParticle->m_vDirection * m_fRadius);

			m_iCurParticles++;
			return;
		}
	}
}

void CParticleEmitter::Update(float fTime)
{
	m_fParticleSpawnTime += fTime;
	unsigned int millisecPassed = (unsigned int)(m_fParticleSpawnTime * 1000);
	unsigned int mod = 0;
	if ( m_iSpawnrate != 0 )
		mod = millisecPassed % m_iSpawnrate;
	m_fParticleSpawnTime = (float)(millisecPassed - mod) / 1000.0f;

	// Calculate amount of particles
	unsigned int particles = 0;
	if ( m_iSpawnrate != 0 )
		particles = millisecPassed / m_iSpawnrate;

	// Clamp it, or if spawnrate is 0, max it out
	if ( particles > m_iMaxParticles - m_iCurParticles || m_iSpawnrate == 0 )
		particles = m_iMaxParticles - m_iCurParticles;

	if ( particles > 0 )
	{
		for ( unsigned int i = 0; i<particles; i++ )
			SpawnParticle();
	}

	CParticle *pTemp = m_pFirst;
	while ( pTemp )
	{
		pTemp->Update( fTime );
		if ( pTemp->m_iAge >= pTemp->m_iLifespan ) // Check if this particle has died of age
		{
			CParticle *pNext = pTemp->m_pNext;
			if ( pTemp->m_pPrev ) // Check if it has a previous node
			{
				if ( pTemp->m_pNext )
				{
					pTemp->m_pPrev->m_pNext = pTemp->m_pNext;
					pTemp->m_pNext->m_pPrev = pTemp->m_pPrev;
				}
				else
					pTemp->m_pPrev->m_pNext = NULL;
			}
			else // If it has no previous node this is the first node
			{
				if ( pTemp->m_pNext )
				{
					pTemp->m_pNext->m_pPrev = NULL;
					m_pFirst = pTemp->m_pNext;
				}
				else
					m_pFirst = NULL;
			}
			delete pTemp;
			pTemp = pNext;
			m_iCurParticles--;
		}
		else
			pTemp = pTemp->m_pNext;
	}

	if ( m_iSpawnrate == 0 )
	{
		m_bActive = false;
	}
	m_iAge += (unsigned int)(fTime * 1000.0f);
}

void CParticleEmitter::Render()
{
	CRenderer *pRenderer = CRenderer::Instance();
	CParticle *pTemp = m_pFirst;
	while ( pTemp )
	{
		float alpha = 1.0f - ((float)pTemp->m_iAge / (float)pTemp->m_iLifespan);
		SDL_Rect target;
		target.w = target.h = pTemp->m_iSize;
		target.x = (Sint16)pTemp->m_vPosition[0];
		target.y = (Sint16)pTemp->m_vPosition[1];

		pRenderer->RenderQuad( target, pTemp->m_pSprite, 0, pTemp->GetColor(), alpha );

		pTemp = pTemp->m_pNext;
	}
}

bool CParticleEmitter::IsAlive()
{
	if ( m_iLifespan > 0 )
		return (m_iAge < m_iLifespan);
	return true;
}

void CParticleEmitter::ChangeLifespan( unsigned int iLifespan, bool resetAge )
{
	m_iLifespan = iLifespan;
	if ( resetAge )
		m_iAge = 0;
}
