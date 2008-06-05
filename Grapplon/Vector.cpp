#include "Vector.h"
#include <Math.h>
#include <exception>


Vector Vector::FromAngleLength(float angle, float length)
{
	return Vector(length, 0, 0).Rotate2(angle);
}	


Vector::Vector()
{
	vector[0] = vector[1] = vector[2] = 0.0f;
}

Vector::Vector( const float *vector )
{
	this->vector[0] = vector[0];
	this->vector[1] = vector[1];
	this->vector[2] = vector[2];
}

Vector::Vector( float x, float y, float z )
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
}

void Vector::Normalize()
{
	float l = Length();
	if ( l != 0.0f )
	{
		vector[0] /= l;
		vector[1] /= l;
		vector[2] /= l;
	}
}

Vector Vector::GetNormalized()
{
	Vector v = *this;
	v.Normalize();
	return v;
}

float Vector::Length()
{
	float r = (float)sqrt(LengthSquared());
	if ( r < 0.0f )
		r = -r;
	return r;
}

float Vector::LengthSquared()
{
	return vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
}

float Vector::DotProduct( Vector &other )
{
	return vector[0] * other.vector[0] + vector[1] * other.vector[1] + vector[2] * other.vector[2];
}

float Vector::Angle( Vector &other )
{
	return (float)acos( DotProduct(other) / (Length() * other.Length()) );
}

Vector Vector::CrossProduct(Vector &o)
{
	Vector v;
	Vector &t = *this;

	v[0] = t[1] * o[2] - t[2] * o[1];
	v[1] = t[2] * o[0] - t[0] * o[2];
	v[2] = t[0] * o[1] - t[1] * o[0];

	return v;
}

Vector Vector::Mirror( Vector &normal )
{
	Vector v;
	float angle = DotProduct( normal );
	Vector l = normal * (angle * 2);
	v = (*this * -1) + l;
	return (v * -1);
}

float Vector::CalculateAngle(Vector &other)
{
	Vector v1 = *this;
	Vector v2 = other;

	float angle = atan2( v2[1] - v1[1], v2[0] - v1[0] );
	angle = (angle * (180.0f / 3.14f)) + 90.0f;
	if ( angle > 360.0f )
		angle -= 360.0f;
	if ( angle < 0.0f )
		angle += 360.0f;
	return angle;
}

Vector Vector::Rotate( float fAngle )
{
	float rad = DEGTORAD(fAngle);
	return Vector( vector[0] * cos(rad) - vector[1] * sin(rad), vector[0] * sin(rad) + vector[1] * cos(rad), 0 );
}

Vector Vector::Rotate2( float fAngle )
{
	float rad = DEGTORAD2(fAngle);
	return Vector( vector[0] * cos(rad) + vector[1] * sin(rad), vector[1] * cos(rad) - vector[0] * sin(rad), 0 );
}

Vector Vector::Rotate3( float fAngle )
{
	float rad = DEGTORAD2(fAngle);
	return Vector( vector[0] * cos(rad) - vector[1] * sin(rad), vector[0] * sin(rad) + vector[1] * cos(rad), 0 );
}

float Vector::SignedDistance( Vector& begin, Vector& end )
{
	// p = begin
	// q = end
	// r = point = this

	/*
	(qx*ry - qy*rx) +
	(py*rx - px*ry) +
	(px*qy - py*qx) +
	*/

	return	(end[0]   * vector[1] - end[1]   * vector[0]) +
			(begin[1] * vector[0] - begin[0] * vector[1]) +
			(begin[0] * end[1]    - begin[1] * end[0]   ) ;
}
// Operators
Vector Vector::operator+( Vector &other )
{
	return Vector( vector[0] + other.vector[0], vector[1] + other.vector[1], vector[2] + other.vector[2] );
}

Vector Vector::operator-( Vector &other )
{
	return Vector( vector[0] - other.vector[0], vector[1] - other.vector[1], vector[2] - other.vector[2] );
}

Vector Vector::operator*( float &other )
{
	return Vector( vector[0] * other, vector[1] * other, vector[2] * other );
}

Vector Vector::operator/( float &other )
{
	if ( other != 0.0f )
		return Vector( vector[0] / other, vector[1] / other, vector[2] / other );
	else
		return Vector( 0, 0, 0 );
}

Vector Vector::operator*( const float &other )
{
	return Vector( vector[0] * other, vector[1] * other, vector[2] * other );
}

Vector Vector::operator/( const float &other )
{
	if ( other != 0.0f )
		return Vector( vector[0] / other, vector[1] / other, vector[2] / other );
	else
		return Vector( 0, 0, 0 );
}

Vector &Vector::operator+=( const Vector &other )
{
	vector[0] += other.vector[0];
	vector[1] += other.vector[1];
	vector[2] += other.vector[2];

	return *this;
}

Vector &Vector::operator-=( const Vector &other )
{
	vector[0] -= other.vector[0];
	vector[1] -= other.vector[1];
	vector[2] -= other.vector[2];

	return *this;
}

Vector &Vector::operator*=( const float &other )
{
	vector[0] *= other;
	vector[1] *= other;
	vector[2] *= other;

	return *this;
}

Vector &Vector::operator/=( const float &other )
{
	if ( other != 0.0f )
	{
		vector[0] /= other;
		vector[1] /= other;
		vector[2] /= other;
	}

	return *this;
}

Vector &Vector::operator=( const Vector &other )
{
	if ( &other == this )
		return *this;

	vector[0] = other.vector[0];
	vector[1] = other.vector[1];
	vector[2] = other.vector[2];

	return *this;
}

Vector &Vector::operator=( const float *other )
{
	vector[0] = other[0];
	vector[1] = other[1];
	vector[2] = other[2];

	return *this;
}

float &Vector::operator[] (unsigned i)
{
	if ( i != 0 && i != 1 && i != 2 )
		throw std::exception("incorrect vector index");

	return vector[i];
}
