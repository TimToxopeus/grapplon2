#pragma once

#include "Planet.h"
#include "PlanetaryData.h"
#include <time.h>

class CPlayerObject;
struct RespawnArea;

enum AsteroidType { LARGE, SMALL };
enum AsteroidState { ON_FIRE, NORMAL, FROZEN };

class CAsteroid : public CPlanet
{
private:
	float m_fRespawnTime;
	void ReposAtArea(RespawnArea& area);
	void ReposAtOrbit();
	bool m_bTempChangedThisFrame;
	float m_fTempTime;

public:
	CAsteroid(PlanetaryData &data);
	virtual ~CAsteroid();
	
	bool m_bIsGrabable;
	AsteroidType m_eAsteroidType;
	AsteroidState m_eAsteroidState;

	CPlayerObject* m_pThrowingPlayer;
	CPlayerObject* m_pHoldingPlayer;
	time_t m_fThrowTime;
	int m_iMilliSecsInOrbit;
	int m_iWallBounces;
	float m_fBounceToggleTime;
	float m_fTemperatureTime;


	virtual void Update( float fTime );
	virtual void Render();
	void Respawn();
	void Explode();
	void LeaveField();
	void OnPlanetCollide(CBaseObject *pOther);
	void CollideWith(CBaseObject *pOther);
	void Split();
	void IncreaseTemp(float timePassed);

};
