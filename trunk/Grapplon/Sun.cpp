#include "Sun.h"
#include "ODEManager.h"

CSun::CSun(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = SUN;
	m_iTempRadius = data.tempradius;

	dJointID sunJoint = CODEManager::Instance()->createHingeJoint();
	Vector pos = GetPosition();
	CODEManager::Instance()->JointAttach(sunJoint, m_oPhysicsData.body, 0);
	CODEManager::Instance()->JointSetHingeAnchor(sunJoint , pos);
}

CSun::~CSun(){}

void CSun::Render()
{
	CPlanet::Render();
}