#pragma once

#include <ode/ode.h>
#include <vector>
//#include "ode/objects.h"
#include "PhysicsData.h"
#include "Vector.h"

//Forward declarations
class CBaseObject;
class CUniverse;

#define ODE_DEPTH 10
#define MAX_CONTACTS 128
#define MAX_HINGES 64
#define SOUNDTIME 500

struct Collide
{
	dBodyID b1, b2;
	unsigned int time;
};

struct ODEEvent
{
	int type;
	dBodyID body1, body2;
	dJointID joint;
	Vector force;
	Vector pos;
	dMass mass;
};

class CODEManager
{
private:
	static CODEManager *m_pInstance;
	CODEManager();
	virtual ~CODEManager();

	SDL_Thread *m_pThread;
	bool m_bForceThreadStop;

	dWorldID m_oWorld;
	dSpaceID m_oSpace;
	dJointGroupID m_oContactgroup;
	dJointGroupID m_oJointgroup;

	std::vector<dJointID> m_vJoints;

	bool m_bBuffer;
	int m_iWritingToBuffer;
	std::vector<ODEEvent> m_vBuffer1;
	std::vector<ODEEvent> m_vBuffer2;

	dContactGeom m_oContacts[MAX_CONTACTS];
	int m_iContacts;

	dBodyID CreateBody();
	dGeomID CreateGeom( dBodyID body, float fRadius );
	void AddData( PhysicsData *pData );

	void ApplyGravity(float timePass);
	void ApplyMotorForceAndDrag();

	void HandleCollisions();
	std::vector<Collide> m_vCollisions;
	bool HasRecentlyCollided( dBodyID b1, dBodyID b2, unsigned int curTime );

	void AddToBuffer( ODEEvent ode_event );
	void HandleEvent( ODEEvent ode_event );

	bool m_bInWorldStep;
	bool m_bOnHold;

public:

	std::vector<PhysicsData *> m_vAsteroids;
	std::vector<PhysicsData *> m_vPlanets;	
	std::vector<PhysicsData *> m_vPlayers;	
	std::vector<PhysicsData *> m_vOthers;

	CUniverse* m_pUniverse;

	static CODEManager *Instance() { if ( !m_pInstance ) m_pInstance = new CODEManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void StartEventThread();
	void StopEventThread();
	bool ShouldThreadStop() { return m_bForceThreadStop; }

	void CreatePhysicsData( CBaseObject *pOwner, PhysicsData* d, float fRadius = 70.0f);

	void Update( float fTime );
	dJointID createHingeJoint(char* name);
	dJointID CreateJoint( dBodyID b1, dBodyID b2, float x = 0, float y = 0 );
	void DestroyJoint( dJointID joint );

	//int m_iWidth;
	//int m_iHeight;
	//int m_iBoundaryForce;

	void BodyAddForce( dBodyID body, Vector force );
	void BodySetForce( dBodyID body, Vector force );
	void BodySetPosition( dBodyID body, Vector position );
	void BodySetLinVel( dBodyID body, Vector velocity );
	void BodySetAngVel( dBodyID body, Vector velocity );
	void BodySetMass( dBodyID body, dMass mass );
	void JointAttach( dJointID joint, dBodyID body1, dBodyID body2 );
	void JointSetHingeAnchor( dJointID joint, Vector pos );
	void ProcessBuffer();

	const dWorldID& getWorld() { return m_oWorld; };
	const dSpaceID& getSpace() { return m_oSpace; };

	void CollisionCallback( void *pData, dGeomID o1, dGeomID o2 );
	void WaitUntilWorldstepOver( bool putOnHold = false );
	void ThreadContinue() { m_bOnHold = false; }
	void SetWorldStep( bool bWorldStep ) { m_bInWorldStep = bWorldStep; }
	void WaitOnHold();
};
