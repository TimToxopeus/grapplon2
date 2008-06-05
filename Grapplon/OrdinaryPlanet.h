#pragma once

#include "Planet.h"

class COrdinaryPlanet : public CPlanet
{
private:
//	PlanetaryData *data;
	float m_fRespawnTime;

public:
	COrdinaryPlanet(PlanetaryData &data);
	virtual ~COrdinaryPlanet();
	
	virtual void Render();
};
