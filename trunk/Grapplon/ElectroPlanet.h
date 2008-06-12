#pragma once

#include "Planet.h"
#include "AnimatedTexture.h"

class CElectroPlanet : public CPlanet
{
private:
	float m_fRespawnTime;
	CAnimatedTexture *m_pElectric;

public:
	CElectroPlanet(PlanetaryData &data);
	virtual ~CElectroPlanet();
	
	virtual void Render();
	virtual void Update( float fTime );
};
