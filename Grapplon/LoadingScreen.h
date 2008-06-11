#pragma once

#include "AnimatedTexture.h"

class CLoadingScreen
{
private:
	static CLoadingScreen *m_pInstance;
	CLoadingScreen();
	~CLoadingScreen();

	SDL_Thread *m_pThread;
	bool m_bRendering;

	CAnimatedTexture *m_pBackground;
	CAnimatedTexture *m_pSpinning;

	int m_iContextState;

public:
	static CLoadingScreen *Instance() { if ( !m_pInstance ) m_pInstance = new CLoadingScreen(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void Init();

	void StartRendering();
	void StopRendering();
	bool IsRunning() { return m_bRendering; }

	void Render(float fTime);

	void RequestContext();
	void ReturnContext();
	void CheckContext();

	void SetContext( int iContext ) { m_iContextState = iContext; }
};
