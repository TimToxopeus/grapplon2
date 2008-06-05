#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;
class CChainLink;
class CAnimatedTexture;

enum HookState { CONNECTED, EJECTING, HOMING, GRASPING, SWINGING, THROWING, RETRACTING };


class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;					// The player
	std::vector<CChainLink*> chainLinks;		// Collection of chain links
	std::vector<dJointID>    chainJoints;		// Collection of chain joints
	dJointID m_pLastChainJoint;					// Last joint on the chain, used to connect to ship/hook
	dJointID m_oMiddleChainJoint;				// Middle joint on the chain, used to half the length for swinging
	dJointID m_oHookGrabJoint;					// Joint between hook and object
	dJointID m_oAngleJoint;						// Joint between hook and ship, keeps hook at a fixed length
	bool m_bIsRadialCorrected;					// Lies the hook on a fixed radius? (in swinging mode)
	bool m_bHasAutoAim;							// Should throwing be auto-aimed?
	PhysicsData *m_pGrabbedObject;				// Grabbed object
	CAnimatedTexture* m_pFrozenImage;

public:
	
	CHook( CPlayerObject *pOwner );
	~CHook();

	HookState m_eHookState;						// The current state of the hook

	bool IsDisconnected() { return m_eHookState != CONNECTED; }
	void Eject();								// Release the hook from the ship in order to grasp objects
	void Grasp();								// Grasp the object
	void SetGrasped(PhysicsData*);				// Grasp the object in next update
	void Swing();								// Swing the object
	void Throw(bool playerDied = false);		// Throw the object
	void Retract();								// Retract the hook back to the ship
	void AddChainForce(float x_force, float y_force);
	void adjustPos(Vector displacement);
	void HandlePlayerDied();

	virtual void ApplyForceFront();
	virtual void Update( float fTime );

	virtual void SetAlpha( float fAlpha );
	virtual void SetInvincibleTime( float fTime );
	virtual void Render();
};
