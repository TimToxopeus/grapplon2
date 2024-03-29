#pragma once

#include "StateManager.h"
#include "AnimatedTexture.h"
#include "avikit.h"

#include <vector>
#include <string>

// Menu IDs
#define ABMENU 9
#define GAMEMENU 16
#define SCORE 17
#define SCOREINPUT 18
#define PLAYERSELECT 19
#define LEVELSELECT 20
#define TUTORIAL 21
#define CREDITS 26
#define EXIT 27
#define HIGH 25
#define IR_AVG 1 // LEGACY INFRA RED CODE

// Types of state styles
enum StateStyle
{
	INSTANT = 0,
	FADE_IN,
	FADE_OUT,
	MOVE_UP,
	MOVE_UP2,
	PULSE,
};

// State change object
class StateChange
{
public:
	StateChange( int iState, int iSkipState, CAnimatedTexture *pImage, StateStyle eStyle, bool bIncState, int iStayRendered, float fStartAlpha, float fTime, int iStartX, int iStartY, int iGoalX, int iGoalY, int iAnimation = 0 );

	int m_iState, m_iSkipState; // State this belongs to, and the state to skip to on skip button press
	CAnimatedTexture *m_pImage; // Image
	StateStyle m_eStyle;
	bool m_bIncState;			// Increase state counter at end of state change cycle?
	int m_iStayRendered;		// Till which state this object is still rendered

	float m_fAlpha;
	float m_fTime, m_fTimeLeft;

	int m_iX, m_iY;				// Current position
	int m_iStartX, m_iStartY;
	int m_iGoalX, m_iGoalY;

	int m_iAnimation; // Specific image animation

	bool IsClicked( int x, int y );
};

// Level node on the level selection screen
class LevelNode
{
public:
	LevelNode();
	~LevelNode();

	std::string m_szName;
	std::string m_szParent;
	int x, y;
	CAnimatedTexture *m_pImage;
	std::string m_szTarget;
	CAnimatedTexture *m_pInfo;

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
	CAnimatedTexture *m_pMenuRestart;
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

	CAnimatedTexture *m_pTutorialBorder;
	CAnimatedTexture *m_pCredits1;
	CAnimatedTexture *m_pCredits2;

	CAnimatedTexture *m_pLevelInfoBar;
	CAnimatedTexture *m_pLevelGo;
	CAnimatedTexture *m_pLevelCursor;
	float m_fLevelCursorAngle;
	float m_fLevelCursorAlpha;
	bool m_bLevelCursorIncrease;

	AVIKit *m_pAVIKit;
	char m_AVIdata[1024*1024*3];
	long xres, yres;
	float video_duration;
	float video_position;
	int of, f;
	GLuint m_iAVIid;

	bool m_bNext;
	bool m_bReplayable;

	int cursorX, cursorY;
	int cursorXAvg[IR_AVG], cursorYAvg[IR_AVG];

	int state, skipstate;
	void NextState();

	std::vector<StateChange> m_vStates;

	int PushButton();
	void PrintScore( int pos, std::string szName, int iScore );

	std::vector<LevelNode *> m_vLevelNodes;
	int m_iCurrentUniverseIndex;
	int m_iSelectedLevel;
	bool HandleLevelSelect( int x, int y );
	int GetIndexByName( std::string szName );

	// Score
	std::string m_szNames[10];
	int m_iScores[10];
	void LoadScores();
	void SaveScores();
	void PushKeyboard( int x, int y );
	int m_iNewScores[4];
	std::string m_szInputName;

	int m_iPlayersSelected;
	int m_iActivePlayer;
	std::string ReadLine( FILE *pFile );

	void AddCursorXY( int x, int y );
	void HandleButtonPress( wiimote_t* pWiimoteEvent );

public:
	CMenuState( int iState = 0, int iScore1 = -1, int iScore2 = -1, int iScore3 = -1, int iScore4 = -1 );
	virtual ~CMenuState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );

	int GetPlayersSelected() { return m_iPlayersSelected; }
	std::string GetSelectedLevel();

	void AllowReplay() { m_bReplayable = true; }
};
