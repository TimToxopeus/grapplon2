#include "SoundManager.h"
#include "LogManager.h"

#include "al.h"
#include "alut.h"

#include "OggStream.h"

#pragma warning(disable:4996)

CSoundManager *CSoundManager::m_pInstance = 0;

CSoundManager::CSoundManager()
{
	m_iSampleSet = 0;
	m_iSoundCount = 0;
	m_pMusic = NULL;
}

CSoundManager::~CSoundManager()
{
}

bool CSoundManager::Init()
{
	CLogManager::Instance()->LogMessage("Initializing sound manager.");

	ALenum error;
	alutInit(0, NULL);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage("Error initializing sound: alutInit : " + GetSoundError(error));
		return false;
	}

	alGenBuffers(NUM_SOUNDS, m_iBuffers);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage("Error initializing sound: alGenBuffers : " + GetSoundError(error) );
		return false;
	}

	m_iSampleSet = 0;
	m_iSoundCount = 0;
	return true;
}

void CSoundManager::Shutdown()
{
	CLogManager::Instance()->LogMessage("Terminating sound manager.");

	if ( m_pMusic )
	{
		delete m_pMusic;
		m_pMusic = NULL;
	}

	alDeleteBuffers(NUM_SOUNDS, m_iBuffers);
	alutExit();

	m_iSampleSet = 0;
	m_iSoundCount = 0;
}

unsigned int CSoundManager::LoadSound(std::string name)
{
	if ( name.find(".wav") != std::string::npos )
		return LoadSoundWAV( name );
	if ( name.find(".ogg") != std::string::npos )
		return LoadSoundOGG( name );
	return 0;
}

unsigned int CSoundManager::LoadSoundWAV(std::string name)
{
	ALenum     format, error;
	ALsizei    size;
	ALsizei    freq;
	ALboolean  loop;
	ALvoid*    data;

	alutLoadWAVFile((ALbyte *)name.c_str(), &format, &data, &size, &freq, &loop);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage("Error loading sound : " + GetSoundError(error) );
		return 0;
	}

	alBufferData(m_iBuffers[m_iSoundCount],format,data,size,freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage("Error loading sound : " + GetSoundError(error) );
		return 0;
	}

	alutUnloadWAV(format,data,size,freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage("Error loading sound : " + GetSoundError(error) );
		return 0;
	}

	return m_iBuffers[m_iSoundCount++];
}

unsigned int CSoundManager::LoadSoundOGG( std::string name )
{
	if ( m_pMusic )
	{
		delete m_pMusic;
	}

	m_pMusic = new COggStream();
	m_pMusic->open( name );
	m_pMusic->playback();
	return 0;
}

std::string CSoundManager::GetSoundError( int error )
{
	switch ( error )
	{
	default: case AL_NO_ERROR: return "no error.";
	case AL_INVALID_NAME: return "invalid name";
	case AL_INVALID_ENUM: return "invalid enum";
	case AL_INVALID_VALUE: return "invalid value";
	case AL_INVALID_OPERATION: return "invalid operation";
	case AL_OUT_OF_MEMORY: return "out of memory";
	}
}

void CSoundManager::Update( float fTime )
{
	if ( m_pMusic )
	{
		m_pMusic->update();
		if ( !m_pMusic->playing() )
			m_pMusic->playback();
	}
}
