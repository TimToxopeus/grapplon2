#include "ParticleSystemManager.h"
#include "Tokenizer.h"
#include "LogManager.h"
#include <map>
#include "GameSettings.h"

CParticleSystemManager *CParticleSystemManager::m_pInstanceNear = 0;
CParticleSystemManager *CParticleSystemManager::m_pInstanceFar = 0;

CParticleSystemManager::CParticleSystemManager(float depth)
{
	CLogManager::Instance()->LogMessage( "Initializing Particle System manager." );
	m_eType = PARTICLESYSTEM;

	SetDepth( depth );
}

CParticleSystemManager::~CParticleSystemManager()
{
	CLogManager::Instance()->LogMessage( "Terminating Particle System manager." );
}

void CParticleSystemManager::Update(float fTime)
{
	std::vector<CParticleEmitter *> vDelete;

	for ( unsigned int i = 0; i<m_vEmitters.size(); i++ )
	{
		m_vEmitters[i]->Update( fTime );
		if ( !m_vEmitters[i]->IsAlive() )
		{
			vDelete.push_back( m_vEmitters[i] );
			m_vEmitters.erase( m_vEmitters.begin() + i );
		}
	}

	for ( unsigned int i = 0; i<vDelete.size(); i++ )
	{
		delete vDelete[i];
	}
	vDelete.clear();
}

void CParticleSystemManager::Render()
{
	for ( unsigned int i = 0; i<m_vEmitters.size(); i++ )
	{
		m_vEmitters[i]->Render();
	}
}

std::string CParticleSystemManager::ReadLine( FILE *pFile )
{
	if ( !pFile || feof(pFile) )
		return "<<<EOF>>>";

	char input[1024];
	fgets( input, 1024, pFile );
	if ( feof(pFile) )
		return "";
	int len = strlen(input);
	if ( len > 0 )
		input[len - 1] = 0; // Cut off the \n
	return std::string(input);
}

CParticleEmitter *CParticleSystemManager::LoadEmitter( std::string szEmitterScript )
{
	if ( !SETS->PARTICLES_ON )
		return NULL;

	FILE *pFile = fopen( szEmitterScript.c_str(), "rt" );
	if ( !pFile )
		return NULL;

	std::string in = ReadLine( pFile );
	while ( in != "<<<EOF>>>" && in != "[emitter]" )
		in = ReadLine( pFile );

	EmitterType eType = NONE;
	float fTypeParameter = 0.0f;
	unsigned int iMaxParticles = 50;
	unsigned int iLifespan = 6000;
	unsigned int iSpawnrate = 10;
	float fRadius = 0.0f;

	CTokenizer tokenizer;
	std::vector<std::string> tokens;
	std::map<std::string, int> vParticles;
	std::map<std::string,int>::iterator particleIt;

	while ( in != "<<<EOF>>>" && in != "" )
	{
		tokens = tokenizer.GetTokens( in, " ,;:\"" );
		if ( tokens.size() > 0 )
		{
			if ( tokens[0] == "direction" )
			{
				if ( tokens[2] == "arc" )
				{
					eType = ARC;
					fTypeParameter = (float)atof(tokens[3].c_str());
				}
				else if ( tokens[2] == "line" )
				{
					eType = LINE;
					fTypeParameter = (float)atof(tokens[3].c_str());
				}
				else
				{
					eType = NONE;
					fTypeParameter = 0.0f;
				}
			}
			else if ( tokens[0] == "maxparticles" )
				iMaxParticles = atoi(tokens[2].c_str());
			else if ( tokens[0] == "lifespan" )
				iLifespan = atoi(tokens[2].c_str());
			else if ( tokens[0] == "spawnrate" )
				iSpawnrate = atoi(tokens[2].c_str());
			else if ( tokens[0] == "radius" )
				fRadius = (float)atof(tokens[2].c_str());
			else if ( tokens[0] == "particles" )
			{
				int particles = (tokens.size() - 2) / 2;
				for ( int i = 0; i<particles * 2; i+=2 )
				{
					std::string name = tokens[i + 2];
					int chance = atoi(tokens[i + 3].c_str());
					vParticles[name] = chance;
				}
			}
		}
		in = ReadLine( pFile );
	}
	fclose( pFile );

	CParticleEmitter *pEmitter = new CParticleEmitter( eType, fTypeParameter, iMaxParticles, iLifespan, iSpawnrate, fRadius );

	for ( particleIt = vParticles.begin(); particleIt != vParticles.end(); particleIt++ )
	{
		std::string name = (*particleIt).first;
		int chance = (*particleIt).second;

		CParticle *pParticle = ReadParticle( szEmitterScript, name, pEmitter );
		pEmitter->AddToFactory( pParticle, chance );
	}

	m_vEmitters.push_back( pEmitter );
	return pEmitter;
}

