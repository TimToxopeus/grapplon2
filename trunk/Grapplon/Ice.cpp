#include "Ice.h"
#include "ODEManager.h"

CIce::CIce(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = ICE;
	m_iTempRadius = data.tempradius;

}

CIce::~CIce(){}

void CIce::Render()
{
	CPlanet::Render();
}