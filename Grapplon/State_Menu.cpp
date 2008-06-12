#include "State_Menu.h"
#include "SoundManager.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "LogManager.h"
#include "Tokenizer.h"
#include "GameSettings.h"
#include "Vector.h"
#include <math.h>

LevelNode::LevelNode()
{
	m_szName = "";
	m_szParent = "";
	x = 0;
	y = 0;
	m_pImage = NULL;
	m_szTarget = "";
	m_pInfo = NULL;
}

LevelNode::~LevelNode()
{
	if ( m_pImage )
		delete m_pImage;
	if ( m_pInfo )
		delete m_pInfo;
}

bool LevelNode::IsClicked( int x, int y )
{
	if ( this->x == 0 && this->y == 0 )
		return false;

	SDL_Rect target;
	target = m_pImage->GetSize();

	float radius = (float)sqrt((float)(target.w * target.w + target.h * target.h)) + 10;

	target.w += target.w;
	target.h += target.h;
	target.x = -756 + this->x * 2;
	target.y = -650 + this->y * 2;

	Vector v1 = Vector((float)target.x, (float)target.y, 0);
	Vector v2 = Vector((float)x, (float)y, 0);
	Vector v3 = v1 - v2;
	if ( v3.Length() < radius )
		return true;
	return false;

	if ( x < target.x || x > target.x + target.w )
		return false;
	if ( y < target.y || y > target.y + target.h )
		return false;
	return true;
}

StateChange::StateChange( int iState, int iSkipState, CAnimatedTexture *pImage, StateStyle eStyle, bool bIncState, int iStayRendered, float fStartAlpha, float fTime, int iStartX, int iStartY, int iGoalX, int iGoalY, int iAnimation )
{
	m_iState = iState;
	m_iSkipState = iSkipState;
	m_pImage = pImage;
	m_eStyle = eStyle;
	m_bIncState = bIncState;
	m_iStayRendered = iStayRendered;
	m_fAlpha = fStartAlpha;
	m_fTime = m_fTimeLeft = fTime;
	m_iStartX = iStartX;
	m_iStartY = iStartY;
	m_iGoalX = iGoalX;
	m_iGoalY = iGoalY;
	m_iAnimation = iAnimation;
}

bool StateChange::IsClicked( int x, int y )
{
	SDL_Rect target;
	target = m_pImage->GetSize();

	target.w += target.w;
	target.h += target.h;
	target.x = m_iGoalX;
	target.y = m_iGoalY;

	if ( x < target.x || x > target.x + target.w )
		return false;
	if ( y < target.y || y > target.y + target.h )
		return false;
	return true;
}

