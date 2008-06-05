#include "Renderer.h"
#include "Texture.h"
#include "ActiveObject.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include <algorithm>

#include "Vector.h"
#include "BaseObject.h"

#include "AnimatedTexture.h"
#include <sstream>

CRenderer *CRenderer::m_pInstance = 0;

struct ActiveObjectSort
{
	bool operator()(IActiveObject *&lhs, IActiveObject *&rhs)
	{
		return lhs->GetDepth() < rhs->GetDepth();
	}
};

CRenderer::CRenderer()
{
	m_bSorted = true;
}

CRenderer::~CRenderer()
{
}

bool CRenderer::Register(IActiveObject *pObject)
{
	CLogManager::Instance()->LogMessage("Renderer registring new object.");

	// Non-existant objects need not apply
	if ( !pObject )
		return false;

	// First make sure the object isn't already registered
	for ( unsigned int a = 0; a<m_vActiveObjects.size(); a++ )
	{
		if ( m_vActiveObjects[a] == pObject )
			return true; // Already on the list, this object is registered.
	}

	// Else add it to the list
	m_vActiveObjects.push_back( pObject );

	if(pObject->getType() == SHIP) players.push_back( (CPlayerObject*) pObject );
	if(pObject->getType() == ASTEROID) planets.push_back( (CPlanet*) pObject );

	m_bSorted = false;

	return true;
}

bool CRenderer::Unregister(IActiveObject *pObject)
{
	// Non-existant objects do not exist
	if ( !pObject )
		return false;

	// Check all the objects
	for ( unsigned int a = 0; a<m_vActiveObjects.size(); a++ )
	{
		if ( m_vActiveObjects[a] == pObject )
		{
			// Found it, remove it from the list.
			m_vActiveObjects.erase( m_vActiveObjects.begin() + a );
			return true;
		}
	}

	// This object is not registered.
	return false;
}

void CRenderer::UnregisterAll()
{
	m_vActiveObjects.clear();
}

bool CRenderer::Init()
{
	CLogManager::Instance()->LogMessage("Initializing renderer.");

	m_iWidth = 1024;
	m_iHeight = 768;

	// Initialize renderer
	_putenv("SDL_VIDEO_CENTERED=center" );// Centers the screen
	const SDL_VideoInfo* info = NULL;

	// Initialize SDL VIDEO
	if (!(info = SDL_GetVideoInfo()))
		return false;

//	SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
	SDL_WM_SetCaption("Grapplon", NULL);
	int flags = SDL_OPENGL;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 4);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	CLogManager::Instance()->LogMessage("Creating window.");
	m_pScreen = SDL_SetVideoMode(m_iWidth, m_iHeight, 32, flags);
	if(m_pScreen == 0)
	{
		SDL_Quit();
		return false;
	}

	SDL_ShowCursor( SDL_DISABLE );

	m_vCameraPosition = Vector( 0, 0, 0 );
	m_fZoom = 2.0f;
	SetOrtho();

	CLogManager::Instance()->LogMessage("Initialization renderer succesful.");
	return true;
}

void CRenderer::Shutdown()
{
	CLogManager::Instance()->LogMessage("Shutting down renderer.");
	// Shutdown renderer
	UnregisterAll();
	SDL_Quit();
}

void CRenderer::Render()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetOrtho();

	if ( !m_bSorted )
		ResortObjects();

	// Loop through all objects
	for ( unsigned int a = 0; a<m_vActiveObjects.size(); a++ )
	{
		m_vActiveObjects[a]->Render();
	}
	SDL_GL_SwapBuffers();
}

void CRenderer::Update(float fTime)
{
	// Loop through all objects
	int size = m_vActiveObjects.size();
	int a = size - 1;
	while ( a >= 0 )
	{
		m_vActiveObjects[a]->Update( fTime );
		if ( m_vActiveObjects[a]->ShouldBeDeleted() )
		{
			delete m_vActiveObjects[a];
			m_vActiveObjects.erase( m_vActiveObjects.begin() + a );
		}

		a--;
	}
}

