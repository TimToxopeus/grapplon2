#pragma once

#include "BaseObject.h"
#include "WiimoteListener.h"

#include <ode/ode.h>

class CParticleEmitter;

class CHook;

struct CollisionEffect
{
	CAnimatedTexture *m_pEffect;
	Vector m_vPosition;
	CBaseObject *m_pOther;
};

class CPlayerObject : public CBaseObject, public IWiimoteListener
{
private:
	float m_fShipVel;
	float m_fVelocityForward;

	CAnimatedTexture *m_pImageDamage;
	CAnimatedTexture *m_pFrozenImage;
	CAnimatedTexture *m_pExplosion;
	CAnimatedTexture *m_pJellyImage;
	CAnimatedTexture *m_pShieldImage;
	CAnimatedTexture *m_pRespawnImage;
	CAnimatedTexture *m_pPUFrozenImage;
	CAnimatedTexture *m_pElectricImage;
	CAnimatedTexture *m_pRespawnRing;

	Vector explosionPos;

	Vector diePosition;
	float timeTillDeath;
	Vector respawnDisplacement;
	bool m_bIsReinitialized;

	std::vector<CollisionEffect *> m_vCollisionEffects;

	int m_iPlayer;
	float m_fExplosionAngle;
	CHook *m_pHook;

	float timeSinceNoInput;
	float m_fRespawnTime;

	float m_fPUSpeedTime;
	float m_fPUShieldTime;
	float m_fPUHealthTime;
	float m_fPUFreezeTime;

	bool  m_bHandleWiiMoteEvents;
	CParticleEmitter *m_pThrusterLeft, *m_pThrusterRight;

	void Respawn();
	bool HasSpark( CBaseObject *pOther );

	bool m_bElectroChangedThisFrame;
	float m_fTimeForEMP;
	float m_fElectroTime;
	float m_fEMPTime;

	int m_iJellyFrame;

public:

	CPlayerObject( int iPlayer );
	virtual ~CPlayerObject();

	CAnimatedTexture *m_pRadius;

	float m_fPUJellyTime;

	int m_iScore;
	float m_fFreezeTime;

	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual void Update( float fTime );
	virtual void Render();
	virtual void SetPosition( float fX, float fY );
	virtual void SetPosition( Vector pos );
	virtual void CollideWith( CBaseObject *pOther, Vector &pos);

	virtual void OnDie( CBaseObject *m_pKiller );
	virtual inline void ApplyForceFront();

	void TookHealthPowerUp();
	void TookSpeedPowerUp();
	void TookJellyPowerUp();
	void TookShieldPowerUp();
	void ResetStatus();
	void AffectedByFreezePU();

	int GetPlayerNr() { return m_iPlayer; }
	void IncreaseElectro(float timePassed);

	int GetPlayerID() { return m_iPlayer; }

};
