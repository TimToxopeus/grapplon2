#pragma once

#include "Resource.h"
#include <SDL.h>
#include <SDL_OpenGL.h>

class CTexture : public IResource
{
protected:
	GLuint m_iGLTexture;
	SDL_Rect size;

public:
	CTexture( std::string szName, int iWidth, int iHeight, GLuint iGLTexture );
	virtual ~CTexture() {}

	GLuint GetTexture() { return m_iGLTexture; }
	SDL_Rect GetSize();
};
