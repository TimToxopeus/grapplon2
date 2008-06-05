#include "HUD.h"
#include "PlayerObject.h"
#include "Renderer.h"
#include "LogManager.h"
#include "AnimatedTexture.h"

CHUD::CHUD()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	SetDepth( 100.0f );
	m_eType = HUD;

	m_pBorders = new CAnimatedTexture( "media/scripts/texture_hud_border.txt" );
	m_pHealth = new CAnimatedTexture( "media/scripts/texture_hud_bar.txt" );
	m_pNumbers = new CAnimatedTexture( "media/scripts/texture_numbers.txt" );
}

CHUD::~CHUD()
{
	delete m_pNumbers;

	delete m_pBorders;
	delete m_pHealth;
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
