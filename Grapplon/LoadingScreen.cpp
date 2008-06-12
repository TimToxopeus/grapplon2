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

	while ( pLoading->IsRunning() )
	{
		pLoading->ThreadClaimContext();
		time = SDL_GetTicks();
		float timeSinceLastUpdate = (float)(time - lastUpdate) / 1000.0f;
		pLoading->Render( timeSinceLastUpdate );
		pLoading->ThreadReleaseContext();
 		SDL_Delay( 5 );
		lastUpdate = time;
	}

	if ( !wglMakeCurrent(0, 0) )
		CLogManager::Instance()->LogMessage( "Error releasing renderer from loading screen thread" );

	return 0;
}

CLoadingScreen::CLoadingScreen()
{
	m_pBackground = NULL;
	m_pSpinning = NULL;
	m_pThread = NULL;

	m_bRendering = false;

	m_pMutex = SDL_CreateMutex();
	m_pThreadCondition = SDL_CreateCond();
	m_pEngineCondition = SDL_CreateCond();
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

	SDL_DestroyMutex( m_pMutex );
	SDL_DestroyCond( m_pThreadCondition );
	SDL_DestroyCond( m_pEngineCondition );
}

void CLoadingScreen::StartRendering()
{
	if ( !wglMakeCurrent(0, 0) )
		CLogManager::Instance()->LogMessage( "Error removing the renderer from the main thread" );

	m_iContext = 1;
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
		CLogManager::Instance()->LogMessage( "Waiting for loading thread to terminate." );
		SDL_CondSignal( m_pEngineCondition );
		SDL_WaitThread( m_pThread, &status );
		CLogManager::Instance()->LogMessage( "Loading thread to terminated." );
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
	target.x = -80; target.y = 100;
	m_pSpinning->UpdateFrame( fTime );
	pRenderer->RenderQuad( target, m_pSpinning, 0, 1 );

	SDL_GL_SwapBuffers();
}

void CLoadingScreen::ThreadClaimContext()
{
	SDL_mutexP( m_pMutex );

	if ( m_iContext == 0 )
	{
		SDL_CondWait( m_pEngineCondition, m_pMutex );
	}

	wglMakeCurrent( currentDC, currentContext );
	m_iContext = 1;
}

void CLoadingScreen::ThreadReleaseContext()
{
	wglMakeCurrent( 0, 0 ); // Release context

	SDL_mutexV( m_pMutex );
	SDL_CondSignal( m_pThreadCondition );
}

void CLoadingScreen::EngineClaimContext()
{
	if ( m_bRendering )
	{
		SDL_mutexP( m_pMutex );
		if ( m_iContext == 1 )
		{
			SDL_CondWait( m_pThreadCondition, m_pMutex );
		}

		wglMakeCurrent( currentDC, currentContext );
		m_iContext = 0;
	}
}

void CLoadingScreen::EngineReleaseContext()
{
	if ( m_bRendering )
	{
		wglMakeCurrent( 0, 0 ); // Release context

		SDL_mutexV( m_pMutex );
		SDL_CondSignal( m_pEngineCondition );
	}
}
