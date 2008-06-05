#pragma once

#include <SDL.h>

/*
// These are now forward declared.
#include "ResourceManager.h"
#include "WiimoteManager.h"
#include "State_Game.h"
#include "ODEManager.h"
#include "Renderer.h"
*/

// Forward declarations
class CResourceManager;
class CWiimoteManager;
class IStateManager;
class CODEManager;
class CRenderer;
class CSoundManager;
class CParticleSystemManager;

class CCore
{
private:
	static CCore *m_pInstance;
	CCore();
	virtual ~CCore();

	CRenderer *m_pRenderer;
	CResourceManager *m_pResourceManager;
	CWiimoteManager *m_pWiimoteManager;
	IStateManager *m_pActiveState;
	CODEManager *m_pODEManager;
	CSoundManager *m_pSoundManager;
	CParticleSystemManager *m_pParticleSystemManagerNear, *m_pParticleSystemManagerFar;

	bool m_bMenu;
	bool m_bRunningValid;

public:
	static CCore *Instance() { if ( !m_pInstance ) m_pInstance = new CCore(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	bool SystemsInit();
	void SystemsDestroy();

	void Run();
	bool IsRunning();
	bool ShouldQuit();

	IStateManager *GetActiveState() { return m_pActiveState; }
};
