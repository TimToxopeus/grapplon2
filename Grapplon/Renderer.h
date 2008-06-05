#pragma once

#include <vector>

#include <sdl.h>
#include <SDL_opengl.h>
#include "Vector.h"

// Forward declaration
class CAnimatedTexture;
class IActiveObject;
class CPlayerObject;
class CPlanet;

class CRenderer
{
private:
	static CRenderer *m_pInstance;
	CRenderer();
	virtual ~CRenderer();

	std::vector<IActiveObject *> m_vActiveObjects;
	Vector m_vCameraPosition;
	float m_fZoom;

	unsigned int m_iWidth, m_iHeight;
	SDL_Surface *m_pScreen;
	bool m_bSorted;

	void SetOrtho();

public:
	static CRenderer *Instance() { if ( !m_pInstance ) m_pInstance = new CRenderer(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	bool Register( IActiveObject *pObject );
	bool Unregister( IActiveObject *pObject );
	void UnregisterAll();

	bool Init();
	void Shutdown();

	void Render();
	void Update( float fTime );
	void ResortObjects();

	void RenderQuad( SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha );
	void RenderQuad( SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha );

	bool ObjectsInRange( int x, int y, int radius );

	void SetCamera( Vector cameraPosition, float fZoom );
	
	std::vector<CPlayerObject*> players;
	std::vector<CPlanet*> planets;

};
