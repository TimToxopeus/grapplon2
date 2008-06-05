#include "BaseObject.h"
#include "Texture.h"
#include "ODEManager.h"
//#include "ode/objects.h"
#include "AnimatedTexture.h"
#include "LogManager.h"
#include "Planet.h"
#include "GameSettings.h"

CBaseObject::CBaseObject()
{
	m_fAngle = 0.0f;
	m_pImage = NULL;
	SetDepth( -2.0f );
	m_fGravitationalConstant = 0.0f;

	CODEManager* ode = CODEManager::Instance(); 
	//ode->CreatePhysicsData(this, &m_oPhysicsData);
	//SetMass( 10.0f );

	m_iHitpoints = 10000;
	m_iMaxHitpoints = 10000;
	m_fSecondaryScale = 1.0f;
	SetScale( 1.0f );
	m_fInvincibleTime = 0.0f;
}

CBaseObject::~CBaseObject(){
	if(m_pImage) delete m_pImage;
}

void CBaseObject::Render()
{
	SDL_Rect target, size;
	size = m_pImage->GetSize();
	target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
	target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
	target.x = (int)GetX() - (target.w / 2);
	target.y = (int)GetY() - (target.h / 2);

	SDL_Color blink;
	blink.r = blink.g = blink.b = 255;

	float blinkTime = 0.25;
	int div = (int)(m_fInvincibleTime / blinkTime);
	if ( div % 2 == 1 )
		blink.r = blink.g = 0;

	RenderQuad( target, m_pImage, m_fAngle, blink );
}

void CBaseObject::Update( float fTime )
{
	if ( m_fInvincibleTime > 0.0f )
		m_fInvincibleTime -= fTime;

	m_pImage->UpdateFrame( fTime );
}

void CBaseObject::SetPosition( float fX, float fY )
{
	CODEManager::Instance()->BodySetPosition(m_oPhysicsData.body, Vector(fX, fY, 0) );
//	dBodySetPosition(m_oPhysicsData.body, fX, fY, 0.0f);
}

void CBaseObject::SetPosition( Vector pos )
{
	CODEManager::Instance()->BodySetPosition(m_oPhysicsData.body, pos );
//	dBodySetPosition(m_oPhysicsData.body, pos[0], pos[1], 0.0f);
}

Vector CBaseObject::GetPosition()
{
	return dBodyGetPosition(m_oPhysicsData.body);
}

float CBaseObject::GetX()
{
	return dBodyGetPosition(m_oPhysicsData.body)[0];
}

float CBaseObject::GetY()
{
	return dBodyGetPosition(m_oPhysicsData.body)[1];
}

void CBaseObject::SetRotation( float fAngle )
{
	m_fAngle = fAngle;
}

float CBaseObject::GetRotation()
{
	return m_fAngle;
}

void CBaseObject::SetMass( float fMass, bool permanent )
{
	dMass mass; 
	dMassSetBox(&mass, 1, 1, 1, 1); 
	dMassAdjust(&mass, fMass); 
	CODEManager::Instance()->BodySetMass( m_oPhysicsData.body, mass );
//	dBodySetMass(m_oPhysicsData.body, &mass);
	
	if(permanent)
		m_oPhysicsData.m_fMass = fMass;
}

void CBaseObject::ResetMass(){
	SetMass(m_oPhysicsData.m_fMass, true);
}
float CBaseObject::GetMass()
{
	dMass mass;
	dBodyGetMass( m_oPhysicsData.body, &mass );
	return mass.mass;
}

void CBaseObject::SetLinVelocity( Vector& v )
{
	CODEManager::Instance()->BodySetLinVel( m_oPhysicsData.body, v );
//	dBodySetLinearVel(m_oPhysicsData.body, v[0], v[1], v[2]);
}

Vector CBaseObject::GetLinVelocity()
{
	return dBodyGetLinearVel(m_oPhysicsData.body);
}

void CBaseObject::SetAngVelocity( Vector& v)
{
	CODEManager::Instance()->BodySetAngVel( m_oPhysicsData.body, v );
//	dBodySetAngularVel(m_oPhysicsData.body, v[0], v[1], v[2]);
}

void CBaseObject::AddForce( Vector& f )
{
	CODEManager::Instance()->BodyAddForce( m_oPhysicsData.body, f );
//	dBodyAddForce(m_oPhysicsData.body, f[0], f[1], 0.0f);
}

void inline CBaseObject::ApplyForceFront()
{
	dBodyAddForceAtRelPos(m_oPhysicsData.body, frontForce[0], frontForce[1], 0.0f, 0.0f, 0.0f, 0.0f);
}


void CBaseObject::SetForceFront( Vector& f )
{
	frontForce = f;
}

void CBaseObject::SetForce( Vector f )
{
	CODEManager::Instance()->BodySetForce( m_oPhysicsData.body, f );
//	dBodySetForce(m_oPhysicsData.body, f[0], f[1], 0.0f);
}

Vector CBaseObject::GetForwardVector()
{
	Vector v = Vector( cos(DEGTORAD(m_fAngle)), sin(DEGTORAD(m_fAngle)), 0.0f );
	v.Normalize();
	return v;
}

void CBaseObject::CollideWith( CBaseObject *pOther )
{
}

void CBaseObject::OnDie( CBaseObject *m_pKiller )
{
}
