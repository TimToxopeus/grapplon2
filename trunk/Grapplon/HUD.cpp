#include "HUD.h"
#include "PlayerObject.h"
#include "Renderer.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "AnimatedTexture.h"
#include <algorithm>
#include <functional>

CHUD::CHUD()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	SetDepth( 100.0f );
	m_eType = HUD;

	m_pCountDown[0] = new CAnimatedTexture("media/scripts/texture_countdown_3.txt");
	m_pCountDown[1] = new CAnimatedTexture("media/scripts/texture_countdown_2.txt");
	m_pCountDown[2] = new CAnimatedTexture("media/scripts/texture_countdown_1.txt");
	m_pCountDown[3] = new CAnimatedTexture("media/scripts/texture_countdown_go.txt");

	m_pShips = new CAnimatedTexture("media/scripts/texture_hud_ship.txt");

	m_pBorders = new CAnimatedTexture( "media/scripts/texture_hud_border.txt" );
	m_pHealth = new CAnimatedTexture( "media/scripts/texture_hud_bar.txt" );
	m_pNumbers = new CAnimatedTexture( "media/scripts/texture_numbers.txt" );

	m_pGameEnd = new CAnimatedTexture( "media/scripts/texture_game_end.txt" );
	m_pWins = new CAnimatedTexture( "media/scripts/texture_wins.txt" );
	m_pPlayer = new CAnimatedTexture("media/scripts/texture_player.txt");
	m_pExplosion = new CAnimatedTexture("media/scripts/texture_explosion.txt");

	m_fScoreTiming = 0.0f;
	m_iSoundStep = 0;
	m_fPlayerOffset = 0.0f;
	m_iWinner = -1;
}

CHUD::~CHUD()
{
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pCountDown[i] )
		{
			delete m_pCountDown[i];
			m_pCountDown[i] = NULL;
		}
	}

	delete m_pBorders;
	delete m_pHealth;
	delete m_pNumbers;

	delete m_pGameEnd;
	delete m_pWins;
	delete m_pPlayer;
	delete m_pExplosion;
	delete m_pShips;
}

void CHUD::SetPlayers( CPlayerObject *p1, CPlayerObject *p2, CPlayerObject *p3, CPlayerObject *p4 )
{
	m_pPlayers[0] = p1;
	m_pPlayers[1] = p2;
	m_pPlayers[2] = p3;
	m_pPlayers[3] = p4;
}

void CHUD::Update( float fTime )
{
	if ( m_vScores.size() > 0 )
	{
		for ( int i = m_vScores.size() - 1; i>=0; i-- )
		{
			m_vScores[i].m_fTimeLeft -= fTime;
			if ( m_vScores[i].m_fTimeLeft <= 0.0f )
				m_vScores.erase( m_vScores.begin() + i );
		}
	}

	for ( int i = 0; i<4; i++ )
		m_pCountDown[i]->UpdateFrame( fTime );

	if ( m_fMatchTimeLeft <= 5.0f && m_fMatchTimeLeft >= 4.0f )
	{
		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Countdown.wav", RT_SOUND);
		pSound->Play();
	}

	if ( m_fMatchTimeLeft == 0.0f )
	{
		if ( m_iWinner == -1 )
		{
			std::vector<int> m_vScores;
			for ( int i = 0; i<4; i++ )
				if ( m_pPlayers[i] )
					m_vScores.push_back( m_pPlayers[i]->m_iScore );
			std::sort( m_vScores.begin(), m_vScores.end(), std::greater<int>() );

			if ( m_vScores[0] == m_vScores[1] )
			{
				m_iWinner = 4; // DRAW
			}
			else
			{
				for ( int i = 0; i<4; i++ )
				{
					if ( m_pPlayers[i] )
					{
						if ( m_vScores[0] == m_pPlayers[i]->m_iScore )
						{
							m_iWinner = i;
							break;
						}
					}
				}
			}
			m_pPlayer->SetAnimation(m_iWinner);
		}

		m_fScoreTiming += fTime;
		if ( m_fScoreTiming >= 0.5f && m_iSoundStep == 0 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Collision_Schip_Asteroid.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 1.5f && m_iSoundStep == 1 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Collision_Schip_Asteroid.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 2.0f && m_iSoundStep == 2 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Collision_Schip_Asteroid.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 2.5f && m_iSoundStep == 3 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Collision_Schip_Asteroid.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 3.0f && m_iSoundStep == 4 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Collision_Schip_Asteroid.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 4.0f && m_iSoundStep == 5 )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Explosion4.wav", RT_SOUND);
			pSound->Play(true);
			m_iSoundStep++;
		}

		if ( m_fScoreTiming >= 4.0f )
			m_pExplosion->UpdateFrame( fTime );
	}
}

