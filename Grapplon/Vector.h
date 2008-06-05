#pragma once

#define DEGTORAD(a) (a - 90.0f)*(3.14f/180.0f)
#define RADTODEG(a) (a * 180.0f / 3.14f) + 90.0f
#define DEGTORAD2(a) a*3.14159265f/180.0f
#define RADTODEG2(a) a * 180.0f / 3.14159265f


class Vector
{
public:

	static Vector FromAngleLength(float angle, float length);

	Vector();
	Vector( const float *vector );
	Vector( float x, float y, float z );

	void Normalize();
	Vector GetNormalized();
	float Length();
	float LengthSquared();
	float DotProduct( Vector &other );
	float Angle( Vector &other );
	float SignedDistance( Vector& start, Vector& end);
	Vector CrossProduct( Vector &other );
	Vector Mirror( Vector &normal );
	float CalculateAngle( Vector &other );
	Vector Rotate( float fAngle );
	Vector Rotate2( float fAngle );
	Vector Rotate3( float fAngle );

	void CopyInto( float *v ) { v[0] = vector[0]; v[1] = vector[1]; v[2] = vector[2]; }

	float vector[3];

	// Operators
	Vector operator+( Vector &other );
	Vector operator-( Vector &other );
	Vector operator*( float &other );
	Vector operator/( float &other );
	Vector operator*( const float &other );
	Vector operator/( const float &other );

	Vector &operator+=( const Vector &other );
	Vector &operator-=( const Vector &other );
	Vector &operator*=( const float &other );
	Vector &operator/=( const float &other );

	Vector &operator=( const Vector &other );
	Vector &operator=( const float *other );

	float &operator[] (unsigned i);
};
