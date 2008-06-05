#pragma once

#include <string>
#include "Vector.h"
#include <ode/ode.h>
#include "ActiveObject.h"

enum OrbitStyle
{
	STATIC,
	CIRCLE,
	ELLIPSE
};

struct PlanetaryData
{
	Vector position;
	dJointID orbitJoint;

	std::string name;
	std::string orbit;
	std::string image;
	std::string imageOrbit;
	std::string imageGlow;
	std::string imageFire;
	std::string imageFrozen;

	ObjectType planetType;
	OrbitStyle orbitStyle;

	float gravconst;
	int orbitAngle;
	int mass;
	int orbitLength;
	float orbitSpeed;
	int asteroidcount;
	int radius;
	int tempradius;
	float damageMult;
	float scale;
	int rotation;

	std::string emitter;
	int offsetForward, offsetRight;
	bool bNear;

	bool bIsOrbitting;
};
