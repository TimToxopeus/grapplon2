#pragma once

#include <string>
#include "Vector.h"

class CParticleBehaviour
{
public:
	CParticleBehaviour();
	CParticleBehaviour( std::string szName, float fEffect, float fVelocity );

	std::string m_szName;
	float m_fVelocity;
	float m_fEffect;

	Vector ComputeDirection( Vector direction );
};
