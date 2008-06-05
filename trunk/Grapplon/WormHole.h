#pragma once
#include "planet.h"
#include <map>
class Vector;
class CPlayerObject;

class CWormHole :
	public CPlanet
{
public:
	CWormHole(PlanetaryData &data, int index);
	~CWormHole();

	CWormHole* twin;

	void ThrowObject(CBaseObject* player);
	void CollideWith(CBaseObject *pOther);
	CBaseObject* m_pToThrow;
	virtual void Update(float fTime);

private:
	Vector m_pExitForce;
	std::map<CBaseObject*, time_t> thrownObjects;

};
