#pragma once

#include "Texture.h"
#include <vector>

// Texture coords
struct Coords
{
	float x, y, w, h;
};

// Animation entry
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

	unsigned int m_iCurAnim;
	unsigned int m_iCurFrame;
	std::vector<Animation> m_vAnimations;
	float m_fTimeFrameChange, m_fDesiredFramesPerSecond;

	FILE *pFile;
	bool m_bHeaderRead;
	void ReadHeader();
	void ReadAnimation( std::string anim );
	std::string ReadLine();

	float m_fOverrideHeight; // HUD Override value
	bool m_bLoaded, m_bValid; // Error checking flags

public:
	CAnimatedTexture( std::string name );

	void LoadTextureData();
	void UpdateFrame( float fTime );

	SDL_Rect GetOriginalSize(){ return m_vAnimations[m_iCurAnim].m_pTexture->GetSize(); };

	Coords GetTextureCoords();
	GLuint GetTexture() { return m_vAnimations[m_iCurAnim].m_pTexture->GetTexture(); }
	SDL_Rect GetSize();
	void SetFramerate( unsigned int FramesPerSecond );
	void Scale( float fScale );
	void SetAnimation( int iAnimation );
	void SetFrame( unsigned int iFrame );
	unsigned int GetFrame() { return m_iCurFrame; }

	void OverrideHeight( float fHeight ) { m_fOverrideHeight = fHeight; } // HUD ONLY!!
	bool IsFinished() { return (m_vAnimations[m_iCurAnim].m_iFrames == m_iCurFrame + 1); } // Finished animation playback?
	int GetAnimCount() { return m_vAnimations.size(); }
};
