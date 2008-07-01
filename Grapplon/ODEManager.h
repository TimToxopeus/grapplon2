#pragma once

#include <ode/ode.h>
#include <vector>
#include "PhysicsData.h"

//Forward declarations
class CBaseObject;
class CUniverse;

#define ODE_DEPTH 10
#define MAX_CONTACTS 128
#define MAX_HINGES 64

struct Collide
{
	dBodyID b1, b2;
	unsigned int time;
};

/* // Outdated thread synchronization
struct ODEEvent
{
	int type;
	dBodyID body1, body2;
	dJointID joint;
	Vector force;
	Vector pos;
	dMass mass;
};*/

class CODEManager
{
private:
	static CODEManager *m_pInstance;
	CODEManager();
	virtual ~CODEManager();

	// Thread
	SDL_Thread *m_pThread;
	bool m_bForceThreadStop;

	// ODE
	dWorldID m_oWorld;
	dSpaceID m_oSpace;
	dJointGroupID m_oContactgroup;

	// Joint list
	std::vector<dJointID> m_vJoints;

	// Contact list
	dContactGeom m_oContacts[MAX_CONTACTS];
	int m_iContacts;

	dBodyID CreateBody();
	dGeomID CreateGeom( dBodyID body, float fRadius );
	void AddData( PhysicsData *pData );

	void ApplyGravity(float timePass);
	void ApplyMotorForceAndDrag();

	// Collisions
	void HandleCollisions();
	std::vector<Collide> m_vCollisions;
	bool HasRecentlyCollided( dBodyID b1, dBodyID b2, unsigned int curTime );

	// Outdated thread synchronization
//	bool m_bBuffer;
//	int m_iWritingToBuffer;
//	std::vector<ODEEvent> m_vBuffer1;
//	std::vector<ODEEvent> m_vBuffer2;

	// Outdated thread synchronization
//	void AddToBuffer( ODEEvent ode_event );
//	void HandleEvent( ODEEvent ode_event );

public:
	// ODE objects lists
	std::vector<PhysicsData*> m_vAsteroids;
	std::vector<PhysicsData*> m_vPlanets;	
	std::vector<PhysicsData*> m_vPlayers;	
	std::vector<PhysicsData*> m_vOthers;
	std::vector<PhysicsData*> m_vPowerUps;	

	// Universe
	CUniverse* m_pUniverse;

	static CODEManager *Instance() { if ( !m_pInstance ) m_pInstance = new CODEManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void StartEventThread();
	void StopEventThread();
	bool ShouldThreadStop() { return m_bForceThreadStop; }

	// Create a physics object
	void CreatePhysicsData( CBaseObject *pOwner, PhysicsData* d, float fRadius = 70.0f);

	void Update( float fTime );
	dJointID createHingeJoint();
	void DestroyJoint( dJointID joint );

	// ODE Wrapping functions
	void BodyAddForce( dBodyID body, Vector force );
	void BodySetForce( dBodyID body, Vector force );
	void BodySetPosition( dBodyID body, Vector position );
	void BodySetLinVel( dBodyID body, Vector velocity );
	void BodySetAngVel( dBodyID body, Vector velocity );
	void BodySetMass( dBodyID body, dMass mass );
	void JointAttach( dJointID joint, dBodyID body1, dBodyID body2 );
	void JointSetHingeAnchor( dJointID joint, Vector pos );

	// Outdated thread synchronization
//	void ProcessBuffer();

	const dWorldID& getWorld() { return m_oWorld; };
	const dSpaceID& getSpace() { return m_oSpace; };

	// Collision callback function
	void CollisionCallback( void *pData, dGeomID o1, dGeomID o2 );
};
