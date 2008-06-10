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
#include "WormHole.h"

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

	m_fMatchTimeLeft = 180.0f;
	m_fCountDown = 3.0f;
}

bool CGameState::Init( int iPlayers, std::string level )
{
//	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/xpstart.wav", RT_SOUND);
//	pSound->Play();
	CSoundManager::Instance()->LoadSound( "media/music/music_game.ogg" );

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
	m_fCountDown = 3.0f;
	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/321-3.wav", RT_SOUND);
	if ( pSound )
		pSound->Play();

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

	int level_width = (int) GetUniverse()->m_fWidth;
	int level_height = (int) GetUniverse()->m_fHeight;

	// Note: m_fWidth is actually half the width (same for height)
	int level_l = -level_width;
	int level_r =  level_width;
	int level_t = -level_height;
	int level_b =  level_height;

	// aabb = Axis Aligned Bounding Box of the players (and interpolated wormholes)
	int aabb_l = 99999999;
	int aabb_r = -99999999;
	int aabb_t = 99999999;
	int aabb_b = -99999999;

	Vector playerPos;

	// Calculate aabb
	for( int i = 0; i<4; i++ )
	{
		if( m_pPlayers[i] )
		{
			playerPos = m_pPlayers[i]->GetPosition();
			if(aabb_l > (int) playerPos[0]) aabb_l = (int) playerPos[0];
			if(aabb_r < (int) playerPos[0]) aabb_r = (int) playerPos[0];
			if(aabb_t > (int) playerPos[1]) aabb_t = (int) playerPos[1];
			if(aabb_b < (int) playerPos[1]) aabb_b = (int) playerPos[1];

			std::vector<CWormHole*>& whs = GetUniverse()->m_vWormHoles;
			for(unsigned int w = 0; w < whs.size(); w++){
				CWormHole* thisWH = whs[w];
				Vector WHPos = thisWH->GetPosition();
				float distance = (WHPos - playerPos).Length();
				if( distance < thisWH->m_fZoomRadius){
					float perc = 1 - (distance - thisWH->GetPhysicsData()->m_fRadius) / (thisWH->m_fZoomRadius - thisWH->GetPhysicsData()->m_fRadius);

					Vector DistanceToTwin = (thisWH->twin->GetPosition() - playerPos);
					Vector noticePoint = playerPos + DistanceToTwin*perc;
					
					if(aabb_l > (int) noticePoint[0]) aabb_l = (int) noticePoint[0];
					if(aabb_r < (int) noticePoint[0]) aabb_r = (int) noticePoint[0];
					if(aabb_t > (int) noticePoint[1]) aabb_t = (int) noticePoint[1];
					if(aabb_b < (int) noticePoint[1]) aabb_b = (int) noticePoint[1];
				}
			}
		}
	}

	// Desired view
	int view_l = aabb_l + (int) (SETS->VIEW_PERC * (float) (level_l - aabb_l));
	int view_r = aabb_r + (int) (SETS->VIEW_PERC * (float) (level_r - aabb_r));
	int view_b = aabb_b + (int) (SETS->VIEW_PERC * (float) (level_b - aabb_b));
	int view_t = aabb_t + (int) (SETS->VIEW_PERC * (float) (level_t - aabb_t));

	// Rescale view such that it's aspect ratio is 1024/768
	// For correct aspect ratio, 4/3 = w/h ----> 4h = 3w -----> 0 = 4h - 3w
	int diff = 4*(view_b - view_t) - 3*(view_r - view_l);			// , else enlarge height

	if( diff > 0 ){		// diff > 0 ---> 4h = 3w + diff ---> 4h > 3w ---> enlarge width
		view_l -= (diff / 6);
		view_r += (diff / 6);
	} else {			// diff < 0 ---> 4h + diff = 3w ---> 4h < 3w ---> enlarge height
		view_t -= (diff / 8);
		view_b += (diff / 8);
	}

	// Correct view outside level
	if(view_l < level_l){
		view_r += level_l - view_l;
		view_l = level_l;
	} else if(view_r > level_r){
		view_l += level_r - view_r;
		view_r = level_r;
	}

	if(view_t < level_t){
		view_b += level_t - view_t;
		view_t = level_t;
	} else if(view_b > level_b){
		view_t += level_b - view_b;
		view_b = level_b;
	}
	// View has correct aspect ratio
	// Determine zoom-level
	float zoom = ((view_r - view_l) + 2* SETS->SCREEN_MARGIN) / 1024.0f;					// w = zoom * 1024 --> zoom = w / 1024

	if(zoom < SETS->MIN_ZOOM) zoom = SETS->MIN_ZOOM;
	// Determine center of view
	Vector view_center( (float) ((view_r + view_l) / 2), (float) ((view_t + view_b) / 2), 0.0f);

	
	
	/*
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
*/

	CRenderer::Instance()->SetCamera( view_center, zoom );
}

void CGameState::Update(float fTime)
{

	if(m_fCountDown < 30){
		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/hook_throw.wav", RT_SOUND);
		pSound->SetPitch(1.0f);
	}


	if ( m_fCountDown > -2.0f )
		m_fCountDown -= fTime;
	m_pHUD->SetCountdown( m_fCountDown );

	if ( m_fCountDown <= 0.0f && m_fMatchTimeLeft > 0.0f )
	{
		m_fMatchTimeLeft -= fTime;
		if ( m_fMatchTimeLeft < 0.0f )
			m_fMatchTimeLeft = 0.0f;
	}
	m_pHUD->SetMatchTimeLeft( m_fMatchTimeLeft );

	m_pUniverse->Update( fTime );
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

		if ( m_fMatchTimeLeft <= 0.0f )
		{
			if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) ||
				IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) )
				m_bRunning = false;
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
		if ( event.key.keysym.sym == SDLK_ESCAPE )
		{
			m_bRunning = false;
			m_bQuit = true;
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

bool CGameState::IsPaused()
{
	if ( m_fCountDown > 0.0f ) // 3-2-1 Go! paused
	{
		return true;
	}
	else
	{
		if ( m_fMatchTimeLeft > 0.0f ) // We are still playing
		{
			return false;
		}
		else
		{
			// Game ended
			return true;
		}
	}
}