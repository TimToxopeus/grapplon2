#include <time.h>
#include "PlayerObject.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "Hook.h"
#include "GameSettings.h"
#include "Asteroid.h"
#include "Renderer.h"
#include "LogManager.h"

#include "ODEManager.h"
#include "Vector.h"

#include "ParticleSystemManager.h"
#include "PowerUp.h"

#include "Core.h"
#include "State_Game.h"

CPlayerObject::CPlayerObject( int iPlayer )
	: m_iScore(0), m_iPlayer(iPlayer), y(10.0f), p(10.0f), r(10.0f), m_bHandleWiiMoteEvents(true), timeSinceNoInput(5.0f), m_fRespawnTime(0.0f)
{
	m_eType = SHIP;
	m_fShipVel = SETS->SHIP_VELOCITY;

	std::string image = "media/scripts/texture_player" + itoa2(iPlayer + 1) + ".txt";
	m_pImage = new CAnimatedTexture(image);
	m_pImage->SetFramerate( 10 );
	m_pImage->Scale( 0.9f );
	m_pRadius = new CAnimatedTexture("media/scripts/texture_white_radius.txt");
	SetDepth( -1.0f );
	m_pImageDamage = new CAnimatedTexture("media/scripts/texture_player_damage.txt");
	m_pImageDamage->SetFramerate( 10 );
	m_pImageDamage->Scale( 0.9f );
	m_pFrozenImage = new CAnimatedTexture("media/scripts/texture_ship_frozen.txt");
	m_pFrozenImage->Scale( 0.9f );
	m_pJellyImage = new CAnimatedTexture("media/scripts/texture_ship_jelly.txt");
	m_pJellyImage->Scale( 0.9f );
	image = "media/scripts/texture_ship_shield" + itoa2(iPlayer + 1) + ".txt";
	m_pShieldImage = new CAnimatedTexture(image);
	m_pShieldImage->Scale( 0.9f );

	m_pExplosion = new CAnimatedTexture("media/scripts/texture_explosion.txt");

	m_fPUSpeedTime = 0;
	m_fPUJellyTime = 0;
	m_fPUHealthTime = 0;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 30.0f);
	SetMass( 1000.0f );
	m_oPhysicsData.m_bAffectedByGravity = true;
	m_oPhysicsData.m_fAirDragConst = 3000.0f;

	m_pHook = new CHook( this );
	
	m_pThrusterLeft = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/particle_thruster" + itoa2(iPlayer + 1) + ".txt" );
	if ( m_pThrusterLeft) m_pThrusterLeft->ToggleSpawn();		// TODO: Reset
	m_pThrusterRight = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/particle_thruster" + itoa2(iPlayer + 1) + ".txt" );
	if ( m_pThrusterRight ) m_pThrusterRight->ToggleSpawn();
}

CPlayerObject::~CPlayerObject()
{
	delete m_pRadius;
	delete m_pImageDamage;
	delete m_pExplosion;
}


void CPlayerObject::SetPosition( float fX, float fY ){
	this->SetPosition( Vector(fX, fY, 0.0f) );
}

void CPlayerObject::SetPosition( Vector pos ){
	if ( m_pHook )
		this->m_pHook->adjustPos( pos - this->GetPosition() );
	CBaseObject::SetPosition(pos);
}

void inline CPlayerObject::ApplyForceFront()
{
	if(this->m_fPUSpeedTime > 0.001){
		int mult = SETS->PU_SPEED_MULT;
		dBodyAddForceAtRelPos(m_oPhysicsData.body, frontForce[0]*mult, frontForce[1]*mult, 0.0f, 0.0f, 0.0f, 0.0f);
	} else {
		dBodyAddForceAtRelPos(m_oPhysicsData.body, frontForce[0], frontForce[1], 0.0f, 0.0f, 0.0f, 0.0f);
	}
}

