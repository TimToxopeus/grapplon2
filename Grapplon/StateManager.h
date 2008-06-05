#pragma once

#include "ActiveObject.h"
#include "WiimoteListener.h"
#include <sdl.h>

class IStateManager : public IActiveObject, public IWiimoteListener
{
protected:
	bool m_bRunning;
	bool m_bQuit;

public:
	IStateManager();
	~IStateManager();

	virtual void Render() = 0;
	virtual void Update( float fTime ) = 0;
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent ) = 0;
	virtual int HandleSDLEvent( SDL_Event event ) = 0;
	virtual bool IsRunning() { return m_bRunning; }
	virtual bool ShouldQuit() { return m_bQuit; }
};
