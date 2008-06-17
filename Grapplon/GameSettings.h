#pragma once

#include <string>

class CGameSettings
{
private:
	static CGameSettings *m_pInstance;
	CGameSettings();
	virtual ~CGameSettings();

	FILE *pFile;
	std::string ReadLine();

public:
	static CGameSettings *Instance() { if ( !m_pInstance ) m_pInstance = new CGameSettings(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }
	
	std::string LEVEL;
	float		THROW_FORCE;
	float		MAX_TURN_SPEED;
	float		TURN_ACCEL;
	float		EJECT_FORCE;
	float		RETRACT_FORCE;
	float		PITCH_ACCEL_OUT;
	float		PITCH_ACCEL_IN;
	float		MIN_ACCEL_FOR_PROCESS;
	float		LINK_LENGTH;
	int			LINK_AMOUNT;
	int			LINK_GRASP_CON;
	float		CENT_DIST_HOOK;
	float		CFM;
	float		ERP;
	float		HOOK_AIR_DRAG;
	float		HOOK_MASS;
	float		AUTO_AIM_ANGLE;

	int			WALL_BOUNCES;
	float		W_BOUNCE_TOGGLE_TIME;

	int			PLAYERS;
	int			DAMAGE_MULT;

	int			BUFFER_SIZE;
	int			BUFFERS;

	float		MIN_ZOOM, MAX_ZOOM;
	float		MATCH_TIME;
	float		TEMP_TIME;
	
	float		ICE_DAMAGE_MULT;
	float		FIRE_DAMAGE_MULT;
	float		PLANET_DAMAGE_MULT;
	float		FREEZE_TIME;
	float		SHIP_VELOCITY;

	int			MAX_STARS;
	bool		PARTICLES_ON;
	bool		MENU_ON;

	int			SCORE_STEAL;
	int			SCORE_GRAB;
	int			SCORE_PUSH_DEAD;

	int			PU_SPEED_TIME;
	int			PU_SPEED_MULT;
	int			PU_JELLY_TIME;
	int			PU_SHIELD_TIME;
	int			PU_FREEZE_TIME;

	float		PH_STEP_TIME;
	int			PH_NUM_ITS;

	int			HOOK_CORRECTION_FORCE;

	float		CHAIN_MASS;
	float		FIRE_AST_MULT;

	float		TIME_FOR_EMP;
	float		EMP_TIME;


	bool		FULLSCREEN;

	int			SCREEN_MARGIN;
	float		VIEW_PERC;
	float		SPAWN_ZOOM_TIME;
	int			MUSIC_SPEEDUP_TIME;
	float		MUSIC_SPEEDUP_MULT;

	int			BUOY_AMOUNT;
	int			BUOY_DISTANCE;
	float		AUTO_AIM_ANGLE_HOOK;
	bool		ODE_THREAD;
	bool  Init();
	
};

#define SETS CGameSettings::Instance()
