#include "Universe.h"
#include "Tokenizer.h"
#include "Planet.h"
#include "ODEManager.h"
#include "GameSettings.h"
#include "Sun.h"
#include "Ice.h"
#include "OrdinaryPlanet.h"
#include "Asteroid.h"
#include "PowerUp.h"
#include "FreezePowerUp.h"
#include "HealthPowerUp.h"
#include "SpeedUpPowerUp.h"
#include "GellyPowerUp.h"
#include "ShieldPowerUp.h"

extern std::string itoa2(const int x);

CUniverse::CUniverse()
{
	totalChance = 100;
}

CUniverse::~CUniverse()
{
	CleanUp();
}

void CUniverse::CleanUp()
{
	if ( m_vPlanets.size() != 0 )
	{
		for ( unsigned int i = 0; i < m_vPlanets.size(); i++ )
			delete m_vPlanets[i];
		m_vPlanets.clear();
	}
	m_vUniverse.clear();
}

bool CUniverse::Load( std::string file )
{
	CleanUp();

	CODEManager::Instance()->m_pUniverse = this;

	CTokenizer tokenizer;
	pTokenizer = &tokenizer;

	pFile = fopen( file.c_str(), "rt+" );
	if ( pFile )
	{
		while ( !feof(pFile) )
		{
			std::string in = ReadLine();
			if		( in == "[sun]" ) 		{	ReadPlanet(SUN);	}
			else if ( in == "[ordinary]" )	{	ReadPlanet(ORDINARY);	}
			else if ( in == "[asteroid]" )	{	ReadPlanet(ASTEROID);	}
			else if ( in == "[ice]" )		{	ReadPlanet(ICE);	}
			else if ( in == "[broken]" )	{	ReadPlanet(BROKEN);	}
			else if ( in == "[universe]")	{	ReadUniverse();	}
		}
		fclose( pFile );

		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			if		( m_vUniverse[i].planetType == SUN )		{	m_vPlanets.push_back( new CSun( m_vUniverse[i] ) );				}
			else if ( m_vUniverse[i].planetType == ASTEROID )	{	m_vPlanets.push_back( new CAsteroid( m_vUniverse[i] ) );		}
			else if ( m_vUniverse[i].planetType == ICE )		{	m_vPlanets.push_back( new CIce( m_vUniverse[i] ) );	}
			else if ( m_vUniverse[i].planetType == BROKEN )		{	m_vPlanets.push_back( new COrdinaryPlanet( m_vUniverse[i] ) );	}
			else if ( m_vUniverse[i].planetType == ORDINARY )	{	m_vPlanets.push_back( new COrdinaryPlanet( m_vUniverse[i] ) );	}
		}

		// Calculate positions
		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			PlanetaryData& d = m_vUniverse[i];
			if ( d.planetType == SUN || d.orbit == "") continue;

			CPlanet* orbitted = m_vPlanets[IndexByName( d.orbit )];
			SetUpOrbit(m_vPlanets[i], orbitted);
		}

		// Create objects and set up orbits

		PlacePowerUps();

		return true;
	
	}
	return false;
}

