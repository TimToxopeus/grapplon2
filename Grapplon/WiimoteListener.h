#pragma once

#include <wiiuse.h>

#define MAX_MEASUREMENTS 5

class IWiimoteListener
{
private:
	float m_fYawData[MAX_MEASUREMENTS];
	float m_fPitchData[MAX_MEASUREMENTS];
	float m_fRollData[MAX_MEASUREMENTS];

	float m_fXData[MAX_MEASUREMENTS];
	float m_fYData[MAX_MEASUREMENTS];
	float m_fZData[MAX_MEASUREMENTS];

protected:
	float m_fYawAccel;
	float m_fPitchAccel;
	float m_fRollAccel;

	float m_fXAccel;
	float m_fYAccel;
	float m_fZAccel;

	void CalculateAccel( wiimote_t *pWiimoteEvent );

public:
	IWiimoteListener();
	virtual bool HandleWiimoteEvent(wiimote_t* pWiimoteEvent) = 0;
};