bool CPlayerObject::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if (!m_bHandleWiiMoteEvents || m_fFreezeTime > 0.0f) return false;

	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(pWiimoteEvent, 1);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(pWiimoteEvent, 0);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) || (SETS->PITCH_ACCEL_OUT != 0.0f && m_fPitchAccel > SETS->PITCH_ACCEL_OUT) )
		{
			if ( m_pHook )
			{
				if ( m_pHook->m_eHookState == CONNECTED )
				{
					m_pHook->m_eHookState = EJECTING;
				}
			}
		}
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) || (SETS->PITCH_ACCEL_IN != 0.0f && m_fPitchAccel < SETS->PITCH_ACCEL_IN) )
		{
			if ( m_pHook )
			{
				if ( m_pHook->m_eHookState == HOMING )
					m_pHook->m_eHookState = RETRACTING;
				else if( m_pHook->m_eHookState == SWINGING)
					m_pHook->m_eHookState = THROWING;
			}
		}

		if (WIIUSE_USING_ACC(pWiimoteEvent))
		{
			CalculateAccel( pWiimoteEvent );

			if( abs(m_fXAccel) > SETS->MIN_ACCEL_FOR_PROCESS || abs(m_fZAccel) > SETS->MIN_ACCEL_FOR_PROCESS )
			{
				if ( m_pHook )
					m_pHook->AddChainForce(m_fXAccel, m_fZAccel);
			} else
			{
				if ( m_pHook )
					m_pHook->AddChainForce(0, 0);
			}

			y = pWiimoteEvent->orient.yaw;
			p = pWiimoteEvent->orient.pitch;
			r = pWiimoteEvent->orient.roll;

		}

		if (pWiimoteEvent->exp.type == EXP_NUNCHUK)
		{
			if ( pWiimoteEvent->exp.nunchuk.js.mag > 0.05f )
			{
				float difference = -(m_fAngle - pWiimoteEvent->exp.nunchuk.js.ang);
				float direction = difference;
				if ( difference > 180.0f )
					direction = difference - 360.0f;
				else if ( difference < -180.0f )
					direction = difference + 360.0f;

				if ( difference < 0.0f ) difference = -difference;
				if ( difference < 3.0f )
					m_fAngle = pWiimoteEvent->exp.nunchuk.js.ang;
				else
					m_fAngle += (direction / 6.0f);
				m_fVelocityForward = 25000.0f * (sin(1.57f * pWiimoteEvent->exp.nunchuk.js.mag));

				timeSinceNoInput = 0.0f;

				Vector v = GetPosition();
				float angle = (pWiimoteEvent->exp.nunchuk.js.ang - 90.0f)*(3.14f/180.0f);
				v += Vector( cos(angle), sin(angle), 0.0f );
				v -= GetPosition();
				v.Normalize();
				SetForceFront( v * m_fVelocityForward * 100.0f );

				return true;
			}
			else{
				SetForceFront(Vector(0, 0, 0));
				m_fVelocityForward = 0.0f;
			}
		}
	}
	return false;
}

void CPlayerObject::Render()
{
	SDL_Rect target, size;
	size = m_pRadius->GetSize();
	target.w = (int)((float)size.w * GetScale());
	target.h = (int)((float)size.h * GetScale());
	target.x = (int)GetX() - (int)((float)target.w / 2.0f);
	target.y = (int)GetY() - (int)((float)target.h / 2.0f);

	SDL_Color colour;
	colour.r = colour.g = colour.b = 0;
	if ( m_iPlayer == 0 ) // green
	{
		colour.g = 255;
	}
	else if ( m_iPlayer == 1 ) // red
	{
		colour.r = 255;
	}
	else if ( m_iPlayer == 2 ) // blue
	{
		colour.b = 255;
	}
	else // purple
	{
		colour.r = 255;
		colour.b = 255;
	}

	RenderQuad( target, m_pRadius, m_fAngle, colour );

	CBaseMovableObject::Render();

	// Damage
	int part = (m_iHitpoints * 3) / m_iMaxHitpoints;

	if(part < 2)
	{
		m_pImageDamage->SetAnimation((part == 1 ? 0 : 1));

		size = m_pImageDamage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pImageDamage, m_fAngle);
	}

	// Frozen
	if(m_fFreezeTime > 0.01f)
	{
		size = m_pFrozenImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pFrozenImage, m_fAngle);
	}

	// Jelly
	if(this->m_fPUJellyTime > 0.01f)
	{
		size = m_pJellyImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pJellyImage, m_fAngle);
	}

	// Shield
	if(this->m_fPUShieldTime > 0.01f)
	{
		size = m_pShieldImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pShieldImage, m_fAngle);
	}

	if ( m_iHitpoints <= 0 )
	{
		target = m_pExplosion->GetSize();
		target.w += target.w;
		target.h += target.h;
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pExplosion, m_fExplosionAngle + 180.0f, 1 );
	}

}