CMenuState::CMenuState( int iState, int iScore1, int iScore2, int iScore3, int iScore4 )
{
	m_bRunning = true;
	m_bQuit = false;
	m_bNext = false;
	of = -1;
	m_iAVIid = -1;
	m_pAVIKit = NULL;

	for ( int i = 0; i<10; i++ )
	{
		m_szNames[i] = "";
		m_iScores[i] = -1;
	}
	LoadScores();

	m_fLevelCursorAngle = 0.0f;
	m_fLevelCursorAlpha = 0.0f;
	m_bLevelCursorIncrease = true;

	m_iActivePlayer = 1;
	if ( iState == 0 )
	{
		state = 0;
		skipstate = 9;
	}
	else if ( iState == GAMEMENU )
	{
		state = ABMENU + 2;
		skipstate = 16;
	}
	else if ( iState == SCORE )
	{
		state = SCORE;
		skipstate = SCORE;

		m_szInputName = "";
		m_iNewScores[0] = iScore1;
		m_iNewScores[1] = iScore2;
		m_iNewScores[2] = iScore3;
		m_iNewScores[3] = iScore4;

		if ( m_iNewScores[0] > m_iScores[9] || m_iNewScores[1] > m_iScores[9] || m_iNewScores[2] > m_iScores[9] || m_iNewScores[3] > m_iScores[9] )
		{
			if ( m_iNewScores[0] > m_iScores[9] )
				m_iActivePlayer = 1;
			else if ( m_iNewScores[1] > m_iScores[9] )
				m_iActivePlayer = 2;
			else if ( m_iNewScores[2] > m_iScores[9] )
				m_iActivePlayer = 3;
			else if ( m_iNewScores[3] > m_iScores[9] )
				m_iActivePlayer = 4;
			m_szInputName = "player" + itoa2(m_iActivePlayer);
			state = skipstate = SCOREINPUT;
		}
	}

	m_pSplash1 = new CAnimatedTexture("media/scripts/texture_splash1.txt");
	m_pSplash2 = new CAnimatedTexture("media/scripts/texture_splash2.txt");
	m_pSplash3 = new CAnimatedTexture("media/scripts/texture_splash3.txt");
	m_pTitle = new CAnimatedTexture( "media/scripts/texture_title.txt" );

	m_pLogo = new CAnimatedTexture("media/scripts/texture_menu_logo.txt");
	m_pNintendo = new CAnimatedTexture("media/scripts/texture_menu_nintendo.txt");
	m_pAB = new CAnimatedTexture("media/scripts/texture_ab.txt");

	m_pLogo2 = new CAnimatedTexture("media/scripts/texture_menu_logo_small.txt");
	m_pMenuBackground = new CAnimatedTexture("media/scripts/texture_menu_background.txt");
	m_pMenuSingleplayer = new CAnimatedTexture("media/scripts/texture_menu_singleplayer.txt");
	m_pMenuMultiplayer = new CAnimatedTexture("media/scripts/texture_menu_multiplayer.txt");
	m_pMenuTopscore = new CAnimatedTexture("media/scripts/texture_menu_highscore.txt");
	m_pMenuExit = new CAnimatedTexture("media/scripts/texture_menu_exit.txt");

	m_pScoreBack = new CAnimatedTexture("media/scripts/texture_topscores_back.txt");
	m_pScoreBackground = new CAnimatedTexture("media/scripts/texture_topscores_bg.txt");
	m_pScoreFont_Text = new CAnimatedTexture("media/scripts/texture_topscores_text.txt");
	m_pScoreFont_Numbers = new CAnimatedTexture("media/scripts/texture_topscores_numbers.txt");
	m_pScoreKeyboard = new CAnimatedTexture("media/scripts/texture_topscores_keys.txt");
	m_pScoreInputBG = new CAnimatedTexture("media/scripts/texture_nameenter_bg.txt");
	m_pScoreBackspace = new CAnimatedTexture("media/scripts/texture_topscores_backspace.txt");
	m_pScoreEnter = new CAnimatedTexture("media/scripts/texture_topscores_enter.txt");
	m_pActivePlayer = new CAnimatedTexture("media/scripts/texture_player.txt");

	m_pSelect = new CAnimatedTexture("media/scripts/texture_menu_select.txt");
	m_pSelectHowMany = new CAnimatedTexture("media/scripts/texture_menu_select_howmany.txt");

	m_pLevelMainScreen = new CAnimatedTexture("media/scripts/texture_level_main_screen.txt");
	m_pLevelInfoBar = new CAnimatedTexture("media/scripts/texture_level_infobar.txt");
	m_pLevelGo = new CAnimatedTexture("media/scripts/texture_level_go.txt");
	
	m_pCursor = new CAnimatedTexture("media/scripts/texture_cursor.txt");
	m_pLevelCursor = new CAnimatedTexture("media/scripts/texture_level_cursor.txt");

	m_pTutorialBorder = new CAnimatedTexture("media/scripts/texture_controls_bg.txt");

	m_vStates.push_back( StateChange( 0, 2, m_pSplash1, FADE_IN, true, 0, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 1, 2, m_pSplash1, FADE_OUT, true, 1, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 2, 4, m_pSplash2, FADE_IN, true, 2, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 3, 4, m_pSplash2, FADE_OUT, true, 3, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 4, 6, m_pSplash3, FADE_IN, true, 4, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 5, 6, m_pSplash3, FADE_OUT, true, 5, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );

	m_vStates.push_back( StateChange( 6, ABMENU, m_pTitle, FADE_IN, true, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );

	m_vStates.push_back( StateChange( 7, ABMENU, m_pLogo, FADE_IN, true, ABMENU, 1.0f, 2.0f, -746, -286, -746, -286 ) );
	m_vStates.push_back( StateChange( 8, ABMENU, m_pNintendo, FADE_IN, true, HIGH, 1.0f, 2.0f, 664, -768, 664, -768 ) );
	m_vStates.push_back( StateChange( ABMENU, ABMENU, m_pAB, PULSE, false, ABMENU, 0.0f, 1.0f, -450, 67, -450, 67 ) );

	m_vStates.push_back( StateChange( 10, 10, m_pLogo, FADE_OUT, true, 10, 1.0f, 1.0f, -746, -286, -746, -286 ) );
	m_vStates.push_back( StateChange( 10, 10, m_pAB, FADE_OUT, false, 10, 1.0f, 1.0f, -450, 67, -450, 67 ) );

	m_vStates.push_back( StateChange( 11, GAMEMENU, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 11, GAMEMENU, m_pLogo2, MOVE_UP, true, 15, 1.0f, 1.4f, -516, 1000, -516, -366 ) );
	m_vStates.push_back( StateChange( 12, GAMEMENU, m_pMenuSingleplayer, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, -150 ) );
	m_vStates.push_back( StateChange( 13, GAMEMENU, m_pMenuMultiplayer, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, -30 ) );
	m_vStates.push_back( StateChange( 14, GAMEMENU, m_pMenuTopscore, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, 90 ) );
	m_vStates.push_back( StateChange( 15, GAMEMENU, m_pMenuExit, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, 210 ) );

	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pLogo2, INSTANT, false, GAMEMENU, 1.0f, 0.0f, -516, -366, -516, -366 ) );
	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pMenuSingleplayer, INSTANT, false, GAMEMENU, 0.5f, 0.0f, -340, -150, -340, -150 ) );
	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pMenuMultiplayer, INSTANT, false, GAMEMENU, 0.5f, 0.0f, -340, -30, -340, -30 ) );
	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pMenuTopscore, INSTANT, false, GAMEMENU, 0.5f, 0.0f, -340, 90, -340, 90 ) );
	m_vStates.push_back( StateChange( GAMEMENU, GAMEMENU, m_pMenuExit, INSTANT, false, GAMEMENU, 0.5f, 0.0f, -340, 210, -340, 210 ) );

	m_vStates.push_back( StateChange( SCORE, SCORE, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( SCORE, SCORE, m_pScoreBackground, INSTANT, false, SCORE, 1.0f, 0.0f, -488, -416, -488, -416 ) );
	m_vStates.push_back( StateChange( SCORE, SCORE, m_pScoreBack, INSTANT, false, SCORE, 0.5f, 0.0f, -150, 448, -150, 448 ) );

	m_vStates.push_back( StateChange( SCOREINPUT, SCOREINPUT, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( SCOREINPUT, SCOREINPUT, m_pScoreInputBG, INSTANT, false, SCOREINPUT, 1.0f, 0.0f, -797, -352, -797, -352 ) );
	m_vStates.push_back( StateChange( SCOREINPUT, SCOREINPUT, m_pScoreBackspace, INSTANT, false, SCOREINPUT, 1.0f, 0.0f, -640, 225, -640, 225 ) );
	m_vStates.push_back( StateChange( SCOREINPUT, SCOREINPUT, m_pScoreEnter, INSTANT, false, SCOREINPUT, 1.0f, 0.0f, 364, 225, 364, 225 ) );

	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pSelectHowMany, INSTANT, false, PLAYERSELECT, 1.0f, 0.0f, -484, -400, -484, -400, 0 ) );
	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pSelect, INSTANT, false, PLAYERSELECT, 1.0f, 0.0f, -875, -250, -875, -250, 0 ) );
	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pSelect, INSTANT, false, PLAYERSELECT, 1.0f, 0.0f, -275, -250, -275, -250, 1 ) );
	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pSelect, INSTANT, false, PLAYERSELECT, 1.0f, 0.0f, 325, -250, 325, -250, 2 ) );
	m_vStates.push_back( StateChange( PLAYERSELECT, PLAYERSELECT, m_pScoreBack, INSTANT, false, PLAYERSELECT, 0.5f, 0.0f, -150, 448, -150, 448 ) );

	m_vStates.push_back( StateChange( LEVELSELECT, LEVELSELECT, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
//	m_vStates.push_back( StateChange( LEVELSELECT, LEVELSELECT, m_pLevelMainScreen, INSTANT, false, LEVELSELECT, 1.0f, 0.0f, -756, -650, -756, -650, 0 ) );
	m_vStates.push_back( StateChange( LEVELSELECT, LEVELSELECT, m_pLevelInfoBar, INSTANT, false, LEVELSELECT, 1.0f, 0.0f, -756, 250, -756, 250, 0 ) );
	m_vStates.push_back( StateChange( LEVELSELECT, LEVELSELECT, m_pScoreBack, INSTANT, false, LEVELSELECT, 0.5f, 0.0f, 150, 540, 150, 540 ) );
	m_vStates.push_back( StateChange( LEVELSELECT, LEVELSELECT, m_pLevelGo, INSTANT, false, LEVELSELECT, 0.5f, 0.0f, 470, 540, 470, 540 ) );

	m_vStates.push_back( StateChange( TUTORIAL, TUTORIAL, m_pTitle, INSTANT, false, HIGH, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( TUTORIAL, TUTORIAL, m_pTutorialBorder, INSTANT, false, TUTORIAL, 1.0f, 0.0f, -885, -560, -885, -560 ) );
	m_vStates.push_back( StateChange( TUTORIAL, TUTORIAL, m_pScoreBack, INSTANT, false, TUTORIAL, 0.5f, 0.0f, -150, 480, -150, 480 ) );

	m_pAVIKit = new AVIKit("media/controls.avi", false);
	if ( m_pAVIKit )
	{
		if ( m_pAVIKit->getError(NULL) == AVIKIT_NOERROR )
		{
			m_pAVIKit->getVideoInfo(&xres, &yres, &video_duration);
		}
		else
		{
			delete m_pAVIKit;
			m_pAVIKit = NULL;
		}
	}

	m_iSelectedLevel = -1;
	FILE *pFile = fopen("media/scripts/universe.txt", "rt");
	if ( pFile )
	{
		char in[256];
		std::string input;
		CTokenizer tokenizer;
		std::vector<std::string> tokens;
		while ( !feof(pFile) )
		{
			fgets(in, 256, pFile );
			int len = strlen(in);
			if ( in[len-1] == '\n' )
				in[len-1] = 0;
			input = in;
			tokens = tokenizer.GetTokens(input);

			if ( tokens.size() == 7 )
			{
				LevelNode *pNode = new LevelNode();
				pNode->m_szName = tokens[0];
				pNode->m_szParent = tokens[1];
				pNode->x = atoi(tokens[2].c_str());
				pNode->y = atoi(tokens[3].c_str());
				if ( tokens[4] != "null" )
					pNode->m_pImage = new CAnimatedTexture( "media/scripts/" + tokens[4] + ".txt" );
				pNode->m_szTarget = tokens[5];
				if ( tokens[6] != "null" )
					pNode->m_pInfo = new CAnimatedTexture( "media/scripts/" + tokens[6] + ".txt" );
				m_vLevelNodes.push_back( pNode );
			}
			else if ( tokens.size() > 0 )
			{
				CLogManager::Instance()->LogMessage("Incorrect amount of parameters in universe.txt : " + input);
			}
		}
	}
	m_iCurrentUniverseIndex = 0;

	for ( int i = 0; i<IR_AVG; i++ )
	{
		cursorXAvg[i] = SDL_GetCursor()->hot_x;
		cursorYAvg[i] = SDL_GetCursor()->hot_y;
	}
	cursorX = SDL_GetCursor()->hot_x;
	cursorY = SDL_GetCursor()->hot_y;

	CSoundManager::Instance()->LoadSound( "media/music/music_menu.ogg" );
}

CMenuState::~CMenuState()
{
	delete m_pSplash1;
	delete m_pSplash2;
	delete m_pSplash3;
	delete m_pTitle;

	delete m_pLogo;
	delete m_pNintendo;
	delete m_pAB;

	delete m_pLogo2;
	delete m_pMenuBackground;
	delete m_pMenuSingleplayer;
	delete m_pMenuMultiplayer;
	delete m_pMenuTopscore;
	delete m_pMenuExit;
	delete m_pCursor;

	delete m_pScoreBack;
	delete m_pScoreBackground;
	delete m_pScoreFont_Text;
	delete m_pScoreFont_Numbers;
	delete m_pScoreKeyboard;
	delete m_pScoreInputBG;
	delete m_pScoreBackspace;
	delete m_pScoreEnter;
	delete m_pActivePlayer;

	delete m_pSelect;
	delete m_pSelectHowMany;

	delete m_pTutorialBorder;

	delete m_pLevelMainScreen;
	delete m_pLevelInfoBar;
	delete m_pLevelGo;
	delete m_pLevelCursor;

	if ( m_pAVIKit )
		delete m_pAVIKit;

	for ( unsigned int i = 0; i<m_vLevelNodes.size(); i++ )
		delete m_vLevelNodes[i];
	m_vLevelNodes.clear();
}

void CMenuState::Render()
{
	SDL_Rect target;
	for ( unsigned int a = 0; a<m_vStates.size(); a++ )
	{
		if ( m_vStates[a].m_iState == state || (m_vStates[a].m_iState < state && m_vStates[a].m_iStayRendered >= state) )
		{
			target = m_vStates[a].m_pImage->GetSize();
			target.w += target.w;
			target.h += target.h;
			target.x = m_vStates[a].m_iX;
			target.y = m_vStates[a].m_iY;

			if ( m_vStates[a].m_pImage != m_pScoreBackspace && m_vStates[a].m_pImage != m_pScoreEnter )
				m_vStates[a].m_pImage->SetAnimation( m_vStates[a].m_iAnimation );
			RenderQuad( target, m_vStates[a].m_pImage, 0, m_vStates[a].m_fAlpha );
		}
	}

	if ( state == SCORE )
	{
		for ( int i = 0; i<10; i++ )
		{
			if ( m_iScores[i] > 0 && m_szNames[i] != "" )
			PrintScore( i, m_szNames[i], m_iScores[i] );
		}
	}

	if ( state == SCOREINPUT )
	{
		m_pActivePlayer->SetAnimation( m_iActivePlayer - 1 );
		target = m_pActivePlayer->GetSize();
		target.w += target.w;
		target.h += target.h;
		target.x = -600;
		target.y = -313;
		RenderQuad( target, m_pActivePlayer, 0 );

		target.w = target.h = 90;
		int icursorX = (cursorX * 2 - 1024) - 24;
		int icursorY = (cursorY * 2 - 768) - 24;
		for ( int i = 0; i<3; i++ )
		{
			for ( int j = 0; j<10; j++ )
			{
				target.x = -640 + j * 130;
				target.y = -100 + i * 110;

				if ( icursorX > target.x && icursorX < target.x + target.w && icursorY > target.y && icursorY < target.y + target.h )
					m_pScoreKeyboard->SetAnimation(1);
				else
					m_pScoreKeyboard->SetAnimation(0);
				m_pScoreKeyboard->SetFrame( j + i * 10 );

				RenderQuad( target, m_pScoreKeyboard, 0 );
			}
		}

		target.x = -176;
		target.w = target.h = 48;
		target.y = -170;

		SDL_Rect underscore;
		underscore.y = -170 + 41;
		underscore.w = 32;
		underscore.h = 7;

		SDL_Color blue;
		blue.r = 73;
		blue.g = 50;
		blue.b = 244;

		for ( unsigned int i = 0; i<7; i++ )
		{
			if ( i < m_szInputName.length() )
			{
				int frame = m_szInputName[i] - 97;
				if ( frame < 0 )
				{
					if ( m_szInputName[i] == ',' )
						frame = 26;
					if ( m_szInputName[i] == '.' )
						frame = 27;
					if ( m_szInputName[i] == '-' )
						frame = 28;
					if ( m_szInputName[i] == '!' )
						frame = 29;
				}
				m_pScoreFont_Text->SetFrame( frame );
				RenderQuad( target, m_pScoreFont_Text, 0, 1 );
			}
			else
			{
				underscore.x = target.x + 8;
				RenderQuad( underscore, NULL, 0, blue, 1 );
			}
			target.x += 56;
		}
	}

	if ( state == LEVELSELECT )
	{
/*		for ( unsigned int i = 0; i<m_vLevelSelectOptions.size(); i++ )
		{
			target = m_vLevelSelectOptions[i]->m_pImage->GetSize();
			target.x = -756 + m_vLevelSelectOptions[i]->x * 2 - target.w;
			target.y = -650 + m_vLevelSelectOptions[i]->y * 2 - target.h;
			target.w += target.w;
			target.h += target.h;
			RenderQuad( target, m_vLevelSelectOptions[i]->m_pImage, 0, 1 );
		}
		
		if ( m_iSelectedLevel != -1 )
		{
			target = m_pLevelCursor->GetSize();
			target.x = -756 + m_vLevelSelectOptions[m_iSelectedLevel]->x * 2 - target.w;
			target.y = -650 + m_vLevelSelectOptions[m_iSelectedLevel]->y * 2 - target.h;
			target.w += target.w;
			target.h += target.h;
			RenderQuad( target, m_pLevelCursor, m_fLevelCursorAngle, m_fLevelCursorAlpha );

			target = m_vLevelSelectOptions[m_iSelectedLevel]->m_pInfoText->GetSize();
			target.x = -756;
			target.y = 250;
			target.w += target.w;
			target.h += target.h;
			RenderQuad( target, m_vLevelSelectOptions[m_iSelectedLevel]->m_pInfoText, 0, 1 );
		}
*/

		for ( unsigned int i = 0; i<m_vLevelNodes.size(); i++ )
		{
			if ( (m_vLevelNodes[i]->m_szParent == m_vLevelNodes[m_iCurrentUniverseIndex]->m_szName &&
				m_vLevelNodes[i]->x != 0 && m_vLevelNodes[i]->y != 0) || i == m_iCurrentUniverseIndex )
			{
				target = m_vLevelNodes[i]->m_pImage->GetSize();
				target.x = -756;
				target.y = -650;
				if ( m_vLevelNodes[i]->x != 0 && m_vLevelNodes[i]->y != 0 )
				{
					target.x += m_vLevelNodes[i]->x * 2 - target.w;
					target.y += m_vLevelNodes[i]->y * 2 - target.h;
				}
				target.w += target.w;
				target.h += target.h;
				RenderQuad( target, m_vLevelNodes[i]->m_pImage, 0, 1 );
			}
		}

		if ( m_iSelectedLevel != -1 )
		{
			target = m_pLevelCursor->GetSize();
			target.x = -756 + m_vLevelNodes[m_iSelectedLevel]->x * 2 - target.w;
			target.y = -650 + m_vLevelNodes[m_iSelectedLevel]->y * 2 - target.h;
			target.w += target.w;
			target.h += target.h;
			RenderQuad( target, m_pLevelCursor, m_fLevelCursorAngle, m_fLevelCursorAlpha );

			target = m_vLevelNodes[m_iSelectedLevel]->m_pInfo->GetSize();
			target.x = -756;
			target.y = 250;
			target.w += target.w;
			target.h += target.h;
			RenderQuad( target, m_vLevelNodes[m_iSelectedLevel]->m_pInfo, 0, 1 );
		}
	}

	if ( state == TUTORIAL  )
	{
		if ( m_pAVIKit )
		{
			// update the current frame, if necessary
			glBindTexture( GL_TEXTURE_2D, m_iAVIid );
			if (f != of)
			{
				m_pAVIKit->getVideoFrame(m_AVIdata, f);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xres, yres, GL_RGB, GL_UNSIGNED_BYTE, m_AVIdata);
				of = f;
			}

			Coords texture;
			texture.x = texture.y = 0;
			texture.w = texture.h = 1;

			int w = xres;
			int h = yres;

			glTranslatef( 0.0f, -24.0f, 0.0f );

			// Draw the quad
			glBegin(GL_QUADS);
				// Top left corner
				glTexCoord2f( texture.x, texture.y );
				glVertex3f((float)-w, (float)h, 0.0f);

				// Bottom left corner
				glTexCoord2f( texture.x, texture.y + texture.h );
				glVertex3f((float)-w, (float)-h, 0.0f);

				// Bottom right corner
				glTexCoord2f( texture.x + texture.w, texture.y + texture.h );
				glVertex3f((float)w, (float)-h, 0.0f);

				// Top right corner
				glTexCoord2f( texture.x + texture.w, texture.y );
				glVertex3f((float)w, (float)h, 0.0f);
			glEnd();

			glTranslatef( 0.0f, 24.0f, 0.0f );
		}
	}

//	if ( state == GAMEMENU || state == SCORE || state == SCOREINPUT || state == PLAYERSELECT || state == LEVELSELECT )
//	{
		target = m_pCursor->GetSize();
		target.w += target.w;
		target.h += target.h;
		int icursorX = (cursorX * 2 - 1024) + 20;
		int icursorY = (cursorY * 2 - 768) + 20;
		target.x = icursorX - (target.w / 2);
		target.y = icursorY - (target.h / 2);
		RenderQuad( target, m_pCursor, 0, 1 );
//	}
}

void CMenuState::Update(float fTime)
{
	video_position += fTime;
	if ( video_position > video_duration )
		video_position = 0;
	if ( m_pAVIKit )
		f = m_pAVIKit->getVideoFrameNumber(video_position);

	m_fLevelCursorAngle += 180.0f * fTime;
	if ( m_fLevelCursorAngle >= 360.0f )
		m_fLevelCursorAngle -= 360.0f;

	if ( m_bLevelCursorIncrease )
	{
		m_fLevelCursorAlpha += 2.0f * fTime;
		if ( m_fLevelCursorAlpha >= 1.0f )
		{
			m_fLevelCursorAlpha = 1.0f;
			m_bLevelCursorIncrease = false;
		}
	}
	else
	{
		m_fLevelCursorAlpha -= 2.0f * fTime;
		if ( m_fLevelCursorAlpha <= 0.0f )
		{
			m_fLevelCursorAlpha = 0.0f;
			m_bLevelCursorIncrease = true;
		}
	}

	if ( m_bNext && state <= GAMEMENU )
	{
		NextState();
		m_bNext = false;
	}
	for ( unsigned int a = 0; a<m_vStates.size(); a++ )
	{
		if ( m_vStates[a].m_iState == state )
		{
			skipstate = m_vStates[a].m_iSkipState;

			m_vStates[a].m_fTimeLeft -= fTime;
			float ratio = m_vStates[a].m_fTimeLeft / m_vStates[a].m_fTime;

			float ratioSquared = ratio * ratio;
			int dX = m_vStates[a].m_iStartX - m_vStates[a].m_iGoalX;
			int dY = m_vStates[a].m_iStartY - m_vStates[a].m_iGoalY;
			m_vStates[a].m_iX = m_vStates[a].m_iGoalX + (int)(dX * ratioSquared);
			m_vStates[a].m_iY = m_vStates[a].m_iGoalY + (int)(dY * ratioSquared);

			if ( m_vStates[a].m_eStyle == FADE_IN )
				m_vStates[a].m_fAlpha = 1.0f - ratio;
			if ( m_vStates[a].m_eStyle == FADE_OUT )
				m_vStates[a].m_fAlpha = ratio;
			if ( m_vStates[a].m_eStyle == PULSE )
			{
				if ( ratio < 0.5f )
					m_vStates[a].m_fAlpha = 0.5f + 0.5f * (ratio * 2);
				else
					m_vStates[a].m_fAlpha = 0.5f + 0.5f * (1.0f - ((ratio - 0.5f) * 2));
			}

			if ( m_vStates[a].m_fTimeLeft <= 0.0f )
			{
				if ( m_vStates[a].m_bIncState )
					state++;

				if ( m_vStates[a].m_eStyle == PULSE )
					m_vStates[a].m_fTimeLeft = m_vStates[a].m_fTime;
			}

			if ( state == GAMEMENU )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				if ( m_vStates[a].m_pImage != m_pLogo2 )
				{
					// Dealing with a button
					if ( icursorX > m_vStates[a].m_iStartX && icursorX < m_vStates[a].m_iStartX + m_vStates[a].m_pImage->GetSize().w * 2 )
					{
						if ( icursorY > m_vStates[a].m_iStartY && icursorY < m_vStates[a].m_iStartY + m_vStates[a].m_pImage->GetSize().h * 2 )
						{
							if ( m_vStates[a].m_fAlpha != 1.0f )
							{
								m_vStates[a].m_fAlpha = 1.0f;
								CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseon.wav", RT_SOUND);
								if ( pSound )
									pSound->Play(true);
							}
						}
						else
							m_vStates[a].m_fAlpha = 0.5f;
					}
					else
						m_vStates[a].m_fAlpha = 0.5f;
				}
			}
			if ( state == SCORE || state == TUTORIAL )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				if ( m_vStates[a].m_pImage == m_pScoreBack )
				{
					if ( icursorX > m_vStates[a].m_iStartX && icursorX < m_vStates[a].m_iStartX + m_vStates[a].m_pImage->GetSize().w * 2 )
					{
						if ( icursorY > m_vStates[a].m_iStartY && icursorY < m_vStates[a].m_iStartY + m_vStates[a].m_pImage->GetSize().h * 2 )
						{
							if ( m_vStates[a].m_fAlpha != 1.0f )
							{
								m_vStates[a].m_fAlpha = 1.0f;
								CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseon.wav", RT_SOUND);
								if ( pSound )
									pSound->Play(true);
							}
						}
						else
							m_vStates[a].m_fAlpha = 0.5f;
					}
					else
						m_vStates[a].m_fAlpha = 0.5f;
				}
			}
			if ( state == SCOREINPUT )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				if ( m_vStates[a].m_pImage == m_pScoreBackspace || m_vStates[a].m_pImage == m_pScoreEnter )
				{
					if ( icursorX > m_vStates[a].m_iStartX && icursorX < m_vStates[a].m_iStartX + m_vStates[a].m_pImage->GetSize().w * 2 )
					{
						if ( icursorY > m_vStates[a].m_iStartY && icursorY < m_vStates[a].m_iStartY + m_vStates[a].m_pImage->GetSize().h * 2 )
						{
							int frame = m_vStates[a].m_pImage->GetFrame();
							if ( frame != 1 )
							{
								m_vStates[a].m_pImage->SetFrame(1);
								CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseon.wav", RT_SOUND);
								if ( pSound )
									pSound->Play(true);
							}
						}
						else
							m_vStates[a].m_pImage->SetFrame(0);
					}
					else
						m_vStates[a].m_pImage->SetFrame(0);
				}
			}
			if ( state == PLAYERSELECT )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				if ( m_vStates[a].m_pImage == m_pScoreBack || m_vStates[a].m_pImage == m_pSelect )
				{
					if ( icursorX > m_vStates[a].m_iStartX && icursorX < m_vStates[a].m_iStartX + m_vStates[a].m_pImage->GetSize().w * 2 )
					{
						if ( icursorY > m_vStates[a].m_iStartY && icursorY < m_vStates[a].m_iStartY + m_vStates[a].m_pImage->GetSize().h * 2 )
						{
							if ( m_vStates[a].m_fAlpha != 1.0f )
							{
								m_vStates[a].m_fAlpha = 1.0f;
								CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseon.wav", RT_SOUND);
								if ( pSound )
									pSound->Play(true);
							}
						}
						else
							m_vStates[a].m_fAlpha = 0.5f;
					}
					else
						m_vStates[a].m_fAlpha = 0.5f;
				}
			}
			if ( state == LEVELSELECT )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				if ( m_vStates[a].m_pImage == m_pScoreBack || m_vStates[a].m_pImage == m_pLevelGo )
				{
					if ( icursorX > m_vStates[a].m_iStartX && icursorX < m_vStates[a].m_iStartX + m_vStates[a].m_pImage->GetSize().w * 2 )
					{
						if ( icursorY > m_vStates[a].m_iStartY && icursorY < m_vStates[a].m_iStartY + m_vStates[a].m_pImage->GetSize().h * 2 )
						{
							if ( m_vStates[a].m_fAlpha != 1.0f )
							{
								m_vStates[a].m_fAlpha = 1.0f;
								CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseon.wav", RT_SOUND);
								if ( pSound )
									pSound->Play(true);
							}
						}
						else
							m_vStates[a].m_fAlpha = 0.5f;
					}
					else
						m_vStates[a].m_fAlpha = 0.5f;
				}
			}
		}
	}

	for ( unsigned int i = 0; i<m_vLevelNodes.size(); i++ )
	{
		m_vLevelNodes[i]->m_pImage->UpdateFrame( fTime );
	}
}

