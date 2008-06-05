#pragma once

#include <string>

#define NUM_SOUNDS 100

// Forward declarations
class COggStream;

class CSoundManager
{
private:
	static CSoundManager *m_pInstance;
	CSoundManager();
	virtual ~CSoundManager();

	unsigned int m_iSoundCount;
	unsigned int m_iSampleSet;
	unsigned int m_iBuffers[NUM_SOUNDS];

	std::string GetSoundError( int error );

	unsigned int LoadSoundWAV( std::string name );
	unsigned int LoadSoundOGG( std::string name );

	COggStream *m_pMusic;

public:
	static CSoundManager *Instance() { if ( !m_pInstance ) m_pInstance = new CSoundManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	bool Init();
	void Shutdown();

	void Update( float fTime );

	unsigned int LoadSound( std::string name );
};
