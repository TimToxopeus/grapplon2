#pragma once

#include "BaseObject.h"
#include <vector>

class CAnimatedTexture;

// Twinkle, twinkle, little star. :)
struct Star
{
	Star(int x, int y, float timeLeft) { this->x = x; this->y = y; this->timeLeft = this->maxTime = timeLeft; }
	int x, y;
	float timeLeft;
	float maxTime;
};

class CBackground : public CBaseObject
{
private:
	CAnimatedTexture *m_pSpace;
	CAnimatedTexture *m_pNebula;
	CAnimatedTexture *m_pStar;
	float m_fNebulaOffset;

	std::vector<Star> m_vStars;

public:
	CBackground(int imageNr);
	~CBackground();

	virtual void Update( float fTime );
	virtual void Render();
};