bool CMenuState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if ( WIIUSE_USING_IR( pWiimoteEvent ) )
		{
			if ( (state != SCOREINPUT && pWiimoteEvent->unid == m_iActivePlayer) ||
				 (state == SCOREINPUT && pWiimoteEvent->unid == m_iActivePlayer) )
			{
				if ( pWiimoteEvent->ir.x != 0 && pWiimoteEvent->ir.y != 0 )
					AddCursorXY( pWiimoteEvent->ir.x, pWiimoteEvent->ir.y );
			}
		}
		if ( pWiimoteEvent->exp.type == EXP_NUNCHUK )
		{
			HandleButtonPress( pWiimoteEvent );
			if ( (state != SCOREINPUT && pWiimoteEvent->unid == m_iActivePlayer) ||
				 (state == SCOREINPUT && pWiimoteEvent->unid == m_iActivePlayer) )
			{
				if ( pWiimoteEvent->exp.nunchuk.js.mag > 0.05f )
				{
					int icursorX = (cursorX * 2 - 1024);
					int icursorY = (cursorY * 2 - 768);
					Vector v = Vector((float)icursorX, (float)icursorY, 0);
					float angle = (pWiimoteEvent->exp.nunchuk.js.ang - 90.0f)*(3.14f/180.0f);
					v += Vector( cos(angle), sin(angle), 0.0f );
					v -= Vector((float)icursorX, (float)icursorY, 0);
					v.Normalize();
					v *= 15.0f * pWiimoteEvent->exp.nunchuk.js.mag;
					int cx = (int)(cursorX + v[0]);
					int cy = (int)(cursorY + v[1]);
					AddCursorXY( cx, cy );
				}
			}
		}
		else
		{
			HandleButtonPress( pWiimoteEvent );
		}
	}
	return false;
}

