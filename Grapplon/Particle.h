#pragma once

#include "AnimatedTexture.h"
#include "Vector.h"

#include "ParticleBehaviour.h"

#include <sdl.h>
#include <string>
#include <vector>

class CParticleEmitter;

class CParticle
{
public:
	CParticle( CParticleEmitter *pParent, std::string szSpriteScript );
	~CParticle();

	std::string m_szName;

	CParticleEmitter *m_pParent;

	Vector m_vPosition;
	Vector m_vDirection;

	std::string m_szSpriteScript;
	CAnimatedTexture *m_pSprite;
	Vector m_colour1, m_colour2;
	unsigned int m_iLifespan;
	unsigned int m_iAge;
	unsigned int m_iSize;

	std::vector<CParticleBehaviour> m_vBehaviourStyles;

	void Update( float fTime );

	// Linked List data
	CParticle *m_pPrev;
	CParticle *m_pNext;

	CParticle *Clone();
	SDL_Color GetColor();
};