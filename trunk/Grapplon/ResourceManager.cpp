#include <sdl.h>
#include <sdl_image.h>

#include "ResourceManager.h"
#include "Resource.h"
#include "Texture.h"
#include "Sound.h"
using namespace std;

#include "LogManager.h"
#include "SoundManager.h"

CResourceManager *CResourceManager::m_pInstance = 0;

CResourceManager::CResourceManager()
{
	CLogManager::Instance()->LogMessage("Initializing resource manager.");
}

CResourceManager::~CResourceManager()
{
	CLogManager::Instance()->LogMessage("Terminating resource manager.");
	CLogManager::Instance()->LogMessage("Cleaning up all resources.");
	// Unload all resources
	for ( unsigned int a = 0; a<m_vResources.size(); a++ )
	{
		IResource *pResource = m_vResources[a];
		switch ( pResource->GetType() )
		{
		case RT_TEXTURE: {
				CTexture *pTexture = (CTexture *)pResource;
				GLuint texture = pTexture->GetTexture();
				glDeleteTextures( 1, &texture );
				delete pTexture;
				break;
			}
		case RT_SOUND: {
				CSound *pSound = (CSound *)pResource;
				pSound->Clean();
				delete pSound;
				break;
			}
		}
	}
	m_vResources.clear();
}

IResource *CResourceManager::GetResource( std::string name, RTYPE resourceType )
{
	// Loop through all the loaded resources
	for ( unsigned int a = 0; a<m_vResources.size(); a++ )
	{
		if ( m_vResources[a]->GetType() == resourceType )
		{
			if ( m_vResources[a]->GetName() == name )
			{
				return m_vResources[a];
			}
		}
	}

	// Load the resource
	IResource *pResource = NULL;
	switch ( resourceType )
	{
	case RT_TEXTURE:
		pResource = LoadTexture( name );
		if ( pResource != NULL )
			m_vResources.push_back( pResource );
		break;
	case RT_SOUND:
		pResource = LoadSound( name );
		if ( pResource != NULL )
			m_vResources.push_back( pResource );
		break;
	default:
		break;
	}

	return pResource;
}

IResource *CResourceManager::LoadTexture( std::string name )
{
	CLogManager::Instance()->LogMessage("Attempting to load texture: " + name);

	SDL_Surface *pSurface = NULL;
	if ( name.find(".bmp") != string::npos )
		pSurface = SDL_LoadBMP( name.c_str() );
	if ( name.find(".png") != string::npos )
		pSurface = IMG_Load( name.c_str() );
	GLuint iGLTexture;
	GLenum texture_format;
	GLint nOfColors;

	if ( pSurface )
	{
		// Check that the image's width is a power of 2
/*		if ( (pSurface->w & (pSurface->w - 1)) != 0 )
		{
//			printf("warning: image.bmp's width is not a power of 2\n");
			CLogManager::Instance()->LogMessage("Warning: " + name + "'s width is not a power of 2!");
			SDL_FreeSurface( pSurface );
			return NULL;
		}*/

		// Also check if the height is a power of 2
/*		if ( (pSurface->h & (pSurface->h - 1)) != 0 )
		{
//			printf("warning: image.bmp's height is not a power of 2\n");
			CLogManager::Instance()->LogMessage("Warning: " + name + "'s height is not a power of 2!");
			SDL_FreeSurface( pSurface );
			return NULL;
		}*/
 
		// get the number of channels in the SDL surface
		nOfColors = pSurface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
			if (pSurface->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		}
		else if (nOfColors == 3)     // no alpha channel
		{
			if (pSurface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		else
		{
			//printf("warning: the image is not truecolor..  this will probably break\n");
			// this error should not go unhandled
			CLogManager::Instance()->LogMessage("Warning: The image is not truecolor..  this will probably break!");
			SDL_FreeSurface( pSurface );
			return NULL;
		}
        
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &iGLTexture );
 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, iGLTexture );
 
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
 
		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, pSurface->w, pSurface->h, 0,
						texture_format, GL_UNSIGNED_BYTE, pSurface->pixels );

		GLenum error = glGetError();

		CTexture *pTexture = new CTexture( name, pSurface->w, pSurface->h, iGLTexture );
		SDL_FreeSurface( pSurface );
		
		CLogManager::Instance()->LogMessage("Texture succesfully loaded.");
		return pTexture;
	}
	return NULL;
}

IResource *CResourceManager::LoadSound( std::string name )
{
	CLogManager::Instance()->LogMessage("Attempting to load sound: " + name);

	unsigned int sound = CSoundManager::Instance()->LoadSound( name );
	CSound *pSound = new CSound( name, sound );	

	CLogManager::Instance()->LogMessage("Sound succesfully loaded.");
	return pSound;
}