void CHUD::Render()
{
	SDL_Rect target = m_pNumbers->GetSize();
	for ( unsigned int i = 0; i<m_vScores.size(); i++ )
	{
		m_pNumbers->SetAnimation( m_vScores[i].m_iPlayer );
		target.y = m_vScores[i].y;
		int x = m_vScores[i].x;

		for ( unsigned int a = 0; a<m_vScores[i].m_szScore.length(); a++ )
		{
			unsigned int v = (unsigned int)(m_vScores[i].m_szScore[a] - 48);
			m_pNumbers->SetFrame( v );

			target.x = x + 32 * a;
			RenderQuad( target, m_pNumbers, 0, 1 );
		}
	}

	CRenderer::Instance()->SetCamera( Vector( 0, 0, 0 ), 2.0f );
	if ( m_pPlayers[0] )
	{
		DrawHitpointBar( -1000, -744, 0, ((float)m_pPlayers[0]->GetHitpoints() / (float)m_pPlayers[0]->GetMaxHitpoints()) );
		DrawScoreBar( -830, -694, 0, m_pPlayers[0]->m_iScore, false );
	}
	if ( m_pPlayers[1] )
	{
		DrawHitpointBar( 680, -744, 1, ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()) );
		DrawScoreBar( 850, -694, 1, m_pPlayers[1]->m_iScore, true );
	}
	if ( m_pPlayers[2] )
	{
		DrawHitpointBar( -1000, 580, 2, ((float)m_pPlayers[2]->GetHitpoints() / (float)m_pPlayers[2]->GetMaxHitpoints()) );
		DrawScoreBar( -830, 630, 2, m_pPlayers[2]->m_iScore, false );
	}
	if ( m_pPlayers[3] )
	{
		DrawHitpointBar( 680, 580, 3, ((float)m_pPlayers[3]->GetHitpoints() / (float)m_pPlayers[3]->GetMaxHitpoints()) );
		DrawScoreBar( 850, 630, 3, m_pPlayers[3]->m_iScore, true );
	}

	if ( m_fCountdown > -1.0f )
	{
		SDL_Rect target;
		if ( m_fCountdown > 2.0f )
		{
			float delta = 3.0f - m_fCountdown;

			// Render 0
			target = m_pCountDown[0]->GetSize();
			target.w = (int)(target.w * (1.0f + delta));
			target.h = (int)(target.h * (1.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[0], 0, 1.0f - (0.5f * delta));
		}
		else if ( m_fCountdown > 1.0f )
		{
			float delta = 2.0f - m_fCountdown;

			// Render 0 and 1
			target = m_pCountDown[1]->GetSize();
			target.w = (int)(target.w * (1.0f + delta));
			target.h = (int)(target.h * (1.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[1], 0, 1.0f - (0.5f * delta));

			target = m_pCountDown[0]->GetSize();
			target.w = (int)(target.w * (2.0f + delta));
			target.h = (int)(target.h * (2.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[0], 0, 0.5f - (0.5f * delta));
		}
		else if ( m_fCountdown > 0.0f )
		{
			float delta = 1.0f - m_fCountdown;

			// Render 1 and 2
			target = m_pCountDown[2]->GetSize();
			target.w = (int)(target.w * (1.0f + delta));
			target.h = (int)(target.h * (1.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[2], 0, 1.0f - (0.5f * delta));

			target = m_pCountDown[1]->GetSize();
			target.w = (int)(target.w * (2.0f + delta));
			target.h = (int)(target.h * (2.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[1], 0, 0.5f - (0.5f * delta));
		}
		else if ( m_fCountdown > -1.0f )
		{
			float delta = 0.0f - m_fCountdown;

			// Render 2 and Go
			target = m_pCountDown[3]->GetSize();
			target.w = (int)(target.w * (1.0f + delta));
			target.h = (int)(target.h * (1.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[3], 0, 1.0f - (0.5f * delta));

			target = m_pCountDown[2]->GetSize();
			target.w = (int)(target.w * (2.0f + delta));
			target.h = (int)(target.h * (2.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[2], 0, 0.5f - (0.5f * delta));
		}
		else if ( m_fCountdown > -2.0f )
		{
			float delta = -1.0f - m_fCountdown;

			// Render Go
			target = m_pCountDown[3]->GetSize();
			target.w = (int)(target.w * (2.0f + delta));
			target.h = (int)(target.h * (2.0f + delta));
			target.x = -(target.w / 2);
			target.y = -(target.h / 2);
			RenderQuad(target, m_pCountDown[3], 0, 0.5f - (0.5f * delta));
		}
	}

	if ( m_fMatchTimeLeft == 0.0f )
	{
		target.w = 2048;
		target.h = 1536;
		target.x = -1024;
		target.y = -768;
		SDL_Color black;
		black.r = black.g = black.b = 0;
		RenderQuad(target, NULL, 0, black, 0.6f);

		if ( m_fScoreTiming >= 0.5f )
		{
			target = m_pGameEnd->GetSize();
			target.x = -(target.w);
			target.y = -(target.h) - 350;
			target.w += target.w;
			target.h += target.h;
			RenderQuad(target, m_pGameEnd, 0, 1);
		}

		if ( m_fScoreTiming >= 1.5f )
		{
			DrawHitpointBar( -250, -250, 0, 0 );
			DrawScoreBar( -80, -200, 0, m_pPlayers[0]->m_iScore, false );

			if ( m_fScoreTiming < 4.0f || m_iWinner == 0 )
			{
				m_pShips->SetAnimation(0);
				target = m_pShips->GetSize();
				target.x = -250;//-(target.w) - 75;
				target.y = -250;//-(target.h) - 100;
				RenderQuad(target, m_pShips, 0, 1);
			}
			else if ( !m_pExplosion->IsFinished() )
			{
				target = m_pExplosion->GetSize();
				target.x = -(target.w) - 75;
				target.y = -(target.h) - 100;
				RenderQuad(target, m_pExplosion, 0, 1);
			}
		}

		if ( m_fScoreTiming >= 2.0f )
		{
			DrawHitpointBar( -80, -150, 1, 0 );
			DrawScoreBar( 90, -100, 1, m_pPlayers[1]->m_iScore, true );

			if ( m_fScoreTiming < 4.0f || m_iWinner == 1 )
			{
				m_pShips->SetAnimation(1);
				target = m_pShips->GetSize();
				target.x = -80;//-(target.w) + 240;
				target.y = -150;//-(target.h);
				RenderQuad(target, m_pShips, 0, 1);
			}
			else if ( !m_pExplosion->IsFinished() )
			{
				target = m_pExplosion->GetSize();
				target.x = -(target.w) + 240;
				target.y = -(target.h);
				RenderQuad(target, m_pExplosion, 0, 1);
			}
		}

		if ( m_pPlayers[2] )
		{
			if ( m_fScoreTiming >= 2.5f )
			{
				DrawHitpointBar( -250, -50, 2, 0 );
				DrawScoreBar( -80, 0, 2, m_pPlayers[2]->m_iScore, false );

				if ( m_fScoreTiming < 4.0f || m_iWinner == 2 )
				{
					m_pShips->SetAnimation(2);
					target = m_pShips->GetSize();
					target.x = -250;//-(target.w) - 75;
					target.y = -50;//-(target.h) + 100;
					RenderQuad(target, m_pShips, 0, 1);
				}
				else if ( !m_pExplosion->IsFinished() )
				{
					target = m_pExplosion->GetSize();
					target.x = -(target.w) - 75;
					target.y = -(target.h) + 100;
					RenderQuad(target, m_pExplosion, 0, 1);
				}
			}
		}

		if ( m_pPlayers[3] )
		{
			if ( m_fScoreTiming >= 3.0f )
			{
				DrawHitpointBar( -80, 50, 3, 0 );
				DrawScoreBar( 90, 100, 3, m_pPlayers[3]->m_iScore, true );

				if ( m_fScoreTiming < 4.0f || m_iWinner == 3 )
				{
					m_pShips->SetAnimation(3);
					target = m_pShips->GetSize();
					target.x = -80;//-(target.w) + 240;
					target.y = 50;//-(target.h) + 200;
					RenderQuad(target, m_pShips, 0, 1);
				}
				else if ( !m_pExplosion->IsFinished() )
				{
					target = m_pExplosion->GetSize();
					target.x = -(target.w) + 240;
					target.y = -(target.h) + 200;
					RenderQuad(target, m_pExplosion, 0, 1);
				}
			}
		}

		if ( m_fScoreTiming >= (4.0f - m_fPlayerOffset) )
		{
			target = m_pPlayer->GetSize();
			target.x = -(target.w);
			target.y = -(target.h) + 300;
			target.w += target.w;
			target.h += target.h;
			RenderQuad(target, m_pPlayer, 0, 1);

			target = m_pWins->GetSize();
			target.x = -(target.w);
			target.y = -(target.h) + 400;
			target.w += target.w;
			target.h += target.h;
			RenderQuad(target, m_pWins, 0, 1);
		}
	}

	DrawTimer( 0, -750, m_fMatchTimeLeft );
}

void CHUD::DrawHitpointBar( int x, int y, int player, float healthRatio )
{
	m_pBorders->SetAnimation( player );
	m_pHealth->SetAnimation( player );

	SDL_Rect target = m_pBorders->GetSize();
	target.x = x;
	target.y = y;
	RenderQuad( target, m_pBorders, 0, 1.0f );

	m_pHealth->OverrideHeight( healthRatio );
	target.y = y + (int)(target.h * (1.0f - healthRatio));
	target.h = target.h - (int)(target.h * (1.0f - healthRatio));
	RenderQuad( target, m_pHealth, 0, 1.0f );
}

void CHUD::DrawScoreBar( int x, int y, int player, int score, bool rtl )
{
	m_pNumbers->SetAnimation( player );
	SDL_Rect target = m_pNumbers->GetSize();
	target.y = y;

	std::string szScore = itoa2(score);
	if ( rtl )
	{
		int l = (int)szScore.length();
		for ( int a = l - 1; a >= 0; a-- )
		{
			unsigned int v = (unsigned int)(szScore[(l - 1) - a] - 48);
			m_pNumbers->SetFrame( v );

			target.x = x - 32 * (a + 1);
			RenderQuad( target, m_pNumbers, 0, 1 );
		}
	}
	else
	{
		for ( unsigned int a = 0; a<szScore.length(); a++ )
		{
			unsigned int v = (unsigned int)(szScore[a] - 48);
			m_pNumbers->SetFrame( v );

			target.x = x + 32 * a;
			RenderQuad( target, m_pNumbers, 0, 1 );
		}
	}
}

void CHUD::DrawTimer( int x, int y, float fTime )
{
	m_pNumbers->SetAnimation( 4 );
	SDL_Rect target = m_pNumbers->GetSize();
	target.y = y;

	int minutes = (int)(fTime / 60);
	int seconds = (int)(fTime - minutes * 60);

	std::string szTime = itoa2(minutes) + ":" + (seconds < 10 ? "0" : "") + itoa2(seconds);
	int w = szTime.length() * 32;
	x -= (w / 2);
	for ( unsigned int a = 0; a<szTime.length(); a++ )
	{
		unsigned int v = (unsigned int)(szTime[a] - 48);
		m_pNumbers->SetFrame( v );

		target.x = x + 32 * a;
		RenderQuad( target, m_pNumbers, 0, 1 );
	}
}

void CHUD::AddScore( int iPlayer, int iScore, int iX, int iY )
{
	Score score;
	score.m_iPlayer = iPlayer;
	score.m_szScore = itoa2(iScore);
	score.x = iX;
	score.y = iY;
	score.m_fTimeLeft = 1.0f;
	m_vScores.push_back( score );
}
