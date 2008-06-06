#include "WormHole.h"
#include "ODEManager.h"
#include "Vector.h"
#include "AnimatedTexture.h"
#include "PlayerObject.h"
#include <map>

CWormHole::CWormHole(PlanetaryData &data, int index) 
	:	CPlanet(data, index)
{
	m_eType = WORMHOLE;

	m_pToThrow = NULL;

	if(index == 1){
		m_pExitForce = data.exitVector1;
	} else if (index == 2){
		m_pExitForce = data.exitVector2;
	}

	m_pImage->Scale(3.0f);
	m_pGlow->Scale(3.0f);

}

CWormHole::~CWormHole(){}

void CWormHole::ThrowObject(CBaseObject* object){
	m_pToThrow = object;
}

void CWormHole::CollideWith(CBaseObject *pOther, Vector &pos)
{
	ObjectType oType = pOther->getType();
	if(oType == SUN || oType == ICE || oType == ORDINARY || oType == HOOK || oType == FIRE || oType == WORMHOLE) return;

	if(thrownObjects.find(pOther) == thrownObjects.end() || abs((float) (thrownObjects[pOther] - time(NULL))) > 2.0f ){
		twin->ThrowObject(pOther);
	}
}

void CWormHole::Update(float fTime ){

	
	if(m_pToThrow){

		Vector throwVector;
		Vector nullVec;

		m_pToThrow->SetPosition(this->GetPosition());

		if(m_pExitForce[0] == -99999999){			// Random angle, force is second parameter in m_pExitForce
			float angle = (float) (rand() % 360);
			throwVector = Vector(1.0f, 0.0f, 0.0f).Rotate2(angle) * m_pExitForce[1];
			m_pToThrow->SetLinVelocity(nullVec);
			//dBodyAddForce(m_pToThrow->GetBody(), throwVector[0] * m_pToThrow->GetMass(), throwVector[1] * m_pToThrow->GetMass(), 0.0f);
			CODEManager::Instance()->BodyAddForce( m_pToThrow->GetBody(), throwVector * m_pToThrow->GetMass() );
		} else if (m_pExitForce[0] != 0 || m_pExitForce[1] != 0) {		// Vector is fixed
			m_pToThrow->SetLinVelocity(nullVec);
			//dBodyAddForce(m_pToThrow->GetBody(), m_pExitForce[0] * m_pToThrow->GetMass(), m_pExitForce[1] * m_pToThrow->GetMass(), 0.0f);
			CODEManager::Instance()->BodyAddForce( m_pToThrow->GetBody(), m_pExitForce * m_pToThrow->GetMass() );
		}

		thrownObjects[m_pToThrow] = time(NULL);

		m_pToThrow = NULL;
	}

	CPlanet::Update(fTime);
}