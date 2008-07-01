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
	
	std::string LEVEL;						// DEBUG VALUE; Default level loaded
	float		THROW_FORCE;				// Force with which an object is thrown
	float		MAX_TURN_SPEED;				// Speed at which the ship turns
	float		TURN_ACCEL;					// Speed at which the turning accelerates
	float		EJECT_FORCE;				// Force with which the hook is ejected
	float		RETRACT_FORCE;				// Force with which the hook is pulled back to the ship
	float		PITCH_ACCEL_OUT;			// Outgoing acceleration on Pitch
	float		PITCH_ACCEL_IN;				// Incoming acceleration on Pitch
	float		MIN_ACCEL_FOR_PROCESS;		// Minimal level of acceleration before event is raised
	float		LINK_LENGTH;				// Distance of links in the hook chain
	int			LINK_AMOUNT;				// Amount of links in the hook chain
	int			LINK_GRASP_CON;				// Connection to hook chain
	float		CENT_DIST_HOOK;				// Hook distance to center
	float		CFM;						// Physics engine error value
	float		ERP;						// Physics engine error value
	float		HOOK_AIR_DRAG;				// Air drag force on the hook
	float		HOOK_MASS;					// Mass of the hook
	float		AUTO_AIM_ANGLE;				// Auto aim angle

	int			WALL_BOUNCES;				// Wall bounce
	float		W_BOUNCE_TOGGLE_TIME;		// Bounce toggle time

	int			PLAYERS;					// DEBUG VALUE; Default amount of players
	int			DAMAGE_MULT;				// Global damage multiplier

	int			BUFFER_SIZE;				// Sound buffer size
	int			BUFFERS;					// Sound buffer count

	float		MIN_ZOOM, MAX_ZOOM;			// Camera minimal and maximum zoom values
	float		MATCH_TIME;					// Time a game round takes (In seconds)
	float		TEMP_TIME;					// Time for asteroid temperature change
	
	float		ICE_DAMAGE_MULT;			// Damage multiplier for ice asteroids
	float		FIRE_DAMAGE_MULT;			// Damage multiplier for fire asteroids
	float		PLANET_DAMAGE_MULT;			// Damage multiplier for planets
	float		FREEZE_TIME;				// Time an object is frozen
	float		SHIP_VELOCITY;				// Max speed of a ship

	int			MAX_STARS;					// Maximum stars in the background
	bool		PARTICLES_ON;				// Particle system enabled
	bool		MENU_ON;					// DEBUG VALUE; Menu enabled

	int			SCORE_STEAL;				// Points awarded for stealing an asteroid
	int			SCORE_GRAB;					// Points awarded for picking up an asteroid
	int			SCORE_PUSH_DEAD;			// Points awarded for ramming someone to death

	int			PU_SPEED_TIME;				// Time for a speed powerup
	int			PU_SPEED_MULT;				// Speed powerup velocity multiplier
	int			PU_JELLY_TIME;				// Time for a jelly powerup
	int			PU_SHIELD_TIME;				// Time for a shield powerup
	int			PU_FREEZE_TIME;				// Time for a freeze powerup

	float		PH_STEP_TIME;				// Step size in physics simulation
	int			PH_NUM_ITS;					// Number of iterations per time unit

	int			HOOK_CORRECTION_FORCE;		// Corrective force on hook motion

	float		CHAIN_MASS;					// Mass of the chain
	float		FIRE_AST_MULT;				// Score multiplier on fire asteroids

	float		TIME_FOR_EMP;				// EMP pulse on player ship duration
	float		EMP_TIME;					// Time for asteroid to acquire EMP charge

	bool		FULLSCREEN;					// Fullscreen enabled

	int			SCREEN_MARGIN;				// Margin around screen
	float		VIEW_PERC;					// Camera motion control
	float		SPAWN_ZOOM_TIME;			// Time for the camera to correct position
	int			MUSIC_SPEEDUP_TIME;			// Match time at which music speeds up
	float		MUSIC_SPEEDUP_MULT;			// Speed up multiplier

	int			BUOY_AMOUNT;				// Amount of buoys around the universe
	int			BUOY_DISTANCE;				// Distance between buoys
	float		AUTO_AIM_ANGLE_HOOK;		// Grabbing auto aim
	bool		ODE_THREAD;					// DEBUG VALUE; ODE Thread enabled
	bool  Init();
	
};

#define SETS CGameSettings::Instance()
