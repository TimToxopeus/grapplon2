#pragma once

#include "StateManager.h"
#include <sdl.h>

// Forward declaration
class CAnimatedTexture;
class CPlayerObject;
class CUniverse;
class CBackground;
class CHUD;

class CGameState : public IStateManager
{
private:
	SDL_Rect box;
	CPlayerObject *m_pPlayers[4];
	CUniverse *m_pUniverse;

	float m_fMatchTimeLeft;
	CBackground *m_pBackground;
	CHUD *m_pHUD;

public:
	CGameState();
	virtual ~CGameState();

	bool Init( int iPlayers, std::string level = "" );
	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );

	void AddScore( int iPlayer, int iScore, int iX, int iY );

	int GetScore( int iPlayer );
	CUniverse *GetUniverse() { return m_pUniverse; }
};
