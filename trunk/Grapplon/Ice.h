#pragma once

#include "Planet.h"

class CIce : public CPlanet
{
private:
	float m_fRespawnTime;

public:
	CIce(PlanetaryData &data);
	virtual ~CIce();
	
	virtual void Render();
};
