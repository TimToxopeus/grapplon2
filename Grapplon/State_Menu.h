#pragma once

#include "StateManager.h"
#include "AnimatedTexture.h"

#include <vector>
#include <string>

#define ABMENU 9
#define GAMEMENU 16
#define SCORE 17
#define SCOREINPUT 18
#define PLAYERSELECT 19
#define LEVELSELECT 20
#define HIGH 25
#define IR_AVG 20

enum StateStyle
{
	INSTANT = 0,
	FADE_IN,
	FADE_OUT,
	MOVE_UP,
	PULSE,
};

class StateChange
{
public:
	StateChange( int iState, int iSkipState, CAnimatedTexture *pImage, StateStyle eStyle, bool bIncState, int iStayRendered, float fStartAlpha, float fTime, int iStartX, int iStartY, int iGoalX, int iGoalY, int iAnimation = 0 );

	int m_iState, m_iSkipState;
	CAnimatedTexture *m_pImage;
	StateStyle m_eStyle;
	bool m_bIncState;
	int m_iStayRendered;

	float m_fAlpha;
	float m_fTime, m_fTimeLeft;

	int m_iX, m_iY;
	int m_iStartX, m_iStartY;
	int m_iGoalX, m_iGoalY;

	int m_iAnimation;

	bool IsClicked( int x, int y );
};

class LevelSelectOption
{
public:
	LevelSelectOption( std::string szImage, std::string szInfoText, int x, int y, std::string szLevel );
	~LevelSelectOption();

	CAnimatedTexture *m_pImage;
	CAnimatedTexture *m_pInfoText;
	int x, y;
	std::string m_szLevel;

	bool IsClicked( int x, int y );
};

class CMenuState : public IStateManager
{
protected:
	CAnimatedTexture *m_pSplash1, *m_pSplash2, *m_pSplash3, *m_pTitle;
	CAnimatedTexture *m_pAB;

	CAnimatedTexture *m_pLogo, *m_pNintendo;

	CAnimatedTexture *m_pLogo2;
	CAnimatedTexture *m_pMenuBackground;
	CAnimatedTexture *m_pMenuSingleplayer;
	CAnimatedTexture *m_pMenuMultiplayer;
	CAnimatedTexture *m_pMenuTopscore;
	CAnimatedTexture *m_pMenuExit;
	CAnimatedTexture *m_pCursor;

	CAnimatedTexture *m_pScoreBack;
	CAnimatedTexture *m_pScoreBackground;
	CAnimatedTexture *m_pScoreFont_Text, *m_pScoreFont_Numbers;
	CAnimatedTexture *m_pScoreInputBG;
	CAnimatedTexture *m_pActivePlayer;
	CAnimatedTexture *m_pScoreKeyboard;
	CAnimatedTexture *m_pScoreBackspace, *m_pScoreEnter;

	CAnimatedTexture *m_pSelect;
	CAnimatedTexture *m_pSelectHowMany;

	CAnimatedTexture *m_pLevelMainScreen;
	CAnimatedTexture *m_pLevelInfoBar;
	CAnimatedTexture *m_pLevelGo;
	CAnimatedTexture *m_pLevelCursor;
	float m_fLevelCursorAngle;
	float m_fLevelCursorAlpha;
	bool m_bLevelCursorIncrease;

	bool m_bNext;

	int cursorX, cursorY;
	int cursorXAvg[IR_AVG], cursorYAvg[IR_AVG];

	int state, skipstate;
	void NextState();

	std::vector<StateChange> m_vStates;

	bool PushButton();
	void PrintScore( int pos, std::string szName, int iScore );

	std::vector<LevelSelectOption *> m_vLevelSelectOptions;

	// Score
	std::string m_szNames[10];
	int m_iScores[10];
	void LoadScores();
	void SaveScores();
	void PushKeyboard( int x, int y );
	int m_iNewScores[4];
	std::string m_szInputName;

	int m_iPlayersSelected;
	int m_iSelectedLevel;
	int m_iActivePlayer;
	std::string ReadLine( FILE *pFile );

public:
	CMenuState( int iState = 0, int iScore1 = -1, int iScore2 = -1, int iScore3 = -1, int iScore4 = -1 );
	virtual ~CMenuState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );

	int GetPlayersSelected() { return m_iPlayersSelected; }
	std::string GetSelectedLevel();
};