void CPlayerObject::Update( float fTime )
{
	timeSinceNoInput += fTime;
	if ( timeSinceNoInput > 5.0f )
	{
		m_fAngle += 15.0f * fTime;
		m_fVelocityForward = 50.0f;
	}

	if ( m_fRespawnTime > 0.0f )
	{
		m_fRespawnTime -= fTime;

		if ( m_fRespawnTime < 1.0f )
		{
			if ( m_fRespawnTime + fTime > 1.0f )
			{
				Respawn();
				SetDepth( 1.0f );
			}

			SetScale( 1.0f + 3.0f * m_fRespawnTime );
			if ( m_pHook )
				m_pHook->SetScale( 1.0f + 3.0f * m_fRespawnTime );
			SetAlpha( 1.0f - (1.0f * m_fRespawnTime) );
			if ( m_pHook )
				m_pHook->SetAlpha( 1.0f - (1.0f * m_fRespawnTime) );
		}

		if ( m_fRespawnTime <= 0.0f )
		{
			m_bHandleWiiMoteEvents = true;
			SetDepth( -1.0f );
			m_fInvincibleTime = 2.0f;
			if ( m_pHook )
				m_pHook->SetInvincibleTime( 2.0f );
			m_fAlpha = 1.0f;

			if ( m_pThrusterLeft )
				m_pThrusterLeft->ToggleSpawn(true, true);
			if ( m_pThrusterRight )
				m_pThrusterRight->ToggleSpawn(true, true);
		}
	}

	if(m_fFreezeTime > 0){
		m_fFreezeTime -= fTime;
	}

	if(this->m_fPUJellyTime > 0.001){
		m_fPUJellyTime -= fTime;
	}

	if(this->m_fPUShieldTime > 0.001){
		m_fPUShieldTime -= fTime;
	}

	if(this->m_fPUSpeedTime > 0.001){
		m_fPUSpeedTime -= fTime;
	}

	Vector backward = GetForwardVector();
	if ( m_pThrusterLeft )
		m_pThrusterLeft->SetDirection( backward );
	if ( m_pThrusterRight )
		m_pThrusterRight->SetDirection( backward );

	Vector right = backward.Rotate( 180.0f );
	if ( m_pThrusterLeft )
		m_pThrusterLeft->SetPosition( GetPosition() + backward * -50.0f + right * -20.0f );
	if ( m_pThrusterRight )
		m_pThrusterRight->SetPosition( GetPosition() + backward * -50.0f + right * 30.0f );

	//m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );

	if ( m_iHitpoints <= 0 )
		m_pExplosion->UpdateFrame( fTime );

	CBaseMovableObject::Update( fTime );


}

void CPlayerObject::OnDie( CBaseObject *m_pKiller )
{
	m_pExplosion->SetAnimation(rand()%3);
	m_pExplosion->SetFrame(0);
	m_fExplosionAngle = m_fAngle;

	Vector nullVec;
	if ( m_pHook )
		m_pHook->HandlePlayerDied();
	m_bHandleWiiMoteEvents = false;
	SetForceFront(nullVec);
	SetLinVelocity(nullVec);
	SetAngVelocity(nullVec);

	m_fFreezeTime = 0.0f;	
	m_fInvincibleTime = 4.0f;
	m_fPUJellyTime = 0;
	m_fPUShieldTime = 0;
	m_fPUSpeedTime = 0;

	if ( m_pHook )
		m_pHook->SetInvincibleTime( 4.0f );
	m_fRespawnTime = 2.0f;
	SetAlpha( 0.0f );
	if ( m_pHook )
		m_pHook->SetAlpha( 0.0f );

	// Spawn emitter
	Vector direction = m_pKiller->GetPosition() - GetPosition();
	direction.Normalize();

	if ( m_pThrusterLeft )
		m_pThrusterLeft->ToggleSpawn(true, false);
	if ( m_pThrusterRight )
		m_pThrusterRight->ToggleSpawn(true, false);
}

void CPlayerObject::Respawn()
{
	int x, y;

	CRenderer *pRenderer = CRenderer::Instance();
	do
	{
		x = rand()%4000 - 2000;
		y = rand()%3000 - 1500;
	} while ( pRenderer->ObjectsInRange( x, y, 100 ) );

	Vector v = Vector( (float)x, (float)y, 0.0f );
	SetPosition( v );
	Vector n;
	m_oPhysicsData.m_pOwner->SetLinVelocity(n);
	SetForce(n);
	m_iHitpoints = m_iMaxHitpoints;
}

