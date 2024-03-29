#include "Core.h"
#include "LogManager.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "State_Game.h"
#include "State_Menu.h"
#include "WiimoteManager.h"
#include "ODEManager.h"
#include "SoundManager.h"
#include "ParticleSystemManager.h"
#include "GameSettings.h"

#include <time.h>

#include "LoadingScreen.h"

CCore *CCore::m_pInstance = NULL;
bool g_bThreaded = SETS->ODE_THREAD;

CCore::CCore()
{
	m_pRenderer = NULL;
	m_pResourceManager = NULL;
	m_pActiveState = NULL;
	m_pWiimoteManager = NULL;
	m_pODEManager = NULL;
	m_pSoundManager = NULL;
	m_pParticleSystemManagerNear = NULL;
	m_pParticleSystemManagerFar = NULL;
	m_szLastPlayedLevel = "";
	m_iLastAmountOfPlayers = 0;
}

CCore::~CCore()
{
}

bool CCore::SystemsInit()
{
	m_bRunningValid = false;
	CLogManager::Instance()->LogMessage("Initializing engine.");

	unsigned int seed = (unsigned int)time(NULL);
	CLogManager::Instance()->LogMessage("Seeding game: " + itoa2(seed));
	srand( seed );


	CGameSettings::Instance();

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;

	// Initialize renderer
	m_pRenderer = CRenderer::Instance();
	if ( !m_pRenderer->Init() )
		return false;

	// Initialize resource manager
	m_pResourceManager = CResourceManager::Instance();

	// Initialize particle system manager
	m_pParticleSystemManagerNear = CParticleSystemManager::InstanceNear();
	m_pParticleSystemManagerFar = CParticleSystemManager::InstanceFar();

	// Initialize wiimote manager
	m_pWiimoteManager = CWiimoteManager::Instance();

	// Initialize ODE
	m_pODEManager = CODEManager::Instance();

	// Initialize sound
	m_pSoundManager = CSoundManager::Instance();
	if ( !m_pSoundManager->Init() )
		return false;

	CLoadingScreen::Instance()->Init();

	// Initialize active state
	m_bMenu = SETS->MENU_ON;
	if ( m_bMenu )
	{
		m_iLastAmountOfPlayers = 1;
		m_pActiveState = new CMenuState();
	}
	else
	{
		CLoadingScreen::Instance()->StartRendering();
		m_pActiveState = new CGameState();
		((CGameState *)m_pActiveState)->Init( SETS->PLAYERS );
		CLoadingScreen::Instance()->StopRendering();
		m_szLastPlayedLevel = SETS->LEVEL;
		m_iLastAmountOfPlayers = SETS->PLAYERS;
	}
	m_bRunningValid = true;
	m_pWiimoteManager->RegisterListener( m_pActiveState, -1 );

	// All systems go!
	CLogManager::Instance()->LogMessage("Initializion succesful.");
	return true;
}

void CCore::SystemsDestroy()
{
	CLogManager::Instance()->LogMessage("Terminating engine.");

	// Terminate ODE Manager
	if ( m_pODEManager )
	{
		CODEManager::Destroy();
		m_pODEManager = NULL;
	}

	// Terminate state
	if ( m_pActiveState )
	{
		delete m_pActiveState;
		m_pActiveState = NULL;
		m_bRunningValid = false;
	}

	// Terminate Wiimote manager
	if ( m_pWiimoteManager )
	{
		CWiimoteManager::Destroy();
		m_pWiimoteManager = NULL;
	}

	// Terminate resource manager
	if ( m_pResourceManager )
	{
		CResourceManager::Destroy();
		m_pResourceManager = NULL;
	}

	// Terminate particle system manager
	if ( m_pParticleSystemManagerNear )
	{
		CParticleSystemManager::DestroyNear();
		m_pParticleSystemManagerNear = NULL;
	}
	if ( m_pParticleSystemManagerFar )
	{
		CParticleSystemManager::DestroyFar();
		m_pParticleSystemManagerFar = NULL;
	}

	// Terminate sound manager
	if ( m_pSoundManager )
	{
		m_pSoundManager->Shutdown();
		CSoundManager::Destroy();
		m_pSoundManager = NULL;
	}

	// Terminate renderer
	if ( m_pRenderer )
	{
		m_pRenderer->Shutdown();
		CRenderer::Destroy();
		m_pRenderer = NULL;
	}

	CLogManager::Instance()->LogMessage("Termination complete.");
}