CParticle *CParticleSystemManager::ReadParticle(std::string szEmitterScript, std::string szParticleName, CParticleEmitter *pEmitter)
{
	FILE *pFile = fopen( szEmitterScript.c_str(), "rt" );
	if ( !pFile )
		return NULL;

	CTokenizer tokenizer;
	std::vector<std::string> tokens;

	std::string in = ReadLine( pFile );
	bool bInparticle = false;
	CParticle *pParticle = NULL;

	std::string szSpriteScript = "";
	Vector colour1, colour2;
	unsigned int iLifespan = 1500;
	unsigned int iSize = 2;
	std::vector<std::string> vBehaviours;

	while ( in != "<<<EOF>>>" )
	{
		tokens = tokenizer.GetTokens( in, " ,;:\"" );
		if ( tokens.size() == 0 )
		{
			in = ReadLine( pFile );
			bInparticle = false;
			continue;
		}

		if ( tokens[0] == "[emitter]" )
			bInparticle = false;
		else if ( tokens[0] == "[particle]" )
			bInparticle = true;
		else if ( tokens[0] == "[behaviour]" )
			bInparticle = false;

		if ( bInparticle )
		{
			if ( tokens[0] == "name" && tokens[2] == szParticleName )
			{
				// Read until the next blank line
				in = ReadLine( pFile );
				while ( in != "" && in != "<<<EOF>>>" )
				{
					tokens = tokenizer.GetTokens(in);
					if ( tokens[0] == "colour1" )
						colour2 = colour1 = Vector((float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()), (float)atof(tokens[4].c_str()));
					else if ( tokens[0] == "colour2" )
						colour2 = Vector((float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()), (float)atof(tokens[4].c_str()));
					else if ( tokens[0] == "lifespan" )
						iLifespan = atoi(tokens[2].c_str());
					else if ( tokens[0] == "size" )
						iSize = atoi(tokens[2].c_str());
					else if ( tokens[0] == "sprite" )
						szSpriteScript = tokens[2];
					else if ( tokens[0] == "behave" )
					{
						for ( unsigned int i = 2; i<tokens.size(); i++ )
							vBehaviours.push_back( tokens[i] );
					}
					in = ReadLine( pFile );
				}
				pParticle = new CParticle(pEmitter, szSpriteScript);
				pParticle->m_colour1 = colour1;
				pParticle->m_colour2 = colour2;
				pParticle->m_iLifespan = iLifespan;
				pParticle->m_szName = szParticleName;
				pParticle->m_iSize = iSize;
				fclose( pFile );
				break;
			}
		}

		in = ReadLine( pFile );
	}
	fclose( pFile );

	if ( pParticle )
	{
		for ( unsigned int i = 0; i<vBehaviours.size(); i++ )
		{
			CParticleBehaviour behaviour = pEmitter->GetBehaviour(vBehaviours[i]);
			if ( behaviour.m_szName == "NULL" )
			{
				behaviour = ReadParticleBehaviour( szEmitterScript, vBehaviours[i] );
				pEmitter->AddBehaviour( vBehaviours[i], behaviour );
			}
			pParticle->m_vBehaviourStyles.push_back( behaviour );
		}
	}

	return pParticle;
}

CParticleBehaviour CParticleSystemManager::ReadParticleBehaviour( std::string szEmitterScript, std::string szParticleBehaviourName )
{
	FILE *pFile = fopen( szEmitterScript.c_str(), "rt" );
	if ( !pFile )
		return CParticleBehaviour( "NULL", 0.0f, 0.0f );

	CTokenizer tokenizer;
	std::vector<std::string> tokens;

	std::string in = ReadLine( pFile );
	bool bInBehaviour = false;

	float fEffect, fVelocity;

	while ( in != "<<<EOF>>>" )
	{
		tokens = tokenizer.GetTokens( in, " ,;:\"" );
		if ( tokens.size() == 0 )
		{
			in = ReadLine( pFile );
			bInBehaviour = false;
			continue;
		}

		if ( tokens[0] == "[emitter]" )
			bInBehaviour = false;
		else if ( tokens[0] == "[particle]" )
			bInBehaviour = false;
		else if ( tokens[0] == "[behaviour]" )
			bInBehaviour = true;

		if ( bInBehaviour )
		{
			if ( tokens[0] == "name" && tokens[2] == szParticleBehaviourName )
			{
				// Read until the next blank line
				in = ReadLine( pFile );
				while ( in != "" && in != "<<<EOF>>>" )
				{
					tokens = tokenizer.GetTokens(in);
					if ( tokens[0] == "velocity" )
						fVelocity = (float)atof( tokens[2].c_str() );
					else if ( tokens[0] == "move" )
						fEffect = (float)atof( tokens[2].c_str() );
					in = ReadLine( pFile );
				}
				fclose( pFile );
				return CParticleBehaviour( szParticleBehaviourName, fEffect, fVelocity );
			}
		}

		in = ReadLine( pFile );
	}
	fclose( pFile );

	return CParticleBehaviour( "NULL", 0.0f, 0.0f );
}
