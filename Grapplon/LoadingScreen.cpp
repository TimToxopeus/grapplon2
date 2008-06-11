#include "LoadingScreen.h"
#include "Renderer.h"
#include "LogManager.h"

CLoadingScreen *CLoadingScreen::m_pInstance = NULL;

extern HDC currentDC;
extern HGLRC currentContext;

int LoadingScreenThread(void *data)
{
	CLoadingScreen *pLoading = CLoadingScreen::Instance();
	Uint32 time, lastUpdate;
	time = lastUpdate = SDL_GetTicks();

	if ( !wglMakeCurrent(currentDC, currentContext) )
		CLogManager::Instance()->LogMessage( "Error binding renderer to loading screen thread" );
	pLoading->SetContext( 2 );

	while ( pLoading->IsRunning() )
	{
		pLoading->CheckContext();
		time = SDL_GetTicks();
		float timeSinceLastUpdate = (float)(time - lastUpdate) / 1000.0f;
		pLoading->Render( timeSinceLastUpdate );
 		SDL_Delay( 5 );
		lastUpdate = time;
	}

	if ( !wglMakeCurrent(0, 0) )
		CLogManager::Instance()->LogMessage( "Error releasing renderer from loading screen thread" );
	pLoading->SetContext( 0 );

	return 0;
}

CLoadingScreen::CLoadingScreen()
{
	m_pBackground = NULL;
	m_pSpinning = NULL;
	m_pThread = NULL;

	m_bRendering = false;
	m_iContextState = 0;
}

void CLoadingScreen::Init()
{
	m_pBackground = new CAnimatedTexture("media/scripts/texture_loading.txt");
	m_pSpinning = new CAnimatedTexture("media/scripts/texture_loading_rotor.txt");
}

CLoadingScreen::~CLoadingScreen()
{
	delete m_pBackground;
	m_pBackground = NULL;
	delete m_pSpinning;
	m_pSpinning = NULL;
}

void CLoadingScreen::StartRendering()
{
	if ( !wglMakeCurrent(0, 0) )
		CLogManager::Instance()->LogMessage( "Error removing the renderer from the main thread" );
	m_iContextState = 0;

	m_bRendering = true;
	m_pBackground->SetAnimation(rand()%m_pBackground->GetAnimCount());
	if ( m_pThread == NULL )
		m_pThread = SDL_CreateThread( LoadingScreenThread, NULL );
}

void CLoadingScreen::StopRendering()
{
	m_bRendering = false;
	if ( m_pThread )
	{
		int status;
		SDL_WaitThread( m_pThread, &status );
		m_pThread = NULL;
		if ( !wglMakeCurrent(currentDC, currentContext) )
			CLogManager::Instance()->LogMessage( "Error returning the renderer to the main thread" );
	}
}

void CLoadingScreen::Render( float fTime )
{
	SDL_Rect target;
	CRenderer *pRenderer = CRenderer::Instance();

	target.w = 2048; target.h = 1536; target.x = -1024; target.y = -768;
	pRenderer->RenderQuad( target, m_pBackground, 0, 1 );

	target = m_pSpinning->GetSize();
	target.w += target.w; target.h += target.h;
	target.x = -80; target.y = 300;
	m_pSpinning->UpdateFrame( fTime );
	pRenderer->RenderQuad( target, m_pSpinning, 0, 1 );

	SDL_GL_SwapBuffers();
}

void CLoadingScreen::RequestContext()
{
	if ( m_bRendering )
	{
		while ( m_iContextState == 0 )
		{
			SDL_Delay(5);
		}
		m_iContextState = 3;
		while ( m_iContextState != 4 )
		{
			SDL_Delay(5);
		}
		wglMakeCurrent( currentDC, currentContext );
	}
}

void CLoadingScreen::ReturnContext()
{
	if ( m_bRendering )
	{
		wglMakeCurrent( 0, 0 );
		m_iContextState = 1;
	}
}

void CLoadingScreen::CheckContext()
{
	while ( m_iContextState != 2 )
	{
		if ( m_iContextState == 1 )
		{
			wglMakeCurrent( currentDC, currentContext );
			m_iContextState = 2;
		}
		if ( m_iContextState == 3 )
		{
			wglMakeCurrent( 0, 0 );
			m_iContextState = 4;
		}
	}
}