int CMenuState::HandleSDLEvent(SDL_Event event)
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
		else if ( event.key.keysym.sym == SDLK_p )
		{
			state = ABMENU + 1;
		}
		else
			NextState();
	}
	if ( event.type == SDL_MOUSEBUTTONUP )
	{
		if ( state < GAMEMENU )
		{
			NextState();
		}
		else if ( state < SCOREINPUT || state == PLAYERSELECT || state == LEVELSELECT || state == TUTORIAL )
		{
			bool pushed = PushButton();
			if ( pushed )
			{
				CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseclick.wav", RT_SOUND);
				if ( pSound )
					pSound->Play(true);
			}
			if ( state == LEVELSELECT && !pushed )
			{
//				m_iSelectedLevel = -1;
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				HandleLevelSelect( icursorX, icursorY );
			}
		}
		else
		{
			int icursorX = (cursorX * 2 - 1024);
			int icursorY = (cursorY * 2 - 768);
			PushKeyboard( icursorX, icursorY );
		}
	}
	if ( event.type == SDL_MOUSEMOTION )
	{
		cursorX = event.motion.x;// * 2 - 1024;
		cursorY = event.motion.y;// * 2 - 768;
		for ( int i = 0; i < IR_AVG; i++ )
		{
			this->cursorXAvg[i] = event.motion.x;
			this->cursorYAvg[i] = event.motion.y;
		}
	}
	return 0;
}

