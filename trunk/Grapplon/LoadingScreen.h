#pragma once

#include "AnimatedTexture.h"

class CLoadingScreen
{
private:
	static CLoadingScreen *m_pInstance;
	CLoadingScreen();
	~CLoadingScreen();

	// Loading screen runs in own thread
	SDL_Thread *m_pThread;
	SDL_mutex *m_pMutex;
	SDL_cond *m_pThreadCondition, *m_pEngineCondition;
	bool m_bRendering;
	int m_iContext; // Constantly swaps render context from-and-to renderer system

	CAnimatedTexture *m_pBackground;
	CAnimatedTexture *m_pSpinning;

public:
	static CLoadingScreen *Instance() { if ( !m_pInstance ) m_pInstance = new CLoadingScreen(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void Init();

	void StartRendering();
	void StopRendering();
	bool IsRunning() { return m_bRendering; }

	void Render(float fTime);

	// Thread context swapping functions
	void ThreadClaimContext();
	void ThreadReleaseContext();

	void EngineClaimContext();
	void EngineReleaseContext();
};