void CRenderer::RenderQuad( SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha )
{
	SDL_Color c; c.r = c.g = c.b = 255;
	RenderQuad( target, pTexture, fAngle, c, fAlpha );
	return;
}

void CRenderer::RenderQuad( SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha )
{
	if ( pTexture )
	{
		glBindTexture( GL_TEXTURE_2D, pTexture->GetTexture() );
		glEnable( GL_TEXTURE_2D );
	}
	else
	{
		glDisable( GL_TEXTURE_2D );
	}

	int w = target.w / 2;
	int h = target.h / 2;

	glTranslatef( (float)(target.x + w), (float)(target.y + h), 0.0f );
	glRotatef(fAngle, 0, 0, 1);

	Coords texture;
	if ( pTexture )
	{
		texture = pTexture->GetTextureCoords();
	}
	else
	{
		texture.x = texture.y = 0;
		texture.w = texture.h = 1;
	}

	// Draw the quad
	glBegin(GL_QUADS);
		glColor4f((float)colour.r / 255.0f, (float)colour.g / 255.0f, (float)colour.b / 255.0f, fAlpha);

		// Top left corner
		if ( pTexture )
			glTexCoord2f( texture.x, texture.y );
		glVertex3f((float)-w, (float)-h, 0.0f);

		// Bottom left corner
		if ( pTexture )
			glTexCoord2f( texture.x, texture.y + texture.h );
		glVertex3f((float)-w, (float)h, 0.0f);

		// Bottom right corner
		if ( pTexture )
			glTexCoord2f( texture.x + texture.w, texture.y + texture.h );
		glVertex3f((float)w, (float)h, 0.0f);

		// Top right corner
		if ( pTexture )
			glTexCoord2f( texture.x + texture.w, texture.y );
		glVertex3f((float)w, (float)-h, 0.0f);
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnd();

	glRotatef(-fAngle, 0, 0, 1);
	glTranslatef( (float)-(target.x + w), (float)-(target.y + h), 0.0f );
}

void CRenderer::SetOrtho()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// SET ZOOM HERE
//	glOrtho(0, 1024, 768, 0, 2.0f, -5.0f );
//	glOrtho(-1024, 1024, 768, -768, 2.0f, -5.0f );

	float left, right, down, up;
	left = (float)((int)m_vCameraPosition[0] - (int)(((float)m_iWidth / 2.0f) * m_fZoom));
	right = (float)((int)m_vCameraPosition[0] + (int)(((float)m_iWidth / 2.0f) * m_fZoom));
	down = (float)((int)m_vCameraPosition[1] + (int)(((float)m_iHeight / 2.0f) * m_fZoom));
	up = (float)((int)m_vCameraPosition[1] - (int)(((float)m_iHeight / 2.0f) * m_fZoom));

	glOrtho(left, right, down, up, 2.0f, -5.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CRenderer::ResortObjects()
{
	int i = 0;
	// Sort depthbuffer
	std::sort(m_vActiveObjects.begin(), m_vActiveObjects.end(), ActiveObjectSort());

	m_bSorted = true;
	i = 0;
}

bool CRenderer::ObjectsInRange( int x, int y, int radius )
{
	Vector pos = Vector( (float)x, (float)y, 0 );
	for ( unsigned int i = 0; i<m_vActiveObjects.size(); i++ )
	{
		IActiveObject *act = m_vActiveObjects[i];
		CBaseObject *obj = NULL;

		if ( act->getType() != STATE && act->getType() != CHAINLINK && act->getType() != PARTICLESYSTEM && act->getType() != HUD && act->getType() != UNSET )
		{
			obj = (CBaseObject *)act;
			Vector dist = obj->GetPosition() - pos;
			if ( dist.Length() < radius + obj->GetPhysicsData()->m_fRadius )
				return true;
		}
	}
	return false;
}

void CRenderer::SetCamera( Vector cameraPosition, float fZoom )
{
	m_vCameraPosition = cameraPosition;
	m_fZoom = fZoom;
	SetOrtho();
}