void CMenuState::NextState()
{
	state = skipstate;

	for ( unsigned i = 0; i<m_vStates.size(); i++ )
	{
		if ( m_vStates[i].m_iState < state && (m_vStates[i].m_iStayRendered >= state) )
		{
			if ( m_vStates[i].m_eStyle == FADE_IN )
				m_vStates[i].m_fAlpha = 1.0f;
			if ( m_vStates[i].m_eStyle == FADE_OUT )
				m_vStates[i].m_fAlpha = 0.0f;

			m_vStates[i].m_iX = m_vStates[i].m_iGoalX;
			m_vStates[i].m_iY = m_vStates[i].m_iGoalY;
		}
	}
}

bool CMenuState::PushButton()
{
	int newState = state;
	int icursorX = (cursorX * 2 - 1024);
	int icursorY = (cursorY * 2 - 768);
	for ( unsigned int i = 0; i < m_vStates.size(); i++ )
	{
		if ( m_vStates[i].IsClicked(icursorX, icursorY) )
		{
			if ( m_vStates[i].m_pImage == m_pMenuSingleplayer && state == GAMEMENU )
			{
				//m_bRunning = false;
				newState = TUTORIAL;
				m_iActivePlayer = 1;
				if ( m_iAVIid == -1 )
				{
					GLint nOfColors = 4;
					GLenum texture_format;

					// get the number of channels in the SDL surface
					texture_format = GL_RGBA;

					// Have OpenGL generate a texture object handle for us
					glGenTextures( 1, &m_iAVIid );

					// Bind the texture object
					glBindTexture( GL_TEXTURE_2D, m_iAVIid );

					// Set the texture's stretching properties
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

					// Edit the texture object's image data using the information SDL_Surface gives us
					glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, xres, yres, 0,
									texture_format, GL_UNSIGNED_BYTE, NULL );
				}
			}
			if ( m_vStates[i].m_pImage == m_pMenuMultiplayer && state == GAMEMENU )
			{
				//m_bRunning = false;
				newState = PLAYERSELECT;
				m_iActivePlayer = 1;
			}
			if ( m_vStates[i].m_pImage == m_pMenuTopscore && newState == state && state == GAMEMENU )
			{
				newState = SCORE;
				m_iActivePlayer = 1;
			}
			if ( m_vStates[i].m_pImage == m_pMenuExit && state == GAMEMENU )
			{
				m_bRunning = false;
				m_bQuit = true;
				return true;
			}
			if ( m_vStates[i].m_pImage == m_pScoreBack && newState == state && (state == SCORE || state == PLAYERSELECT || state == TUTORIAL) )
			{
				newState = GAMEMENU;
				m_iActivePlayer = 1;
			}
			if ( m_vStates[i].m_pImage == m_pScoreBack && newState == state && state == LEVELSELECT )
			{
				newState = PLAYERSELECT;
				m_iActivePlayer = 1;
			}
			if ( m_vStates[i].m_pImage == m_pLevelGo && newState == state && state == LEVELSELECT )
			{
				if ( m_iSelectedLevel != -1 )
				{
					m_bRunning = false;
					return true;
				}
			}
			if ( m_vStates[i].m_pImage == m_pSelect && state == PLAYERSELECT )
			{
				m_iPlayersSelected = m_vStates[i].m_iAnimation + 2;
				newState = LEVELSELECT;
				m_iActivePlayer = 1;
			}
		}
	}
	if ( state == newState )
		return false;

	state = newState;
	return true;
}

