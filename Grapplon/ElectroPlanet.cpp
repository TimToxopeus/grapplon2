#include "ElectroPlanet.h"
#include "ODEManager.h"

CElectroPlanet::CElectroPlanet(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = ELECTRO;
	m_iTempRadius = data.tempradius;
	m_pElectric = new CAnimatedTexture( "media/scripts/texture_electricplanet_overlay.txt" );
}

CElectroPlanet::~CElectroPlanet()
{
	if ( m_pElectric ) delete m_pElectric;
}

void CElectroPlanet::Render()
{
	CPlanet::Render();
	// Render glow
	if ( m_pElectric )
	{
		SDL_Rect target;
		target = m_pElectric->GetSize();
		target.w = (int)(target.w * m_fSecondaryScale);
		target.h = (int)(target.h * m_fSecondaryScale);
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);
		RenderQuad( target, m_pElectric, 0 );
	}
}

void CElectroPlanet::Update( float fTime )
{
	if ( m_pElectric ) m_pElectric->UpdateFrame( fTime );
	CPlanet::Update( fTime );
}
