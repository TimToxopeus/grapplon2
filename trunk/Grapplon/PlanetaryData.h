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
	// Position
	Vector position;
	Vector position2;
	Vector exitVector1;
	Vector exitVector2;
	dJointID orbitJoint;
	dJointID orbitJoint2;

	// Data
	std::string name;
	std::string orbit;
	std::string orbit2;
	std::string image;
	std::string imageOrbit;
	std::string imageGlow;
	std::string imageFire;
	std::string imageFrozen;

	ObjectType planetType;
	OrbitStyle orbitStyle;

	float gravconst;
	int orbitAngle;
	int orbitAngle2;
	int mass;
	int orbitLength;
	int orbitLength2;
	float orbitSpeed;
	float orbitSpeed2;
	int asteroidcount;	// Amount of asteroids in orbit
	int radius;			// Collision radius
	int tempradius;		// Distance to which temperature affects asteroids
	float damageMult;	// Damage multiplier
	float scale;		// Planet scale
	int rotation;
	float zoomradius;

	std::string emitter;
	int offsetForward, offsetRight;
	bool bNear;

	bool bIsOrbitting;
};
