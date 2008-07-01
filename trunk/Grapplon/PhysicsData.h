#pragma once

#include <ode/ode.h>
#include <vector>

#include "PlanetaryData.h"

class CBaseObject;

class PhysicsData
{
private:
	// Ignore specific objects for collisions
	std::vector<PhysicsData *> m_vIgnoreCollisions;

public:
	PhysicsData() : body(NULL), geom(NULL), m_fAirDragConst(0.0f) { }
	bool CollidesWith( PhysicsData *pData ); // Collision check for specific objects
	void ToggleIgnore( PhysicsData *pData ); // Toggle collision ignore

	CBaseObject *m_pOwner;

	dBodyID body;					// Pointer to ODE body
	dGeomID geom;					// Pointer to ODE geom
	float m_fMass;					// Mass
	float m_fGravConst;				// Grav constant
	float m_fAirDragConst;			// Airdrag constant
	float m_fRadius;				// Collision radius
	bool m_bAffectedByGravity;		// Affected by gravity
	bool m_bHasCollision;			// Affected by collisions
	bool m_bAffectedByTemperature;	// Affected by temperature

	// Hook specifics
	bool m_bIsHook;					// Is a hook

	// Operator overloading to check for ODE body match
	bool operator ==(PhysicsData &other) { return (this->body == other.body); }
	bool operator !=(PhysicsData &other) { return !(*this == other); }
};