void CMenuState::PrintScore( int pos, std::string szName, int iScore )
{
	SDL_Rect target;
	target.x = -360;
	target.w = target.h = 48;
	target.y = -170 + pos * 52;

	for ( unsigned int i = 0; i<szName.length(); i++ )
	{
		int frame = szName[i] - 97;
		if ( frame < 0 )
		{
			if ( szName[i] == ',' )
				frame = 26;
			if ( szName[i] == '.' )
				frame = 27;
			if ( szName[i] == '-' )
				frame = 28;
			if ( szName[i] == '!' )
				frame = 29;
		}
		m_pScoreFont_Text->SetFrame( frame );
		RenderQuad( target, m_pScoreFont_Text, 0, 1 );
		target.x += 56;
	}

	target.x = 385;
	std::string szScore = itoa2(iScore);

	int l = (int)szScore.length();
	for ( int a = l - 1; a >= 0; a-- )
	{
		unsigned int v = (unsigned int)(szScore[a] - 48);
		m_pScoreFont_Numbers->SetFrame( v );
		RenderQuad( target, m_pScoreFont_Numbers, 0, 1 );
		target.x -= 56;
	}
}

void CMenuState::LoadScores()
{
	FILE *pFile = fopen( "topscores.txt", "rt" );
	if ( !pFile )
		return;

	CTokenizer tokenizer;
	std::vector<std::string> tokens;

	int i = 0;
	std::string in = ReadLine(pFile);
	while ( in != "" && i <= 9 )
	{
		tokens = tokenizer.GetTokens( in );
		m_szNames[i] = tokens[0];
		m_iScores[i] = atoi(tokens[1].c_str());
		i++;
		in = ReadLine( pFile );
	}

	fclose( pFile );
}

