#include <string>
#include <sstream>
#include <time.h>
#include "LogManager.h"

#include "Hook.h"
#include "ChainLink.h"
#include "PlayerObject.h"
#include "Asteroid.h"
#include "PowerUp.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "Renderer.h"
#include "GameSettings.h"

#include "Core.h"
#include "State_Game.h"

CHook::CHook( CPlayerObject *pOwner )
	: m_pOwner(pOwner), m_eHookState(CONNECTED), m_bIsRadialCorrected(false), m_bHasAutoAim(true), m_pGrabbedObject(NULL), m_oHookGrabJoint(NULL), 
	  m_pLastChainJoint(NULL), m_oMiddleChainJoint(NULL), m_oAngleJoint(NULL)
{
	// State logic settings
	m_eType = HOOK;
	// Render settings
	m_pImage = new CAnimatedTexture("media/scripts/texture_hook.txt");
	m_pFrozenImage = new CAnimatedTexture("media/scripts/texture_hook_frozen.txt");
	SetDepth( -1.1f );

	// Physics settings
	CODEManager* ode = CODEManager::Instance();
	ode->CreatePhysicsData(this, &m_oPhysicsData, 32.0f);
	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	m_oPhysicsData.m_fAirDragConst = SETS->HOOK_AIR_DRAG;
	this->SetMass(SETS->HOOK_MASS);

	// Hook position + orientation
	Vector shipPosition = m_pOwner->GetPosition();
	Vector forward = m_pOwner->GetForwardVector();
	this->SetPosition(shipPosition + forward*SETS->CENT_DIST_HOOK);

	// Joints
	m_pLastChainJoint = ode->createHingeJoint("Last ChainJoint");
	m_oMiddleChainJoint = ode->createHingeJoint("Middle ChainJoint");
	m_oHookGrabJoint = ode->createHingeJoint("Hook GrabJoint");
	m_oAngleJoint = ode->createHingeJoint("Angle ChainJoint");

	// Create chains
	dJointID curJointID;
	CChainLink* curLink;
	dBodyID prevBodyID = m_pOwner->GetBody();
	Vector centerPos = shipPosition + Vector(SETS->LINK_LENGTH / 2, 0.0f, 0.0f);
	Vector anchorPoint;

	for(int i = 0; i < SETS->LINK_AMOUNT * 2; i++)
	{
		// New chainlink
		curLink = new CChainLink(pOwner);
		chainLinks.push_back(curLink);
		dBodySetPosition( curLink->GetBody(), centerPos[0], centerPos[1], centerPos[2] );

		// Joint with previous link / ship
		curJointID = ode->createHingeJoint("A LinkJoint");
		chainJoints.push_back(curJointID);
		dJointAttach(curJointID, prevBodyID, curLink->GetBody());
		anchorPoint = (i % 2 == 0 ? shipPosition : shipPosition + Vector(SETS->LINK_LENGTH, 0.0f, 0.0f));
		dJointSetHingeAnchor(curJointID, anchorPoint[0], anchorPoint[1], anchorPoint[2]);

		prevBodyID = curLink->GetBody();		// Current chain will be joint in the next iteration
	}

	// Atach Ship to last ChainLink
	chainJoints.push_back(curJointID);
	dJointAttach(m_pLastChainJoint, prevBodyID, this->m_pOwner->GetBody());
	dJointSetHingeAnchor(m_pLastChainJoint, shipPosition[0] , shipPosition[1], shipPosition[2]);

}

CHook::~CHook()
{
	
	for(int i = 0; i < SETS->LINK_AMOUNT * 2; i++)
	{
		dJointDestroy(chainJoints[i]);
		delete chainLinks[i];
	}

	if(m_pLastChainJoint)	dJointDestroy(m_pLastChainJoint);	
	if(m_oMiddleChainJoint)	dJointDestroy(m_oMiddleChainJoint);
	if(m_oHookGrabJoint) dJointDestroy(m_oHookGrabJoint);
	if(m_oAngleJoint) dJointDestroy(m_oAngleJoint);

}

void CHook::SetGrasped(PhysicsData* toGrasp)
{
	m_pGrabbedObject = toGrasp;
	m_oPhysicsData.m_bHasCollision = false;

	m_eHookState = GRASPING;
}

