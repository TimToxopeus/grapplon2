#include "Sun.h"
#include "ODEManager.h"

CSun::CSun(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = SUN;
	m_iTempRadius = data.tempradius;

	dJointID sunJoint = CODEManager::Instance()->createHingeJoint();
	Vector pos = GetPosition();
	//dJointAttach(sunJoint, m_oPhysicsData.body, 0);
	//dJointSetHingeAnchor(sunJoint , pos[0], pos[1], 0.0f);
	CODEManager::Instance()->JointAttach(sunJoint, m_oPhysicsData.body, 0);
	CODEManager::Instance()->JointSetHingeAnchor(sunJoint , pos);
}

CSun::~CSun(){}

void CSun::Render()
{
	CPlanet::Render();
}