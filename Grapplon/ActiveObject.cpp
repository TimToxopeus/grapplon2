#include "ActiveObject.h"
#include "Renderer.h"
#include "Texture.h"

IActiveObject::IActiveObject()
{
	// Automatically add the object to the renderer.
	CRenderer::Instance()->Register(this);

	m_fDepth = 0.0f; // Default depth
	m_fAlpha = 1.0f;
	m_fScale = 1.0f;
	m_eType = UNSET;

	m_bDeleteMe = false;
}

IActiveObject::~IActiveObject()
{
	// Automatically remove the object from the renderer.
	CRenderer::Instance()->Unregister(this);
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha)
{
	SDL_Color white;
	white.r = white.g = white.b = 255;
	if ( fAlpha == -1 )
		RenderQuad( target, pTexture, fAngle, white, m_fAlpha );
	else
		RenderQuad( target, pTexture, fAngle, white, fAlpha );
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha)
{
//	target.w = (int)((float)target.w * m_fScale);
//	target.h = (int)((float)target.h * m_fScale);

	if ( fAlpha == -1 )
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, colour, m_fAlpha );
	else
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, colour, fAlpha );
}

void IActiveObject::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
	CRenderer::Instance()->ResortObjects();
}
