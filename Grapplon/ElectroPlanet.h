#pragma once

#include "Planet.h"

class CElectroPlanet : public CPlanet
{
private:
	float m_fRespawnTime;

public:
	CElectroPlanet(PlanetaryData &data);
	virtual ~CElectroPlanet();
	
	virtual void Render();
};
