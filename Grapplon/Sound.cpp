#include "Sound.h"

#include "al.h"

void CSound::SetPosition( float fX, float fY, float fZ )
{
	float f[3]; f[0] = fX; f[1] = fY; f[2] = fZ;
	alSourcefv(m_iSource, AL_POSITION, f);
}

bool CSound::CreateSource()
{
	if ( m_iSource != 0 )
		Clean();

	alGenSources(1, &m_iSource);
	if (alGetError() != AL_NO_ERROR)
		return false;

	alSourcei(m_iSource, AL_BUFFER, m_iSound);
	if (alGetError() != AL_NO_ERROR)
		return false;

	return true;
}

void CSound::Play()
{
	if ( m_iSource == 0 )
		CreateSource();
    ALenum state;
    alGetSourcei(m_iSource, AL_SOURCE_STATE, &state);

	if ( !(state == AL_PLAYING) )
		alSourcePlay( m_iSource );
}

void CSound::Clean()
{
	alDeleteSources( 1, &m_iSource );
	m_iSource = 0;
}