void CHook::Grasp()
{
	CODEManager* ode = CODEManager::Instance();
	Vector nullVec;

	// Clear previous joints on object to grasp, if applicable
	if ( m_pGrabbedObject->m_pOwner->getType() == ASTEROID )
	{
		CAsteroid* asteroid = dynamic_cast<CAsteroid*>(m_pGrabbedObject->m_pOwner);
		dJointAttach(asteroid->orbitJoint, NULL, NULL);
		asteroid->m_bIsInOrbit = false;
		asteroid->m_bIsGrabable = false;
		if(asteroid->m_fThrowTime - time(NULL) < 4 && asteroid->m_pThrowingPlayer != m_pOwner)
		{
			CGameState *pState = (CGameState *)CCore::Instance()->GetActiveState();
			pState->AddScore( m_pOwner->GetPlayerID(), SETS->SCORE_STEAL, (int)GetX(), (int)GetY() );
			m_pOwner->m_iScore += SETS->SCORE_STEAL;				// Steal bonus
		}
		asteroid->m_fThrowTime = 0;
		asteroid->m_pHoldingPlayer = m_pOwner;
		asteroid->m_pThrowingPlayer = NULL;
		asteroid->m_iMilliSecsInOrbit = 0;
	}

	if ( m_pGrabbedObject->m_pOwner->getType() == POWERUP )
	{
		CPowerUp* powerup = dynamic_cast<CPowerUp*>(m_pGrabbedObject->m_pOwner);
		powerup->m_bIsGrabable = false;
	}



	// Bring the grabbed object to rest
	m_pGrabbedObject->m_pOwner->SetLinVelocity(nullVec);
	m_pGrabbedObject->m_pOwner->SetAngVelocity(nullVec);
	m_pGrabbedObject->m_pOwner->SetForce(nullVec);

	// Create grab joint
	dJointAttach( m_oHookGrabJoint, m_oPhysicsData.body, m_pGrabbedObject->body );
	m_pGrabbedObject->m_pOwner->SetMass(0.5f, false);				// Remove movement lag
	m_pGrabbedObject->ToggleIgnore(m_pOwner->GetPhysicsData());		// Ignore colission with ship

	m_eHookState = SWINGING;
}

void CHook::Eject()
{
	m_oPhysicsData.m_bHasCollision = true;
	Vector hookPos = GetPosition();

	// Attach last chain to the hook
	dJointAttach(m_pLastChainJoint, chainLinks[SETS->LINK_AMOUNT * 2 - 1]->GetBody(), m_oPhysicsData.body);
	dJointSetHingeAnchor(m_pLastChainJoint, hookPos[0] , hookPos[1], hookPos[2]);

	// Shoot the hook forward
	Vector shipFor = m_pOwner->GetForwardVector() * 6000000.0f;
	dBodyAddForce(m_oPhysicsData.body, shipFor[0], shipFor[1], 0.0f);

	m_eHookState = HOMING;

}

void CHook::Retract()
{
	m_eHookState = RETRACTING;

	Vector shipPos = m_pOwner->GetPosition();
	Vector destPos = shipPos + m_pOwner->GetForwardVector() * SETS->CENT_DIST_HOOK;
	Vector diff = this->GetPosition() - destPos;

	if(diff.Length() > 25.0f){
		Vector change = diff * -10000.0f;
		dBodyAddForce(m_oPhysicsData.body, change[0], change[1], 0.0f);
	} else {
		Vector nullVec;

		// Reattach last link to the ship
		CChainLink* lastLink = chainLinks[SETS->LINK_AMOUNT * 2 - 1];
		lastLink->SetPosition(shipPos);
		lastLink->SetAngVelocity(nullVec);
		lastLink->SetLinVelocity(nullVec);
		m_oPhysicsData.m_pOwner->SetLinVelocity(nullVec);
		m_oPhysicsData.m_pOwner->SetAngVelocity(nullVec);
		dJointAttach(m_pLastChainJoint, lastLink->GetBody(), m_pOwner->GetBody());
		dJointSetHingeAnchor(m_pLastChainJoint, shipPos[0] , shipPos[1], 0.0f);

		if(m_bIsRadialCorrected){

			// Detach middle chainlink from ship
			dJointAttach(m_oMiddleChainJoint, NULL, NULL);

			// Attach middle chainlink to next chainlink
			chainLinks[SETS->LINK_GRASP_CON - 1]->SetPosition( shipPos + Vector(SETS->LINK_LENGTH / 2, 0.0f, 0.0f) );
			chainLinks[SETS->LINK_GRASP_CON]->SetPosition( shipPos + Vector(SETS->LINK_LENGTH / 2, 0.0f, 0.0f) );
			dJointAttach(chainJoints[SETS->LINK_GRASP_CON], chainLinks[SETS->LINK_GRASP_CON - 1]->GetBody(), chainLinks[SETS->LINK_GRASP_CON]->GetBody());
			dJointSetHingeAnchor(chainJoints[SETS->LINK_GRASP_CON], shipPos[0], shipPos[1], 0.0f); 

			m_bIsRadialCorrected = false;
		}
		
		m_oPhysicsData.m_bHasCollision = false;

		m_eHookState = CONNECTED;
	}
}