void CUniverse::ReadUniverse()
{
	std::string in = ReadLine();
	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in, " ,;[]:\t" );

		if		( tokens[0]	== "width" )			m_fWidth			= (float) atof(tokens[2].c_str());
		else if ( tokens[0]	== "height" )			m_fHeight			= (float) atof(tokens[2].c_str());
		else if ( tokens[0] == "boundaryforce" )	m_fBoundaryForce	= (float) atof(tokens[2].c_str());
		else if ( tokens[0] == "respawnarea" ) {
			RespawnArea area;
													area.x1				= atoi(tokens[2].c_str());
													area.y1				= atoi(tokens[3].c_str());
													area.x2				= atoi(tokens[4].c_str());
													area.y2				= atoi(tokens[5].c_str());
													area.chance			= atoi(tokens[6].c_str());
			m_vRespawnAreas.push_back(area);
		}
		else if ( tokens[0] == "initspawn1" )		m_iInitSpawnPos1	= Vector((float) atoi(tokens[2].c_str()), (float) atoi(tokens[3].c_str()), 0.0f);
		else if ( tokens[0] == "initspawn2" )		m_iInitSpawnPos2	= Vector((float) atoi(tokens[2].c_str()), (float) atoi(tokens[3].c_str()), 0.0f);
		else if ( tokens[0] == "initspawn3" )		m_iInitSpawnPos3	= Vector((float) atoi(tokens[2].c_str()), (float) atoi(tokens[3].c_str()), 0.0f);
		else if ( tokens[0] == "initspawn4" )		m_iInitSpawnPos4	= Vector((float) atoi(tokens[2].c_str()), (float) atoi(tokens[3].c_str()), 0.0f);
		else if ( tokens[0] == "max_power_up" )		m_iMaxPowerUp		= atoi(tokens[2].c_str());
		else if ( tokens[0] == "pu_freeze" ) {
			PowerUpSetting pu;
													pu.powerUp			= new CFreezePowerUp();
													pu.chance			= atoi(tokens[2].c_str());
			m_lIdlePowerUps.push_back(pu);
		}
		else if ( tokens[0] == "pu_health" ) {
			PowerUpSetting pu;
													pu.powerUp			= new CHealthPowerUp();
													pu.chance			= atoi(tokens[2].c_str());
			m_lIdlePowerUps.push_back(pu);
		}
		else if ( tokens[0] == "pu_speed" ) {
			PowerUpSetting pu;
													pu.powerUp			= new CSpeedUpPowerUp();
													pu.chance			= atoi(tokens[2].c_str());
			m_lIdlePowerUps.push_back(pu);
		}
		else if ( tokens[0] == "pu_gelly" ) {
			PowerUpSetting pu;
													pu.powerUp			= new CGellyPowerUp();
													pu.chance			= atoi(tokens[2].c_str());
			m_lIdlePowerUps.push_back(pu);
		}
		else if ( tokens[0] == "pu_shield" ) {
			PowerUpSetting pu;
													pu.powerUp			= new CShieldPowerUp();
													pu.chance			= atoi(tokens[2].c_str());
			m_lIdlePowerUps.push_back(pu);
		}

		in = ReadLine();
	}


}

void CUniverse::ReadPlanet(ObjectType planType)
{
	PlanetaryData planetData;
	planetData.imageOrbit = "";
	planetData.imageGlow = "";
	planetData.imageFire= "";
	planetData.imageFrozen = "";

	planetData.orbitLength = 0;
	planetData.orbitSpeed = 1000;
	planetData.asteroidcount = 0;
	planetData.orbitJoint = 0;
	planetData.rotation = 0;
	planetData.scale = 1.0f;
	planetData.emitter = "";
	planetData.orbit = "";
	planetData.position = Vector(0, 0, 0);
	planetData.damageMult = -1.0;

	planetData.planetType = planType;

	std::string in = ReadLine();

	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in );

		if		( tokens[0] == "name" )				planetData.name				= tokens[2];
		else if ( tokens[0] == "orbit" )			{
													planetData.orbit			= tokens[2];
													planetData.orbitLength		= atoi(tokens[3].c_str());
													planetData.orbitSpeed		= (float)atof(tokens[4].c_str());
													}
		else if ( tokens[0] == "mass" )				planetData.mass				= atoi(tokens[2].c_str());
		else if ( tokens[0] == "grav" )				planetData.gravconst		= (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "image" )			planetData.image			= tokens[2];
		else if ( tokens[0] == "imageorbit" )		planetData.imageOrbit		= tokens[2];
		else if ( tokens[0] == "imageglow" )		planetData.imageGlow		= tokens[2];
		else if ( tokens[0] == "imagefire" )		planetData.imageFire		= tokens[2];
		else if ( tokens[0] == "imagefrozen" )		planetData.imageFrozen		= tokens[2];
		else if ( tokens[0] == "asteroids" )		planetData.asteroidcount	= atoi(tokens[2].c_str());
		else if ( tokens[0] == "angle" )			planetData.orbitAngle		= atoi(tokens[2].c_str());
		else if ( tokens[0] == "radius" )			planetData.radius			= atoi(tokens[2].c_str());
		else if ( tokens[0] == "damagemult" )		planetData.damageMult		= (float) atof(tokens[2].c_str());
		else if ( tokens[0] == "tempradius" )		planetData.tempradius		= atoi(tokens[2].c_str());
		else if ( tokens[0] == "pos" )	
		{
			planetData.position[0]												= (float)atof(tokens[2].c_str());
			planetData.position[1]												= (float)atof(tokens[3].c_str());
		}
		else if ( tokens[0] == "rotation" )	planetData.rotation					= atoi(tokens[2].c_str());
		else if ( tokens[0] == "scale" )	planetData.scale					= (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "emitter" )	
		{
			planetData.emitter													= tokens[2];
			planetData.bNear													= (tokens[3] == "near" ? true : false);
			planetData.offsetForward											= atoi(tokens[4].c_str());
			planetData.offsetRight												= atoi(tokens[5].c_str());
		}

		in = ReadLine();
	}

	m_vUniverse.push_back( planetData );
}

