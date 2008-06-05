#pragma once

#include "Texture.h"
#include <vector>

struct Coords
{
	float x, y, w, h;
};

struct Animation
{
	std::string m_szName;
	CTexture *m_pTexture;
	unsigned int m_iRows;
	unsigned int m_iFrames;
	unsigned int m_iFramesPerRow;
	int m_iSpeed;
	float m_fXStep, m_fYStep;
	bool loop;
};

class CAnimatedTexture
{
private:
	std::string m_szScriptFile;
	SDL_Rect size;

	unsigned char m_iCurAnim;
	unsigned char m_iCurFrame;
	std::vector<Animation> m_vAnimations;
	float m_fTimeFrameChange, m_fDesiredFramesPerSecond;

	FILE *pFile;
	bool m_bHeaderRead;
	void ReadHeader();
	void ReadAnimation( std::string anim );
	std::string ReadLine();

	float m_fOverrideHeight;

public:
	CAnimatedTexture( std::string name );

	void LoadTextureData();
	void UpdateFrame( float fTime );

	Coords GetTextureCoords();
	GLuint GetTexture() { return m_vAnimations[m_iCurAnim].m_pTexture->GetTexture(); }
	SDL_Rect GetSize();
	void SetFramerate( unsigned int FramesPerSecond );
	void Scale( float fScale );
	void SetAnimation( int iAnimation );
	void SetFrame( unsigned int iFrame );

	void OverrideHeight( float fHeight ) { m_fOverrideHeight = fHeight; } // HUD ONLY!!
};
