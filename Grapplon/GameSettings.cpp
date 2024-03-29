#include "GameSettings.h"
#include "Tokenizer.h"

#include <vector>

CGameSettings *CGameSettings::m_pInstance = NULL;

CGameSettings::CGameSettings()
{

/*
	THROW_FORCE			= 500000.0f;
	MAX_TURN_SPEED		= 70.0f;
	TURN_ACCEL			= 2.0f;
	EJECT_FORCE			= 6000000.0f;
	RETRACT_FORCE		= 10000.0f;
	PITCH_ACCEL_OUT		= 40.0f;
	PITCH_ACCEL_IN		= 0.0f;
	LINK_LENGTH			= 10.0f;
	LINK_AMOUNT			= 8;
	LINK_GRASP_CON		= 8;
	CENT_DIST_HOOK		= -18;
	CFM					= 0.001f;
	ERP					= 0.8f;
	HOOK_AIR_DRAG		= 2.0f;
	HOOK_MASS			= 1.0f;
	AUTO_AIM_ANGLE		= 10.0f;
*/

	THROW_FORCE				= -1;
	MAX_TURN_SPEED			= -1;
	TURN_ACCEL				= -1;
	EJECT_FORCE				= -1;
	RETRACT_FORCE			= -1;
	PITCH_ACCEL_OUT			= -1;
	PITCH_ACCEL_IN			= -1;
	LINK_LENGTH				= -1;
	LINK_AMOUNT				= -1;
	LINK_GRASP_CON			= -1;
	CENT_DIST_HOOK			= -1;
	CFM						= -1;
	ERP						= -1;
	HOOK_AIR_DRAG			= -1;
	HOOK_MASS				= -1;
	AUTO_AIM_ANGLE			= -1;
	WALL_BOUNCES			= -1;
	W_BOUNCE_TOGGLE_TIME	= -1;
	PLAYERS					= -1;
	DAMAGE_MULT				= -1;

	BUFFERS					= 2;
	BUFFER_SIZE				= 49600;

	MIN_ZOOM				= 2.0f;
	MAX_ZOOM				= 4.0f;

	MATCH_TIME				= 180.0f;
	TEMP_TIME				= -1.0f;
	MAX_STARS				= 3;
	PARTICLES_ON			= true;
	SHIP_VELOCITY			= -1;

	PU_SPEED_TIME			= -1;
	PU_SPEED_MULT			= -1;
	PU_JELLY_TIME			= -1;
	PU_SHIELD_TIME			= -1;
	CHAIN_MASS				= 0.01f;
	SCORE_STEAL				= 1000;
	SCORE_GRAB				= 200;
	PH_NUM_ITS				= 20;
	PH_STEP_TIME			= 0.0005f;
	MENU_ON					= true;
	HOOK_CORRECTION_FORCE	= 2000;
	FIRE_AST_MULT			= 3.0;
	SCORE_PUSH_DEAD			= -1;
	FULLSCREEN				= false;
	SCREEN_MARGIN			= -99999;
	VIEW_PERC				= -9999.0f;
	SPAWN_ZOOM_TIME			= -1;
	PU_FREEZE_TIME			= -1;
	MUSIC_SPEEDUP_TIME		= -1;
	MUSIC_SPEEDUP_MULT		= -1;
	AUTO_AIM_ANGLE_HOOK		= -1;
	ODE_THREAD				= false;

	Init();
}

CGameSettings::~CGameSettings()
{
}

