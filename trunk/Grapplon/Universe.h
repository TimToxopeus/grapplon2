#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <list>

#include "ActiveObject.h"
#include "PlanetaryData.h"

class CTokenizer;
class CPlanet;
class Vector;
class CPowerUp;

struct RespawnArea
{
	int x1;
	int y1;
	int x2;
	int y2;
	int chance;
};

struct PowerUpSetting
{
	CPowerUp* powerUp;
	int chance;
};

class CUniverse
{
private:
	void CleanUp();
	void ReadPlanet(ObjectType planetType);
	void ReadUniverse();
	int  totalChance;

	FILE *pFile;
	CTokenizer *pTokenizer;
	std::vector<PlanetaryData> m_vUniverse;

	std::vector<CPlanet *> m_vPlanets;

	void SetUpOrbit( CPlanet* orbittee, CPlanet* orbitted );
	int IndexByName( std::string name );
	std::string ReadLine();
	void PlacePowerUps();

public:
	CUniverse();
	~CUniverse();

	Vector m_iInitSpawnPos1;
	Vector m_iInitSpawnPos2;
	Vector m_iInitSpawnPos3;
	Vector m_iInitSpawnPos4;

	float m_fWidth;
	float m_fHeight;
	float m_fBoundaryForce;
	int	  m_iMaxPowerUp;
	
	std::vector<RespawnArea> m_vRespawnAreas;
	std::list<PowerUpSetting> m_lIdlePowerUps;
	std::list<PowerUpSetting> m_lPlacedPowerUps;

	bool Load( std::string file );
	void Update( float fTime );

	void RemovePowerUp(CPowerUp* powerup);

};