void CHook::Swing()
{
	Vector hookPos  = this->GetPosition();

//	Vector chainPos = chainLinks[LINK_AMOUNT]->GetPosition();
//	Vector dir = hookPos - chainPos;
//	dir.Normalize();
//	m_fAngle = RADTODEG2(asin(dir[1]));
//	this->SetAngVelocity(Vector(0, 0, 0));			
//	m_pOwner->SetAngVelocity(Vector(0, 0, 0));

	if(!m_bIsRadialCorrected)
	{
		Vector shipPos = m_pOwner->GetPosition();
		Vector diff =  hookPos - shipPos;

		if(diff.Length() < (SETS->LINK_AMOUNT * 2 - SETS->LINK_GRASP_CON) * SETS->LINK_LENGTH + 30)
		{

			int LINK_GRASP_CON = SETS->LINK_GRASP_CON;
			// Joint between ship and before-middle link
			chainLinks[LINK_GRASP_CON - 1]->SetPosition(shipPos);
			dJointAttach( chainJoints[LINK_GRASP_CON], chainLinks[LINK_GRASP_CON - 1]->GetBody(), this->m_pOwner->GetBody() );
			dJointSetHingeAnchor(chainJoints[LINK_GRASP_CON], shipPos[0] , shipPos[1], shipPos[2]);

			// Joint between ship and middle link
			chainLinks[LINK_GRASP_CON]->SetPosition(shipPos);
			dJointAttach( m_oMiddleChainJoint, chainLinks[LINK_GRASP_CON]->GetBody(), this->m_pOwner->GetBody() );  
			dJointSetHingeAnchor(m_oMiddleChainJoint, shipPos[0] , shipPos[1], shipPos[2]);

			diff.Normalize();
			diff *= (float) ((SETS->LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_GRASP_CON);
			Vector hookPos = diff + shipPos;
			this->SetPosition(hookPos);
			dJointAttach( m_oAngleJoint, m_oPhysicsData.body, this->m_pOwner->GetBody() );
			dJointSetHingeAnchor(m_oAngleJoint, hookPos[0] , hookPos[1], hookPos[2]);

			m_bIsRadialCorrected = true;
		} 
		else 
		{
			Vector tangent(diff[1], -diff[0], 0.0f);
			tangent.Normalize();
			tangent *= (SETS->LINK_AMOUNT * 2 - SETS->LINK_GRASP_CON) * SETS->LINK_LENGTH;
			Vector destDirection = (tangent + shipPos) - this->GetPosition();
			Vector counterForce = destDirection * 2000;
			dBodyAddForce(m_oPhysicsData.body, counterForce[0], counterForce[1], 0.0f);
		} 
	}

}

void CHook::Throw(bool playerDied)
{
	// Joint between hook and object is destroyed
	dJointAttach(m_oHookGrabJoint, NULL, NULL);

	// Joint between hook and ship is destroyed
	dJointAttach(m_oAngleJoint, NULL, NULL);

	if(m_pGrabbedObject->m_pOwner->getType() == ASTEROID)
	{
		CAsteroid* asteroid = dynamic_cast<CAsteroid*>(m_pGrabbedObject->m_pOwner);
		asteroid->m_pThrowingPlayer = m_pOwner;
		asteroid->m_pHoldingPlayer = NULL;
		asteroid->m_fThrowTime = time(NULL);
		asteroid->m_iMilliSecsInOrbit = 0;
		asteroid->m_bIsGrabable = true;
		m_pGrabbedObject->m_bAffectedByGravity = true;
	}

	if(m_pGrabbedObject->m_pOwner->getType() == POWERUP)
	{
		CPowerUp* powerup = dynamic_cast<CPowerUp*>(m_pGrabbedObject->m_pOwner);
		powerup->m_bIsGrabable = true;
	}


	// Throwed object gets updated
	m_pGrabbedObject->ToggleIgnore( m_pOwner->GetPhysicsData() );
	m_pGrabbedObject->m_bHasCollision = true;
	m_pGrabbedObject->m_pOwner->ResetMass();

	Vector nullVec;
	Vector forward = this->m_pOwner->GetForwardVector();
	Vector shipPos = this->m_pOwner->GetPosition();
	Vector hookVel = dBodyGetLinearVel( m_pGrabbedObject->body );
	Vector shipVel = dBodyGetLinearVel( this->m_pOwner->GetBody() );
	Vector objPos  = shipPos + forward * (m_pGrabbedObject->m_fRadius + m_pOwner->GetPhysicsData()->m_fRadius + 5 );
	m_pGrabbedObject->m_pOwner->SetPosition(objPos);
	m_pGrabbedObject->m_pOwner->SetLinVelocity(nullVec);

	if(m_bHasAutoAim && !playerDied)
	{
		std::vector<PhysicsData*> ships = CODEManager::Instance()->m_vPlayers;
		Vector lBorder = forward.Rotate2(SETS->AUTO_AIM_ANGLE);
		Vector rBorder = forward.Rotate2(-SETS->AUTO_AIM_ANGLE);
		Vector pPos;

		for(unsigned int i = 0; i < ships.size(); i++)
		{
			if(ships[i] == m_pOwner->GetPhysicsData()) continue;

			pPos = ships[i]->m_pOwner->GetPosition();

			if( pPos.SignedDistance( objPos, objPos + lBorder ) >= 0.0f &&
				pPos.SignedDistance( objPos, objPos + rBorder ) <= 0.0f  )
			{
				forward = pPos - objPos;
				forward.Normalize();
				break;
			}
		}
	}

	if(!playerDied) m_pGrabbedObject->m_pOwner->AddForce(forward * (shipVel.Length() + hookVel.Length()) * SETS->THROW_FORCE);
	m_pGrabbedObject = NULL;

	m_eHookState = RETRACTING;
}

void CHook::adjustPos(Vector displacement)
{
	SetPosition(GetPosition() + displacement);			// Displace the hook

	// Displace all chainlinks
	for(int i = 0; i < SETS->LINK_AMOUNT * 2; i++)
	{	
		chainLinks[i]->SetPosition(chainLinks[i]->GetPosition() + displacement);
	}
	
}

void CHook::AddChainForce(float x_force, float y_force)
{
	frontForce = Vector(x_force, -y_force, 0.0f);
}

void CHook::ApplyForceFront()
{
	if(m_eHookState == SWINGING && m_bIsRadialCorrected){

		Vector shipPos = m_pOwner->GetPosition(); //+ (m_pOwner->GetForwardVector() * CENT_DIST);

		Vector hookPos = this->GetPosition();
		Vector radial  = hookPos - shipPos;
		radial.Normalize();
		Vector tangent(radial[1], -radial[0], 0.0f);
		
		if( frontForce[0] * frontForce[1] != 0.0f ){
			float force = frontForce.Length();
			
			tangent *= force * 1;
			if(tangent.Length() > 35.0f){
				tangent.Normalize();
				tangent *= 35.0f;
			}
		
		}

		dBodyAddForce(m_oPhysicsData.body, tangent[0] * 300.0f, tangent[1] * 300.0f, 0.0f);


	} else if (m_eHookState == HOMING) {


	}
}

void CHook::Update( float fTime )
{

	switch(m_eHookState)
	{
		case CONNECTED:
		{
			Vector nullVec;
			m_fAngle = m_pOwner->GetRotation();
			Vector shipFor = m_pOwner->GetForwardVector();
			Vector shipPos = m_pOwner->GetPosition();
			SetPosition(shipPos + shipFor * SETS->CENT_DIST_HOOK);
			SetAngVelocity(nullVec);
			SetLinVelocity(nullVec);
			break;
		}
		case EJECTING:
		{
			Eject();
			break;
		}
		case GRASPING:
		{
			Grasp();
			break;
		}
		case SWINGING:
		{
			Swing();
			break;
		}
		case THROWING:
		{
			Throw();
			break;
		}
		case RETRACTING:
		{
			Retract();
			break;
		}
	}

	CBaseMovableObject::Update(fTime);

}

void CHook::HandlePlayerDied()
{

	switch(m_eHookState)
	{
		case EJECTING:
			m_eHookState = CONNECTED;
			break;
		case HOMING:
			Retract();
			break;
		case GRASPING:
			m_pGrabbedObject = NULL;
			Retract();
			break;
		case SWINGING:
			Throw(true);
			Retract();
			break;
		case THROWING:
			Throw();
			Retract();
			break;
		case RETRACTING:
			Retract();
			break;
	}

}

void CHook::SetAlpha( float fAlpha )
{
	m_fAlpha = fAlpha;
	for ( unsigned int i = 0; i<chainLinks.size(); i++ )
	{
		chainLinks[i]->SetAlpha( fAlpha );
	}
}

void CHook::SetInvincibleTime( float fTime )
{
	m_fInvincibleTime = fTime;
	for ( unsigned int i = 0; i<chainLinks.size(); i++ )
	{
		chainLinks[i]->SetInvincibleTime( fTime );
	}
}

void CHook::Render()
{
	CBaseMovableObject::Render();
	if(m_pOwner->m_fFreezeTime > 0.0f)
	{
		SDL_Rect size, target;

		size = m_pFrozenImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pFrozenImage, m_fAngle);
	}

}