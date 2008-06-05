#include "WiimoteListener.h"

IWiimoteListener::IWiimoteListener()
{
	for ( int i = 0; i<MAX_MEASUREMENTS; i++ )
	{
		m_fYawData[i] = 0.0f;
		m_fPitchData[i] = 0.0f;
		m_fRollData[i] = 0.0f;

		m_fXData[i] = 0.0f;
		m_fYData[i] = 0.0f;
		m_fZData[i] = 0.0f;

	}


}

void IWiimoteListener::CalculateAccel( wiimote_t *pWiimoteEvent )
{
	// Move all data up one step
	for ( int i = 0; i<MAX_MEASUREMENTS - 1; i++ )
	{
		m_fYawData[i] = m_fYawData[i + 1];
		m_fPitchData[i] = m_fPitchData[i + 1];
		m_fRollData[i] = m_fRollData[i + 1];

		m_fXData[i] = m_fXData[i + 1];
		m_fYData[i] = m_fYData[i + 1];
		m_fZData[i] = m_fZData[i + 1];

	}

	// Assign the new data
	m_fYawData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->orient.yaw;
	m_fPitchData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->orient.pitch;
	m_fRollData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->orient.roll;

	m_fXData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->accel.x;
	m_fYData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->accel.y;
	m_fZData[MAX_MEASUREMENTS - 1] = pWiimoteEvent->accel.z;

	// Clear old results
	m_fYawAccel = m_fPitchAccel = m_fRollAccel = 0.0f;
	m_fXAccel = m_fYAccel = m_fZAccel = 0.0f;

	// Calculate deltas
	for ( int i = 0; i<MAX_MEASUREMENTS - 1; i++ )
	{
		m_fYawAccel += m_fYawData[i + 1] - m_fYawData[i];
		m_fPitchAccel += m_fPitchData[i + 1] - m_fPitchData[i];
		m_fRollAccel += m_fRollData[i + 1] - m_fRollData[i];

		m_fXAccel += m_fXData[i + 1] - m_fXData[i];
		m_fYAccel += m_fYData[i + 1] - m_fYData[i];
		m_fZAccel += m_fZData[i + 1] - m_fZData[i];

	}

	// Calculate average
	m_fYawAccel /= (MAX_MEASUREMENTS - 1);
	m_fPitchAccel /= (MAX_MEASUREMENTS - 1);
	m_fRollAccel /= (MAX_MEASUREMENTS - 1);

	m_fXAccel /= (MAX_MEASUREMENTS - 1);
	m_fYAccel /= (MAX_MEASUREMENTS - 1);
	m_fZAccel /= (MAX_MEASUREMENTS - 1);

}

