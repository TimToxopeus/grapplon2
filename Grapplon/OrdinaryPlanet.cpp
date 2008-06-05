#include "OrdinaryPlanet.h"
#include "ODEManager.h"
#include "Renderer.h"

COrdinaryPlanet::COrdinaryPlanet(PlanetaryData &data) : CPlanet(data)
{
	m_eType = ORDINARY;
}

COrdinaryPlanet::~COrdinaryPlanet(){}

void COrdinaryPlanet::Render()
{
	CPlanet::Render();
}