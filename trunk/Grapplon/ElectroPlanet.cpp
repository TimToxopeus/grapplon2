#include "ElectroPlanet.h"
#include "ODEManager.h"

CElectroPlanet::CElectroPlanet(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = ELECTRO;
	m_iTempRadius = data.tempradius;

}

CElectroPlanet::~CElectroPlanet(){}

void CElectroPlanet::Render()
{
	CPlanet::Render();
}