void CCore::Run()
{
	Uint32 time, lastUpdate;
	SDL_Event event;

	static float stime = 0;
	static int frames = 0;

	// Start handling Wiimote events
	CLogManager::Instance()->LogMessage("Starting Wiimote thread.");
	m_pWiimoteManager->StartEventThread();
	if ( g_bThreaded )
		m_pODEManager->StartEventThread();

	// Ensure nunchuk is working
/*	while ( !m_pWiimoteManager->HasNunchuk() )
	{
		CLogManager::Instance()->LogMessage("Unable to find nunchuks, restarting Wiimote manager!");
		m_pWiimoteManager->StopEventThread();
		CWiimoteManager::Destroy();
		m_pWiimoteManager = CWiimoteManager::Instance();
		if ( m_pActiveState )
			m_pWiimoteManager->RegisterListener( m_pActiveState, -1 );
		m_pWiimoteManager->StartEventThread();
	}*/

	// Game loop goes here
	CLogManager::Instance()->LogMessage("Starting game loop.");
	while ( !ShouldQuit() )
	{
		time = lastUpdate = SDL_GetTicks();
		while ( IsRunning() )
		{
			// Handle SDL events
			while ( SDL_PollEvent( &event ) )
			{
				m_pActiveState->HandleSDLEvent( event );
			}

			// Do update
			time = SDL_GetTicks();
			float timeSinceLastUpdate = (float)(time - lastUpdate) / 1000.0f;

			float u1, u2, u3, r;
			if ( m_pSoundManager ) m_pSoundManager->Update( timeSinceLastUpdate );
			u1 = (float)(SDL_GetTicks() - lastUpdate) / 1000.0f;
			if ( !g_bThreaded )
			{
				if ( !((CGameState *)m_pActiveState)->IsPaused() )
				{
					if ( m_pODEManager ) m_pODEManager->Update( timeSinceLastUpdate );
					u3 = (float)(SDL_GetTicks() - lastUpdate) / 1000.0f;
				}
			}
			if ( m_pRenderer ) m_pRenderer->Update( timeSinceLastUpdate );
				u2 = (float)(SDL_GetTicks() - lastUpdate) / 1000.0f;

			stime += timeSinceLastUpdate;
			frames++;
			if ( stime > 1.0f ) // 1 second has passed
			{
				//CLogManager::Instance()->LogMessage("FPS: " + itoa2(frames));
				frames = 0;
				stime = 0.0f;
			}

			// Handle rendering
			if ( m_pRenderer ) m_pRenderer->Render();
			r = (float)(SDL_GetTicks() - lastUpdate) / 1000.0f;
			lastUpdate = time;

			//CLogManager::Instance()->LogMessage( "Times: " + ftoa2(u1 * 1000.0f - timeSinceLastUpdate * 1000.0f) + ", " + ftoa2(u2 * 1000.0f - u1 * 1000.0f) + ", " + ftoa2(u3 * 1000.0f - u2 * 1000.0f) + ", " + ftoa2(r * 1000.0f - u3 * 1000.0f) );
		}

		if ( !ShouldQuit() )
		{
			CLoadingScreen::Instance()->StartRendering();
			m_pRenderer->UnregisterAll();

			if ( g_bThreaded )
				m_pODEManager->StopEventThread();
			CODEManager::Destroy();
			m_pODEManager = CODEManager::Instance();

			CParticleSystemManager::DestroyNear();
			CParticleSystemManager::DestroyFar();
			m_pParticleSystemManagerNear = CParticleSystemManager::InstanceNear();
			m_pParticleSystemManagerFar = CParticleSystemManager::InstanceFar();

			m_pWiimoteManager->UnregisterAll();

			if ( m_bMenu )
			{
				int players = ((CMenuState *)m_pActiveState)->GetPlayersSelected();
				if ( players == 0 )
					players = m_iLastAmountOfPlayers;
				if ( players != m_iLastAmountOfPlayers )
					m_iLastAmountOfPlayers = players;

				std::string selectedLevel = ((CMenuState *)m_pActiveState)->GetSelectedLevel();
				if ( selectedLevel == "" )
					selectedLevel = m_szLastPlayedLevel;
				if ( selectedLevel != m_szLastPlayedLevel )
					m_szLastPlayedLevel = selectedLevel;

				m_bRunningValid = false;
				delete m_pActiveState;
				m_pActiveState = new CGameState();

				((CGameState *)m_pActiveState)->Init( players, selectedLevel );
				m_pODEManager->m_pUniverse = ((CGameState *)m_pActiveState)->GetUniverse();
			}
			else
			{
				CGameState *pGameState = ((CGameState *)m_pActiveState);
				int iScores[4];
				for ( int i = 0; i<4; i++ )
					iScores[i] = pGameState->GetScore( i );

				m_bRunningValid = false;
				delete m_pActiveState;
				m_pActiveState = new CMenuState(SCORE, iScores[0], iScores[1], iScores[2], iScores[3]);
				((CMenuState *)m_pActiveState)->AllowReplay();
			}

			if ( g_bThreaded )
				m_pODEManager->StartEventThread();
			m_pWiimoteManager->RegisterListener( m_pActiveState, -1 );
			m_bMenu = !m_bMenu;
			m_bRunningValid = true;
			CLoadingScreen::Instance()->StopRendering();
		}
	}

	// Stop handling Wiimote events
	CLogManager::Instance()->LogMessage("Stopping Wiimote thread.");
	m_pWiimoteManager->StopEventThread();
	if ( g_bThreaded )
		m_pODEManager->StopEventThread();
}

bool CCore::IsRunning()
{
	if ( m_bRunningValid )
	{
		if ( m_pActiveState )
			return m_pActiveState->IsRunning();
	}
	return true;
}

bool CCore::ShouldQuit()
{
	if ( m_pActiveState )
		return m_pActiveState->ShouldQuit();
	return false;
}

bool CCore::IsPaused()
{
	if ( m_bRunningValid )
	{
		if ( m_pActiveState->getType() == GAMESTATE )
		{
			CGameState *state = (CGameState *)m_pActiveState;
			return state->IsPaused();
		}
	}
	return true;
}
