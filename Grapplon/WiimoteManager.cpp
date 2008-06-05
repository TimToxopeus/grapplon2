#include "WiimoteListener.h"
#include "WiimoteManager.h"
#include "LogManager.h"
#include "Core.h"
#include <sdl.h>

CWiimoteManager *CWiimoteManager::m_pInstance = 0;

int WiimoteManagerThread(void *data)
{
	CWiimoteManager *pMan = CWiimoteManager::Instance();
	pMan->Setup();
	CCore *pCore = CCore::Instance();
	while ( pCore->IsRunning() )
	{
		pMan->HandleWiimoteEvents();
		//SDL_Delay( 10 );
	}
	return 0;
}

// Initialize wiimote manager and check for wiimotes
CWiimoteManager::CWiimoteManager()
{
	CLogManager::Instance()->LogMessage("Initializing Wiimote manager.");

	m_pThread = NULL;
}

// Clean up wiimote manager
CWiimoteManager::~CWiimoteManager()
{
	CLogManager::Instance()->LogMessage("Terminating Wiimote manager.");
	// Wait for wiimote event thread to finish
	if ( m_pThread )
	{
		int status;
		SDL_WaitThread( m_pThread, &status );
		m_pThread = NULL;
	}

	CLogManager::Instance()->LogMessage("Unregistering Wiimote listeners.");
	if ( m_vWiimoteListeners.size() > 0 )
	{
		for ( unsigned int a = 0; a<m_vWiimoteListeners.size(); a++ )
			delete m_vWiimoteListeners[a];
		m_vWiimoteListeners.clear();
	}
	wiiuse_cleanup(m_pWiimotes, 4);
}

// Handle wiimote events
void CWiimoteManager::HandleWiimoteEvents()
{
	// Check every second
//	if ( m_iLastCheck + 1000 < SDL_GetTicks() )
//		CheckForWiimotes();

	if ( wiiuse_poll(m_pWiimotes, 4) )
	{
		int i = 0;
		for ( i = 0; i<4; ++i )
		{
			if ( m_pWiimotes[i]->event == WIIUSE_EVENT )
				DispatchEvent( m_pWiimotes[i], i );
		}
	}
}

// Check for wiimotes
int CWiimoteManager::CheckForWiimotes()
{
	int iNewFound, iConnected;
	iNewFound = 0;
	iConnected = 0;

	iNewFound = wiiuse_find(m_pWiimotes, 4, 5);
	if ( iNewFound != m_iFoundWiimotes )
	{
		iConnected = wiiuse_connect(m_pWiimotes, 4);
		if ( iConnected != m_iConnectedWiimotes )
		{
/*			for ( int i = 0; i<4; i++ )
			{
				wiiuse_set_leds(m_pWiimotes[i], 2 << (3 + i));
				wiiuse_rumble(m_pWiimotes[i], 1);
				Sleep(200);
				wiiuse_rumble(m_pWiimotes[i], 0);

				wiiuse_set_nunchuk_orient_threshold( m_pWiimotes[i], 15 );
				wiiuse_set_nunchuk_accel_threshold( m_pWiimotes[i], 15 );

				wiiuse_motion_sensing( m_pWiimotes[i], 1 );
				wiiuse_set_orient_threshold( m_pWiimotes[i], 15 );
				wiiuse_set_accel_threshold( m_pWiimotes[i], 15 );
				wiiuse_set_ir( m_pWiimotes[i], 1 );
			}*/
			wiiuse_set_leds(m_pWiimotes[0], WIIMOTE_LED_1);
			wiiuse_set_leds(m_pWiimotes[1], WIIMOTE_LED_2);
			wiiuse_set_leds(m_pWiimotes[2], WIIMOTE_LED_3);
			wiiuse_set_leds(m_pWiimotes[3], WIIMOTE_LED_4);

			wiiuse_rumble(m_pWiimotes[0], 1);
			wiiuse_rumble(m_pWiimotes[1], 1);

			Sleep(200);

			wiiuse_rumble(m_pWiimotes[0], 0);
			wiiuse_rumble(m_pWiimotes[1], 0);

			for ( int i = 0; i<iConnected; i++ )
			{
				wiiuse_set_nunchuk_orient_threshold( m_pWiimotes[i], 15 );
				wiiuse_set_nunchuk_accel_threshold( m_pWiimotes[i], 15 );

				wiiuse_motion_sensing(m_pWiimotes[i], 1);
				wiiuse_set_flags( m_pWiimotes[i], WIIUSE_SMOOTHING, 0 );

				wiiuse_set_ir(m_pWiimotes[i], 1);
				wiiuse_set_ir_vres( m_pWiimotes[i], 1024, 768 );
				wiiuse_set_ir_position( m_pWiimotes[i], WIIUSE_IR_ABOVE );
			}

/*			wiiuse_set_nunchuk_orient_threshold( m_pWiimotes[1], 15 );
			wiiuse_set_nunchuk_accel_threshold( m_pWiimotes[1], 15 );

			wiiuse_motion_sensing(m_pWiimotes[1], 1);
			wiiuse_set_flags( m_pWiimotes[1], WIIUSE_SMOOTHING, 0 );

			wiiuse_set_ir(m_pWiimotes[1], 1);

			wiiuse_set_ir_vres( m_pWiimotes[1], 1024, 768 );

//			wiiuse_set_orient_threshold( m_pWiimotes[0], 1.0f );
//			wiiuse_motion_sensing( m_pWiimotes[0], 1 );
//			wiiuse_set_ir( m_pWiimotes[0], 1 );*/
		}
	}

	m_iFoundWiimotes = iNewFound;
	m_iConnectedWiimotes = iConnected;

	m_iLastCheck = SDL_GetTicks();
	return m_iConnectedWiimotes;
}

