#include "Texture.h"
#include "Tokenizer.h"

#include "LogManager.h"

CTexture::CTexture( std::string szName, int iWidth, int iHeight, GLuint iGLTexture ) : IResource( szName )
{
	m_eType = RT_TEXTURE;
	m_iGLTexture = iGLTexture;
	size.x = size.y = 0;
	size.w = iWidth; size.h = iHeight;
}

SDL_Rect CTexture::GetSize()
{
	return size;
}