bool CGameSettings::Init()
{
	pFile = fopen( "media/scripts/settings.txt", "rt" );
	if ( !pFile )
		return false;

	CTokenizer tokenizer;

	std::string in = ReadLine();
	std::vector<std::string> tokens;
	while ( in != "" )
	{
		tokens = tokenizer.GetTokens( in, " ,;[]:\n" );
		if ( tokens.size() == 0 )
			continue;

		if (	  tokens[0] == "ThrowForce" )			{ THROW_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "MaxTurnSpeed" )			{ MAX_TURN_SPEED		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "TurnAcceleration" )		{ TURN_ACCEL			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "EjectForce" )			{ EJECT_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "RetractForce" )    		{ RETRACT_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "PitchAccelOut" )		{ PITCH_ACCEL_OUT		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "PitchAccelIn" )			{ PITCH_ACCEL_IN		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "MinAccelForProcess" )	{ MIN_ACCEL_FOR_PROCESS	= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkLength" )			{ LINK_LENGTH			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkAmount" )			{ LINK_AMOUNT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkGraspCon" )			{ LINK_GRASP_CON		=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "CentDistHook" )			{ CENT_DIST_HOOK		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "CFM" )					{ CFM					= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "ERP" )					{ ERP					= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "HookAirDrag" )			{ HOOK_AIR_DRAG			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "HookMass" )				{ HOOK_MASS				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "AutoAimAngle" )			{ AUTO_AIM_ANGLE		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "AutoAimAngleHook" )		{ AUTO_AIM_ANGLE_HOOK	= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "Level" )				{ LEVEL					=			   tokens[2]		  ;	}
		else if ( tokens[0] == "AsterWallBounces" )		{ WALL_BOUNCES			=	     atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "WBounceToggleTime" )    { W_BOUNCE_TOGGLE_TIME  = (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "Players" )				{ PLAYERS				=	     atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "DamageMult" )			{ DAMAGE_MULT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "BufferSize" )			{ BUFFER_SIZE			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "Buffers" )				{ BUFFERS				=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "MinZoom" )				{ MIN_ZOOM				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MaxZoom" )				{ MAX_ZOOM				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MatchTime" )			{ MATCH_TIME			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "TempTime" )				{ TEMP_TIME				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "IceDamageMult" )		{ ICE_DAMAGE_MULT		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "FireDamageMult" )		{ FIRE_DAMAGE_MULT		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "PlanetDamageMult" )		{ PLANET_DAMAGE_MULT	= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MaxStars" )				{ MAX_STARS				=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "ParticlesOn" )			{ PARTICLES_ON			=		 (tokens[2] == "1"); }
		else if ( tokens[0] == "FreezeTime" )			{ FREEZE_TIME			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "ShipVelocity" )			{ SHIP_VELOCITY			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "PUSpeedTime" )			{ PU_SPEED_TIME			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "MenuOn" )				{ MENU_ON				=		 (tokens[2] == "1"); }
		else if ( tokens[0] == "PUSpeedMult" )			{ PU_SPEED_MULT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "PUJellyTime" )			{ PU_JELLY_TIME			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "PUShieldTime" )			{ PU_SHIELD_TIME		=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "PUFreezeTime" )			{ PU_FREEZE_TIME		=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "PhysicsStepTime" )		{ PH_STEP_TIME			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "PhysicsNrIterations" )	{ PH_NUM_ITS			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "ChainMass" )			{ CHAIN_MASS			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "ScoreSteal" )			{ SCORE_STEAL			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "ScoreGrab" )			{ SCORE_GRAB			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "HookCorrectionForce" )	{ HOOK_CORRECTION_FORCE =		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "FireAstMult" )			{ FIRE_AST_MULT			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "ScorePushDead" )		{ SCORE_PUSH_DEAD		=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "TimeForEMP" )			{ TIME_FOR_EMP			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "EMPTime" )				{ EMP_TIME				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "Fullscreen" )			{ FULLSCREEN			=		 (tokens[2] == "1"); }
		else if ( tokens[0] == "ScreenMargin" )			{ SCREEN_MARGIN			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "ViewEdgePercentage" )	{ VIEW_PERC				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "PlayerSpawnZoomTime" )	{ SPAWN_ZOOM_TIME		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MusicSpeedUpTime" )		{ MUSIC_SPEEDUP_TIME	=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "MusicSpeedUpMult" )		{ MUSIC_SPEEDUP_MULT	= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "BuoyAmount" )			{ BUOY_AMOUNT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "BuoyDistance" )			{ BUOY_DISTANCE			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "OdeThread" )			{ ODE_THREAD			=		 (tokens[2] == "1"); }


	MUSIC_SPEEDUP_TIME		= -1;
	MUSIC_SPEEDUP_MULT		= -1;


		in = ReadLine();
	}

	fclose( pFile );
	return true;
}

std::string CGameSettings::ReadLine()
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