// Send the event to the registered listeners
bool CWiimoteManager::DispatchEvent( wiimote_t *pWiimoteEvent, int iWiimote )
{
	bool handled = false;
	for ( unsigned int a = 0; a<m_vWiimoteListeners.size(); a++ )
	{
		CListenerCarrier *pListener = m_vWiimoteListeners[a];

		//   Send to registered listeners						 Send to all	   Receives all events
		if ( pListener->GetWiimote() == iWiimote || iWiimote == -1 || pListener->GetWiimote() == -1 )
		{
			bool result = pListener->GetListener()->HandleWiimoteEvent( pWiimoteEvent );
			if ( result )
				handled = true;
		}
	}
	return handled;
}

// Register a listener
bool CWiimoteManager::RegisterListener( IWiimoteListener *pListener, int iWiimote )
{
	CLogManager::Instance()->LogMessage("Registering Wiimote listener.");

	// Non-existant objects need not apply
	if ( !pListener )
		return false;

	// First make sure the object isn't already registered
	for ( unsigned int a = 0; a<m_vWiimoteListeners.size(); a++ )
	{
		if ( m_vWiimoteListeners[a]->GetListener() == pListener && m_vWiimoteListeners[a]->GetWiimote() == iWiimote )
			return true; // Already on the list, this object is registered to this wiimote already.
	}

	// Else add it to the list
	CListenerCarrier *pCarrier = new CListenerCarrier( pListener, iWiimote );
	m_vWiimoteListeners.push_back( pCarrier );
	return true;
}

// Unregister a listener
bool CWiimoteManager::UnregisterListener( IWiimoteListener *pListener )
{
	// Non-existant objects do not exist
	if ( !pListener )
		return false;

	// Check all the objects
	for ( unsigned int a = 0; a<m_vWiimoteListeners.size(); a++ )
	{
		if ( m_vWiimoteListeners[a]->GetListener() == pListener )
		{
			// Found it, remove it from the list.
			delete m_vWiimoteListeners[a];
			m_vWiimoteListeners.erase( m_vWiimoteListeners.begin() + a );
			return true;
		}
	}

	// This object is not registered.
	return false;
}

void CWiimoteManager::StartEventThread()
{
	if ( m_pThread == NULL )
		m_pThread = SDL_CreateThread( WiimoteManagerThread, NULL );
}

void CWiimoteManager::StopEventThread()
{
	if ( m_pThread )
	{
		int status;
		SDL_WaitThread( m_pThread, &status );
		m_pThread = NULL;
	}
}

void CWiimoteManager::Setup()
{
	m_iFoundWiimotes = 0;
	m_iConnectedWiimotes = 0;

	m_pWiimotes = wiiuse_init(4);
	CheckForWiimotes();
}