void CPlayerObject::TookHealthPowerUp(){ m_iHitpoints	 = m_iMaxHitpoints; }
void CPlayerObject::TookSpeedPowerUp() { m_fPUSpeedTime  = (float) SETS->PU_SPEED_TIME; }
void CPlayerObject::TookJellyPowerUp() { m_fFreezeTime = 0; m_fPUJellyTime  = (float) SETS->PU_JELLY_TIME;  m_fPUShieldTime = 0; }
void CPlayerObject::TookShieldPowerUp(){ m_fFreezeTime = 0; m_fPUShieldTime = (float) SETS->PU_SHIELD_TIME; m_fPUJellyTime = 0; }

void CPlayerObject::CollideWith( CBaseObject *pOther)
{
	if ( pOther->getType() == POWERUP )
	{
		CPowerUp *pUp = (CPowerUp *)pOther;
		if ( pUp->GetPowerupType() == 2 )
		{
			m_iHitpoints = m_iMaxHitpoints;
		}
		return;
	}

	if( m_fPUShieldTime > 0.0001f) return;

	if ( m_fInvincibleTime > 0.0001f ) return;

	Vector posThis  = this->GetPosition();
	Vector posOther = pOther->GetPosition();
	Vector velThis  = this->GetLinVelocity();
	Vector velOther = pOther->GetLinVelocity();
	float  mThis    = this->GetMass();
	float  mOther   = pOther->GetMass();

	float xThis   = posThis[0];
	float yThis   = posThis[1];
	float xOther  = posOther[0];
	float yOther  = posOther[1];
	float vxThis  = velThis[0];
	float vyThis  = velThis[1];
	float vxOther = velOther[0];
	float vyOther = velOther[1];

	float m21  = mOther/mThis;
    float x21  = xOther-xThis;
    float y21  = yOther-yThis;
    float vx21 = vxOther-vxThis;
    float vy21 = vyOther-vyThis;

	float angle = y21/x21;				// Angle of difference vector
	float dvx2  = -2*(vx21 +angle*vy21)/((1+angle*angle)*(1+m21)) ;		// Change in x-velocity for other
	float vx2   = vxOther+dvx2;				// Velocity of other after collision (y-dir)
	float vy2   = vyOther+angle*dvx2;		// Velocity of other after collision (x-dir)								
	float vx1   = vxThis-m21*dvx2;			// Velocity of self after collision (x-dir)
	float vy1   = vyThis-angle*m21*dvx2;	// Velocity of self after collision (y-dir)

	int iOldHitpoints = m_iHitpoints;
	
	float diffX = abs(vx1 - vxThis);
	float diffY = abs(vy1 - vyThis);
	int damage = (int) Vector(diffX, diffY, 0.0f).Length() * SETS->DAMAGE_MULT;
	
	float mult = 1.0;
	
	ObjectType oType = pOther->getType();

	if(oType == ICE || oType == ASTEROID || oType == SUN || oType == FIRE || oType == ORDINARY)
		mult = dynamic_cast<CPlanet*>(pOther)->m_fDamageMult;

	if( pOther->getType() == ASTEROID)
	{
		CAsteroid* asteroid = dynamic_cast<CAsteroid*>(pOther);
		time_t throwTime = time(NULL) - asteroid->m_fThrowTime;
		if(throwTime <= 4)
		{
			if(asteroid->m_eAsteroidState == ON_FIRE)
			{
				mult = SETS->FIRE_DAMAGE_MULT;
				m_fPUJellyTime = 0;
			}
			else if(asteroid->m_eAsteroidState == FROZEN)
			{
				mult = SETS->ICE_DAMAGE_MULT;
				m_fFreezeTime = SETS->FREEZE_TIME;
				m_fPUJellyTime = 0;
			}

			int score = 0;
			if(asteroid->m_pThrowingPlayer != this && m_fPUJellyTime <= 0.0001f)
			{
				score += (int) (damage * mult);
				asteroid->m_pThrowingPlayer->m_iScore += score;
				asteroid->m_pThrowingPlayer->m_iScore += asteroid->m_iMilliSecsInOrbit / 10;
				score += asteroid->m_iMilliSecsInOrbit / 10;
			}

			CGameState *pState = (CGameState *)CCore::Instance()->GetActiveState();
			pState->AddScore( asteroid->m_pThrowingPlayer->GetPlayerID(), score, (int)GetX(), (int)GetY() );
		}
	}

	if(m_fPUJellyTime > 0) return; 
	m_iHitpoints -= (int) (damage * mult);

	if ( m_iHitpoints <= 0 )
		m_iHitpoints = 0;

	if ( m_iHitpoints == 0 && iOldHitpoints > 0 )
	{
		CLogManager::Instance()->LogMessage( "Object died" );
		OnDie( pOther );
	}

	//CBaseObject::CollideWith(pOther, force);
}
