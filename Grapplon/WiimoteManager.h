#pragma once

#include <vector>
#include <sdl.h>
#include "wiiuse.h"

// Forward declaration
class IWiimoteListener;

class CListenerCarrier
{
private:
	IWiimoteListener *m_pListener;
	int m_iWiimote;

public:
	CListenerCarrier( IWiimoteListener *pListener, int iWiimote ) { m_pListener = pListener; m_iWiimote = iWiimote; }
	virtual ~CListenerCarrier() {}

	IWiimoteListener *GetListener() { return m_pListener; }
	int GetWiimote() { return m_iWiimote; }
};

class CWiimoteManager
{
private:
	static CWiimoteManager *m_pInstance;
	CWiimoteManager();
	virtual ~CWiimoteManager();

	SDL_Thread *m_pThread;
	wiimote **m_pWiimotes;
	int m_iFoundWiimotes;
	int m_iConnectedWiimotes;
	int m_iLastCheck;

	std::vector<CListenerCarrier *> m_vWiimoteListeners;

	bool DispatchEvent( wiimote_t *pWiimoteEvent, int iWiimote );

public:
	static CWiimoteManager *Instance() { if ( !m_pInstance ) m_pInstance = new CWiimoteManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void Setup();
	void StartEventThread();
	void StopEventThread();
	void HandleWiimoteEvents();
	int CheckForWiimotes();

	int GetActiveWiimotes() { return m_iConnectedWiimotes; }

	bool RegisterListener( IWiimoteListener *pListener, int iWiimote );
	bool UnregisterListener( IWiimoteListener *pListener );
};