void CMenuState::SaveScores()
{
	FILE *pFile = fopen( "topscores.txt", "wt+" );
	if ( !pFile )
		return;

	for ( int i = 0; i<10; i++ )
	{
		if ( m_iScores[i] > 0 )
			fprintf( pFile, "%s %d\n", m_szNames[i].c_str(), m_iScores[i] );
	}

	fclose( pFile );
}

std::string CMenuState::ReadLine( FILE *pFile )
{
	if ( !pFile || feof(pFile) )
		return "";

	char input[1024];
	fgets( input, 1024, pFile );
	if ( feof(pFile) )
		return "";
	int len = strlen(input);
	if ( len > 0 )
		input[len - 1] = 0; // Cut off the \n
	return std::string(input);
}

void CMenuState::PushKeyboard( int x, int y )
{
	SDL_Rect target;
	target.w = target.h = 90;
	int icursorX = (cursorX * 2 - 1024) - 24;
	int icursorY = (cursorY * 2 - 768) - 24;
	if ( m_szInputName.length() < 7 )
	{
		for ( int i = 0; i<3; i++ )
		{
			for ( int j = 0; j<10; j++ )
			{
				target.x = -640 + j * 130;
				target.y = -100 + i * 110;
				if ( icursorX > target.x && icursorX < target.x + target.w && icursorY > target.y && icursorY < target.y + target.h )
				{
					int t = j + i * 10;
					char tokens[31] = "qwertyuiopasdfghjkl!zxcvbnm,.-";
					m_szInputName += tokens[t];
					CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseclick.wav", RT_SOUND);
					if ( pSound )
						pSound->Play(true);
					return;
				}
			}
		}
	}

	if ( icursorX > -640 && icursorX < -248 && icursorY > 225 && icursorY < 317 )
	{
		m_szInputName = m_szInputName.substr(0, m_szInputName.length()-1);
		CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseclick.wav", RT_SOUND);
		if ( pSound )
			pSound->Play(true);
	}
	if ( icursorX > 364 && icursorX < 620 && icursorY > 225 && icursorY < 317 )
	{
		if ( m_szInputName.length() != 0 )
		{
			// Add to text file:
			int i = 9;
			while ( m_iScores[i - 1] < m_iNewScores[m_iActivePlayer - 1] && i > 0)
			{
				m_iScores[i] = m_iScores[i - 1];
				m_szNames[i] = m_szNames[i - 1];
				i--;
			}
			m_iScores[i] = m_iNewScores[m_iActivePlayer - 1];
			m_szNames[i] = m_szInputName;
			SaveScores();
		}

		m_iNewScores[m_iActivePlayer - 1] = -1;
		if ( m_iNewScores[0] > m_iScores[9] || m_iNewScores[1] > m_iScores[9] || m_iNewScores[2] > m_iScores[9] || m_iNewScores[3] > m_iScores[9] )
		{
			if ( m_iNewScores[0] > m_iScores[9] )
				m_iActivePlayer = 1;
			else if ( m_iNewScores[1] > m_iScores[9] )
				m_iActivePlayer = 2;
			else if ( m_iNewScores[2] > m_iScores[9] )
				m_iActivePlayer = 3;
			else if ( m_iNewScores[3] > m_iScores[9] )
				m_iActivePlayer = 4;
			state = skipstate = SCOREINPUT;
			m_szInputName = "";
		}
		else
			state = skipstate = SCORE;
	}
}

