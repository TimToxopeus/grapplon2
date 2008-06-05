#include "Background.h"
#include "Renderer.h"
#include "AnimatedTexture.h"

#include "GameSettings.h"

CBackground::CBackground()
{
	SetDepth( -1000.0f );
	m_pSpace = new CAnimatedTexture("media/scripts/texture_bkg_space.txt");
	m_pNebula = new CAnimatedTexture("media/scripts/texture_bkg_movinglayer.txt");
	m_pStar = new CAnimatedTexture("media/scripts/texture_bkg_star.txt");
	m_fNebulaOffset = -1024.0f;
}

CBackground::~CBackground()
{
	delete m_pSpace;
	delete m_pNebula;
	delete m_pStar;
}

void CBackground::Update( float fTime )
{
	m_pSpace->UpdateFrame( fTime );
	m_pNebula->UpdateFrame( fTime );
	m_fNebulaOffset += 75.0f * fTime;
	if ( m_fNebulaOffset >= 4096 )
		m_fNebulaOffset -= 4096;

	for ( int i = m_vStars.size() - 1; i>=0; i-- )
	{
		m_vStars[i].timeLeft -= fTime;
		if ( m_vStars[i].timeLeft <= 0.0f )
			m_vStars.erase( m_vStars.begin() + i );
	}

	int size = m_vStars.size();
	if ( size < SETS->MAX_STARS )
	{
		for ( int i = 0; i<SETS->MAX_STARS - size; i++ )
		{
			int x, y;
			x = rand()%2048 - 1024;
			y = rand()%1536 - 768;
			int sec = rand()%180;
			float time = sec/60.0f;
			m_vStars.push_back( Star( x, y, time ) );
		}
	}
}

void CBackground::Render()
{
	SDL_Rect target;
	target.w = 2048;
	target.h = 1536;
	target.x = -1024;
	target.y = -768;

	CRenderer::Instance()->SetCamera( Vector( 0, 0, 0 ), 2.0f );

	// Render space
	RenderQuad( target, m_pSpace, 0 );

	// Render stars
	target = m_pStar->GetSize();
	target.w += target.w;
	target.h += target.h;
	for ( unsigned int i = 0; i<m_vStars.size(); i++ )
	{
		target.x = m_vStars[i].x;
		target.y = m_vStars[i].y;

		float alpha = 1.0f;
		float halfTime = m_vStars[i].maxTime / 2.0f;
		if ( m_vStars[i].timeLeft <= halfTime )
			alpha = 1.0f * m_vStars[i].timeLeft;
		else
			alpha = 1.0f - (1.0f * (m_vStars[i].timeLeft - halfTime));
		RenderQuad( target, m_pStar, 90.0f * m_vStars[i].timeLeft, alpha );
	}

	// Render nebula
	target.w = 4096;
	target.h = 1536;
	target.y = -768;
	target.x = -1024 + (int)m_fNebulaOffset;
	RenderQuad( target, m_pNebula, 0, 0.6f );
	target.x = -(1024 + 4096) + (int)m_fNebulaOffset;
	RenderQuad( target, m_pNebula, 0, 0.6f );
}
