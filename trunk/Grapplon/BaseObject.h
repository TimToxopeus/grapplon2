#pragma once

#include "ActiveObject.h"
#include <ode/ode.h>
#include "Vector.h"
#include "PhysicsData.h"

class CAnimatedTexture; // Forward declaration

class CBaseObject : public IActiveObject
{
protected:
	PhysicsData m_oPhysicsData;
	float m_fSecondaryScale;
	float m_fAngle;
	float m_fGravitationalConstant;
	Vector frontForce;
	CAnimatedTexture *m_pImage;
	float m_fInvincibleTime;
	int m_iHitpoints, m_iMaxHitpoints;

public:
	CBaseObject();
	virtual ~CBaseObject();

	void Render();
	void Update( float fTime );

	virtual void SetPosition( float fX, float fY );
	virtual void SetPosition( Vector pos );
	Vector GetPosition();
	float GetX();
	float GetY();
	void SetRotation( float fAngle );
	float GetRotation();
	dBodyID GetBody() { return m_oPhysicsData.body; };
	PhysicsData *GetPhysicsData() { return &m_oPhysicsData; }

	void SetMass( float fMass, bool perminent = true );
	void ResetMass();
	float GetMass();

	void SetGravitationalConstant( float fGravitationalConstant ) { m_oPhysicsData.m_fGravConst = fGravitationalConstant; }
	float GetGravitationalConstant() { return m_oPhysicsData.m_fGravConst; }

	void SetLinVelocity( Vector& v );
	void SetAngVelocity( Vector& v );
	void AddForce( Vector& f );
	void SetForceFront( Vector& f );
	void SetForce( Vector f );
	virtual inline void ApplyForceFront();

	Vector GetLinVelocity();

	Vector GetForwardVector();

	virtual void CollideWith( CBaseObject *pOther);
	virtual void OnDie( CBaseObject *m_pKiller );
	virtual void IncreaseTemp( float timePassed ) {};
	int GetHitpoints() { return m_iHitpoints; };
	int GetMaxHitpoints() { return m_iMaxHitpoints; };
	virtual void SetInvincibleTime( float fTime ) { m_fInvincibleTime = fTime; };
};