std::string CMenuState::GetSelectedLevel()
{
	if ( m_iSelectedLevel < 0 )
		return SETS->LEVEL;
	return "media/scripts/" + m_vLevelNodes[m_iSelectedLevel]->m_szTarget + ".txt";
}

void CMenuState::AddCursorXY( int x, int y )
{
	if ( x < 0 || x > 1024 || y < 0 || y > 768 )
		return;

	// Check if the new value doesn't crazily exceed old average
	int deltaX = abs(cursorX - x);
	int deltaY = abs(cursorY - y);

	if ( deltaX < 512 && deltaY < 384 )
	{
		for ( int i = 0; i<(IR_AVG - 1); i++ )
		{
			cursorXAvg[i] = cursorXAvg[i + 1];
			cursorYAvg[i] = cursorYAvg[i + 1];
		}

		cursorXAvg[IR_AVG - 1] = x;
		cursorYAvg[IR_AVG - 1] = y;

		cursorX = 0; cursorY = 0;
		for ( int i = 0; i < IR_AVG; i++ )
		{
			cursorX += cursorXAvg[i];
			cursorY += cursorYAvg[i];
		}

		cursorX /= IR_AVG;
		cursorY /= IR_AVG;

		SDL_WarpMouse( cursorX, cursorY );
	}
}

void CMenuState::HandleButtonPress( wiimote_t* pWiimoteEvent )
{
	if ( state == ABMENU )
	{
		if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) && IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) ||
			 IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) && IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A))
			state++;
	}
	else if ( state < GAMEMENU )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) ||
			IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
			m_bNext = true;
	}
	else if ( state == SCOREINPUT )
	{
		if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) )
		{
			if ( pWiimoteEvent->unid == m_iActivePlayer )
			{
				int icursorX = (cursorX * 2 - 1024);
				int icursorY = (cursorY * 2 - 768);
				PushKeyboard( icursorX, icursorY );
			}
		}
		if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) )
		{
			m_szInputName = m_szInputName.substr(0, m_szInputName.length()-1);
		}
	}

	if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) ||
		IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
	{
		m_pCursor->SetAnimation(1);

		bool pushed = PushButton();
		if ( pushed )
		{
			CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Menu_button_mouseclick.wav", RT_SOUND);
			if ( pSound )
				pSound->Play(true);
		}
		if ( state == LEVELSELECT && !pushed && !IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) )
		{
//			m_iSelectedLevel = -1;
			int icursorX = (cursorX * 2 - 1024);
			int icursorY = (cursorY * 2 - 768);
			HandleLevelSelect( icursorX, icursorY );
		}
		if ( state == LEVELSELECT && IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) )
		{
			if ( m_vLevelNodes[m_iCurrentUniverseIndex]->m_szParent != "null" )
			{
				int index = GetIndexByName( m_vLevelNodes[m_iCurrentUniverseIndex]->m_szParent );
				if ( index != -1 )
					m_iCurrentUniverseIndex = index;
			}
		}
	}

	if ( !IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) && !IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
		m_pCursor->SetAnimation(0);
}

bool CMenuState::HandleLevelSelect( int x, int y )
{
	int oldSelected = m_iSelectedLevel;
	for ( unsigned int i = 0; i<m_vLevelNodes.size(); i++ )
	{
		if ( m_vLevelNodes[i]->m_szParent == m_vLevelNodes[m_iCurrentUniverseIndex]->m_szName && m_vLevelNodes[i]->IsClicked( x, y ) )
		{
			if ( m_vLevelNodes[i]->m_szTarget.substr(0, 6) == "level_" )
				m_iSelectedLevel = i;
			else
				m_iCurrentUniverseIndex = i;
			break;
		}
	}

	return (oldSelected != m_iSelectedLevel);
}

int CMenuState::GetIndexByName( std::string szName )
{
	for ( unsigned int i = 0; i<m_vLevelNodes.size(); i++ )
	{
		if ( m_vLevelNodes[i]->m_szName == szName )
			return i;
	}
	return -1;
}
