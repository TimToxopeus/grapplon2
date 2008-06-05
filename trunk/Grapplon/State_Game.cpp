#include "State_Game.h"
#include "ResourceManager.h"
#include "WiimoteManager.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "SoundManager.h"
#include "math.h"
#include "AnimatedTexture.h"
#include "Sound.h"
#include "Universe.h"
#include "Renderer.h"
#include "Background.h"
#include "HUD.h"
#include "GameSettings.h"

#include "PowerUp.h"

CGameState::CGameState()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	m_pUniverse = NULL;
	m_pBackground = NULL;
	m_pHUD = NULL;

	m_bRunning = true;
	m_bQuit = false;

	box.x = 20;
	box.y = 10;
	box.w = 256;
	box.h = 256;

	SetDepth( -10.0f );
}

bool CGameState::Init( int iPlayers, std::string level )
{
//	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/xpstart.wav", RT_SOUND);
//	pSound->Play();
	CSoundManager::Instance()->LoadSound( "media/music/exit.ogg" );

	m_pUniverse = new CUniverse();
	if ( level == "" )
		m_pUniverse->Load( CGameSettings::Instance()->LEVEL );
	else
		m_pUniverse->Load( level );

	if ( iPlayers > 0 )
	{
		m_pPlayers[0] = new CPlayerObject(0);
		m_pPlayers[0]->SetPosition( m_pUniverse->m_iInitSpawnPos1 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[0], 0 );
	}

	if ( iPlayers > 1 )
	{
		m_pPlayers[1] = new CPlayerObject(1);
		m_pPlayers[1]->SetPosition( m_pUniverse->m_iInitSpawnPos2 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[1], 1 );
	}

	if ( iPlayers > 2 )
	{
		m_pPlayers[2] = new CPlayerObject(2);
		m_pPlayers[2]->SetPosition( m_pUniverse->m_iInitSpawnPos3 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[2], 2 );
	}

	if ( iPlayers > 3 )
	{
		m_pPlayers[3] = new CPlayerObject(3);
		m_pPlayers[3]->SetPosition( m_pUniverse->m_iInitSpawnPos4 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[3], 3 );
	}

	m_pHUD = new CHUD();
	m_pHUD->SetPlayers( m_pPlayers[0], m_pPlayers[1], m_pPlayers[2], m_pPlayers[3] );

	m_pBackground = new CBackground();

//	m_pPlayers[0]->SetDepth( 1.0f );

//	m_pPlayers[1]->SetDepth( -1.0f );
//	m_pPlayers[2]->SetDepth( 0.0f );
//	m_pPlayers[3]->SetDepth( 2.0f );

/*	m_pPlayers[0]->SetMass( 10000.0f );
	m_pPlayers[1]->SetMass( 100.0f );
	m_pPlayers[2]->SetMass( 10000.0f );
	m_pPlayers[3]->SetMass( 100.0f );*/


	m_fMatchTimeLeft = SETS->MATCH_TIME;

	return true;
}

CGameState::~CGameState()
{
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			delete m_pPlayers[i];
			m_pPlayers[i] = NULL;
		}
	}
	if ( m_pUniverse )
	{
		delete m_pUniverse;
		m_pUniverse = NULL;
		CODEManager::Instance()->m_pUniverse = NULL;
	}

	if ( m_pBackground )
	{
		delete m_pBackground;
		m_pBackground = NULL;
	}

	if ( m_pHUD )
	{
		delete m_pHUD;
		m_pHUD = NULL;
	}
}

void CGameState::Render()
{
	Vector playerCenter;
	float c = 0;
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			playerCenter += m_pPlayers[i]->GetPosition();
			c += 1.0f;
		}
	}
	playerCenter /= c;

	// Furthest distance
	float distance = 0.0f;
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			float d = (playerCenter - m_pPlayers[i]->GetPosition()).Length();
			if ( d > distance )
				distance = d;
		}
	}
	float zoom = distance / 300.0f;
	if ( zoom < SETS->MIN_ZOOM )
		zoom = SETS->MIN_ZOOM;
	if ( zoom > SETS->MAX_ZOOM )
		zoom = SETS->MAX_ZOOM;
	CRenderer::Instance()->SetCamera( playerCenter, zoom );
}

void CGameState::Update(float fTime)
{
	m_fMatchTimeLeft -= fTime;
	m_pHUD->SetMatchTimeLeft( m_fMatchTimeLeft );

	if ( m_fMatchTimeLeft <= 0.0f )
		m_bRunning = false;
}

bool CGameState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		struct nunchuk_t* nc = (nunchuk_t*)&pWiimoteEvent->exp.nunchuk;
		if ( nc->js.mag > 0.3f )
		{
			float angle = (nc->js.ang - 90.0f) * (3.14f / 180.0f);
			box.x += (int)(cos(angle) * (5.0f * nc->js.mag));
			box.y += (int)(sin(angle) * (5.0f * nc->js.mag));
			return true;
		}
	}
	return false;
}

int CGameState::HandleSDLEvent(SDL_Event event)
{
	if ( event.type == SDL_QUIT )
	{
		m_bRunning = false;
		m_bQuit = true;
	}
	if ( event.type == SDL_KEYUP )
	{
		if ( event.key.keysym.sym == SDLK_SPACE )
		{
			m_bRunning = false;
		}
	}
	return 0;
}

int CGameState::GetScore( int iPlayer )
{
	if ( m_pPlayers[iPlayer] )
		return m_pPlayers[iPlayer]->m_iScore;
	return -1;
}

void CGameState::AddScore( int iPlayer, int iScore, int iX, int iY )
{
	m_pHUD->AddScore( iPlayer, iScore, iX, iY );
}
