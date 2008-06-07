#pragma once

#include "ActiveObject.h"
class CPlayerObject;
class CAnimatedTexture;

#include <string>
#include <vector>

struct Score
{
	int x, y;
	int m_iPlayer;
	std::string m_szScore;
	float m_fTimeLeft;
};

class CHUD : public IActiveObject
{
private:
	CPlayerObject *m_pPlayers[4];
	void DrawHitpointBar( int x, int y, int player, float healthRatio );
	void DrawScoreBar( int x, int y, int player, int score, bool rtl = false );
	void DrawTimer( int x, int y, float fTime );
	CAnimatedTexture *m_pNumbers;
	CAnimatedTexture *m_pCountDown[4];

	CAnimatedTexture *m_pBorders;
	CAnimatedTexture *m_pHealth;
	CAnimatedTexture *m_pGameEnd;
	CAnimatedTexture *m_pWins;
	CAnimatedTexture *m_pPlayer;
	CAnimatedTexture *m_pShips;
	CAnimatedTexture *m_pExplosion;

	float m_fMatchTimeLeft;
	float m_fCountdown;

	int m_iSoundStep;
	float m_fScoreTiming;
	float m_fPlayerOffset;
	int m_iWinner;

	std::vector<Score> m_vScores;

public:
	CHUD();
	~CHUD();

	void SetPlayers( CPlayerObject *p1, CPlayerObject *p2, CPlayerObject *p3, CPlayerObject *p4 );
	void SetMatchTimeLeft( float fMatchTimeLeft ) { m_fMatchTimeLeft = fMatchTimeLeft; }
	void SetCountdown( float fCountdown ) { m_fCountdown = fCountdown; }
	void AddScore( int iPlayer, int iScore, int iX, int iY );

	void Update( float fTime );
	void Render();
};
