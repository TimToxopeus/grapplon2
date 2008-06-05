#pragma once

#include "Resource.h"

class CSound : public IResource
{
protected:
	unsigned int m_iSound;
	unsigned int m_iSource;

public:
	CSound( std::string szName, unsigned int iSound ) : IResource( szName ) { m_eType = RT_SOUND; m_iSound = iSound; m_iSource = 0; }
	virtual ~CSound() {}

	unsigned int GetSound() { return m_iSound; }
	bool CreateSource();
	void SetPosition( float fX, float fY, float fZ );
	void Play();
	void Clean();
};