int CUniverse::IndexByName( std::string name )
{
	for ( unsigned int i = 0; i < m_vUniverse.size(); i++ )
	{
		if ( m_vUniverse[i].name == name )
			return i;
	}
	return -1;
}

// object1 orbits around object2
void CUniverse::SetUpOrbit( CPlanet* orbitter, CPlanet* orbitted )
{
	orbitter->m_pOrbitOwner = orbitted;
	orbitter->SetPosition( orbitted->GetPosition() + Vector::FromAngleLength(orbitter->m_fOrbitAngle, orbitter->m_fOrbitLength) );

	Vector hingePos = orbitted->GetPosition();
	// Create joint
	orbitter->orbitJoint = CODEManager::Instance()->createHingeJoint("Orbit Joint");
	dJointAttach( orbitter->orbitJoint, orbitted->GetBody(), orbitter->GetBody() );
	dJointSetHingeAnchor(orbitter->orbitJoint, hingePos[0], hingePos[1], 0.0f);
	orbitter->m_bIsInOrbit = true;
}

std::string CUniverse::ReadLine()
{
	if ( !pFile || feof(pFile) )
		return "";

	char input[1024];
	fgets( input, 1024, pFile );
	if ( feof(pFile) )
		return "";
	int len = strlen(input);
	if ( len > 0 )
		input[len - 1] = 0; // Cut off the \n
	return std::string(input);
}

void CUniverse::Update( float fTime )
{
}

void CUniverse::RemovePowerUp(CPowerUp* powerup)
{

	std::list<PowerUpSetting>::iterator it = m_lPlacedPowerUps.begin();
	std::list<PowerUpSetting>::iterator delIt;

	PowerUpSetting curPU;

	for(it; it != m_lPlacedPowerUps.end(); it++)
	{
		curPU = *it;
		if(curPU.powerUp == powerup){
			delIt = it;	
			break;
		}
	}
	powerup->SetPosition(-10000, -10000);
	m_lPlacedPowerUps.erase(delIt);
	m_lIdlePowerUps.push_back(curPU);
	totalChance += curPU.chance;

	PlacePowerUps();

}


void CUniverse::PlacePowerUps()
{
	
	while(this->m_lPlacedPowerUps.size() < (unsigned int) this->m_iMaxPowerUp)
	{
		int stochast = rand()%totalChance;
		int sum = 0;
		CODEManager* ode = CODEManager::Instance();
		PowerUpSetting curPU;

		std::list<PowerUpSetting>::iterator it = m_lIdlePowerUps.begin();
		std::list<PowerUpSetting>::iterator delIt = m_lIdlePowerUps.end();


		for(it; it != m_lIdlePowerUps.end(); it++)
		{
			curPU = (*it);
			sum += curPU.chance;
			if(stochast < sum)
			{ 
				curPU.powerUp->Respawn();
				m_lPlacedPowerUps.push_back(curPU);
				totalChance -= curPU.chance;
				delIt = it;
				break;	
			}
		}

		if(delIt != m_lIdlePowerUps.end()) m_lIdlePowerUps.erase(delIt);

	}







}