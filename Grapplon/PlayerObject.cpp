#include <time.h>
#include "PlayerObject.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "Sound.h"
#include "Hook.h"
#include "GameSettings.h"
#include "Asteroid.h"
#include "Renderer.h"
#include "LogManager.h"
#include "Universe.h"

#include "ODEManager.h"
#include "Vector.h"

#include "ParticleSystemManager.h"
#include "PowerUp.h"

#include "Core.h"
#include "State_Game.h"

CPlayerObject::CPlayerObject( int iPlayer )
{
	m_iPlayer = iPlayer, 
	m_eType = SHIP;
	m_fShipVel = SETS->SHIP_VELOCITY;

	m_fEMPTime			= 0;
	m_fElectroTime		= 0;
	m_iJellyFrame		= 44;

	m_bIsReinitialized = true;

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
	m_pJellyImage = new CAnimatedTexture("media/scripts/texture_ship_jelly_hit.txt");
	m_pJellyImage->Scale( 0.80f );
	m_pJellyImage->SetFramerate(0);
	image = "media/scripts/texture_ship_shield" + itoa2(iPlayer + 1) + ".txt";
	m_pShieldImage = new CAnimatedTexture(image);
	m_pShieldImage->Scale( 0.9f );

	m_pPUFrozenImage = new CAnimatedTexture("media/scripts/texture_powerup_freeze_ani.txt");
	m_pPUFrozenImage->SetFrame(29);

	image = "media/scripts/texture_ship_respawn" + itoa2(iPlayer + 1) + ".txt";
	this->m_pRespawnImage = new CAnimatedTexture(image);
	m_pRespawnImage->Scale( 0.18f * 0.9f);
	m_pRespawnImage->SetFrame(8);

	image = "media/scripts/texture_electric_ship" + itoa2(iPlayer + 1) + ".txt";
	m_pElectricImage = new CAnimatedTexture(image);
	m_pElectricImage->Scale( 0.33f );
	m_pElectricImage->SetFrame(0);


	m_pExplosion = new CAnimatedTexture("media/scripts/texture_explosion.txt");

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 50);
	SetMass(1000);
	m_oPhysicsData.m_fAirDragConst = 3000;
	m_oPhysicsData.m_bAffectedByTemperature = true;

	m_pHook = new CHook( this );
	
	m_pThrusterLeft = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/particle_thruster" + itoa2(iPlayer + 1) + ".txt" );
	m_pThrusterRight = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/particle_thruster" + itoa2(iPlayer + 1) + ".txt" );

	ResetStatus();
}

CPlayerObject::~CPlayerObject()
{
	delete m_pRadius;
	delete m_pImageDamage;
	delete m_pExplosion;
	delete m_pFrozenImage;
	delete m_pJellyImage;
	delete m_pShieldImage;
	delete m_pRespawnImage;
	delete m_pPUFrozenImage;
	delete m_pElectricImage;

	delete m_pHook;

}


void CPlayerObject::ResetStatus()
{
		m_iScore = 0;
		m_bHandleWiiMoteEvents = true;
		timeSinceNoInput = 5;
		m_fRespawnTime = 0;
		m_fPUSpeedTime = 0;
		m_fPUJellyTime = 0;
		m_fPUHealthTime = 0;
		m_fPUFreezeTime = 0;
		m_fFreezeTime = 0;
		m_pHook->SetFreezeTime(0);
		m_fPUShieldTime = 0;
		m_fElectroTime = 0;
		m_fEMPTime = 0;
		m_oPhysicsData.m_bAffectedByGravity = true;
		m_oPhysicsData.m_bHasCollision = true;

}

void CPlayerObject::SetPosition( float fX, float fY ){
	this->SetPosition( Vector(fX, fY, 0.0f) );
}

void CPlayerObject::SetPosition( Vector pos ){
	this->m_pHook->adjustPos( pos - this->GetPosition() );
	CBaseObject::SetPosition(pos);
}

void inline CPlayerObject::ApplyForceFront()
{
	float mult = 1.0f;
	if(m_fPUSpeedTime > 0.001) mult = (float)SETS->PU_SPEED_MULT;
	if(m_fEMPTime > 0.001) mult = -mult;

	dBodyAddForceAtRelPos(m_oPhysicsData.body, frontForce[0]*mult, frontForce[1]*mult, 0.0f, 0.0f, 0.0f, 0.0f);
}

bool CPlayerObject::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if (!m_bHandleWiiMoteEvents || m_fFreezeTime > 0.0f || m_fPUFreezeTime > 0.0f) return false;

	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(pWiimoteEvent, 1);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(pWiimoteEvent, 0);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) || (SETS->PITCH_ACCEL_OUT != 0.0f && m_fPitchAccel > SETS->PITCH_ACCEL_OUT) )
		{
			if ( m_pHook->m_eHookState == CONNECTED )
			{
				m_pHook->m_eHookState = EJECTING;
			}
		}
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) || (SETS->PITCH_ACCEL_IN != 0.0f && m_fPitchAccel < SETS->PITCH_ACCEL_IN) )
		{
			if ( m_pHook->m_eHookState == HOMING )
				m_pHook->m_eHookState = RETRACTING;
			else if( m_pHook->m_eHookState == SWINGING && m_pHook->m_bIsRadialCorrected)
				m_pHook->m_eHookState = THROWING;
		}

		if (WIIUSE_USING_ACC(pWiimoteEvent))
		{
			CalculateAccel( pWiimoteEvent );

			if( abs(m_fXAccel) > SETS->MIN_ACCEL_FOR_PROCESS || abs(m_fZAccel) > SETS->MIN_ACCEL_FOR_PROCESS )
			{
				m_pHook->AddChainForce(m_fXAccel, m_fZAccel);
			} else
			{
				m_pHook->AddChainForce(0, 0);
			}

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
	// Render radius
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

	if(m_fEMPTime > 0.001){
		size = m_pElectricImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pElectricImage, m_fAngle);
	
	} else {
		CBaseMovableObject::Render();		// Render schip
	}
	// Render EMP-State

	// Render Damage
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

	// Render Freeze
	if(m_fFreezeTime > 0.01f || m_fPUFreezeTime > 0.01f)
	{
		size = m_pFrozenImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale());
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pFrozenImage, m_fAngle);
	}

	// Render Jelly
	if(this->m_fPUJellyTime > 0.01f)
	{
		size = m_pJellyImage->GetSize();
		target.w = (int)(((float)size.w) * m_fSecondaryScale * GetScale());
		target.h = (int)(((float)size.h) * m_fSecondaryScale * GetScale());
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		m_pJellyImage->SetFrame(m_iJellyFrame % 15);
		RenderQuad( target, m_pJellyImage, m_fAngle);
	}

	// Render FreezePowerupDamage
	if(m_fPUFreezeTime > 0.01f)
	{
		size = m_pPUFrozenImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale()) * 2;
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale()) * 2;
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pPUFrozenImage, m_fAngle);
	}

	// Render Shield
	if(this->m_fPUShieldTime > 0.01f)
	{
		size = m_pShieldImage->GetSize();
		target.w = (int)((float)size.w * m_fSecondaryScale * GetScale()) * 2;
		target.h = (int)((float)size.h * m_fSecondaryScale * GetScale()) * 2;
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pShieldImage, m_fAngle);
	}

	// Render collisies
	if(m_fPUShieldTime < 0.01f && m_fPUJellyTime < 0.01f && this->m_fInvincibleTime < 0.01f && m_vCollisionEffects.size() > 0){

		for(unsigned int i = 0; i < m_vCollisionEffects.size(); i++)
		{
			CollisionEffect *ce = m_vCollisionEffects[i];

			target = ce->m_pEffect->GetSize();
			target.x = (int) ce->m_vPosition[0] - (target.w / 2);
			target.y = (int) ce->m_vPosition[1] - (target.w / 2);

			RenderQuad( target, ce->m_pEffect, 0);

		}
	}

	// Render explosie
	if ( m_iHitpoints <= 0 )
	{
		target = m_pExplosion->GetSize();
		target.w += target.w;
		target.h += target.h;
		target.x = (int)explosionPos[0] - (target.w / 2);
		target.y = (int)explosionPos[1] - (target.h / 2);

		RenderQuad( target, m_pExplosion, m_fExplosionAngle + 180.0f, 1 );
	}

	if ( !m_pRespawnImage->IsFinished() )
	{
		target = m_pRespawnImage->GetSize();
		target.w += target.w;
		target.h += target.h;
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);

		RenderQuad( target, m_pRespawnImage, m_fAngle, 1);
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

		if ( m_fRespawnTime < 1.0f && m_fRespawnTime + fTime > 1.0f )
		{
			Respawn();
			//SetDepth( 1.0f );
		}

		if(m_fRespawnTime >= 1.0f && m_fRespawnTime <= 2.0f){
			timeTillDeath += fTime;
			float displacement = sqrt(timeTillDeath / SETS->SPAWN_ZOOM_TIME);
			SetPosition( diePosition + respawnDisplacement*displacement );
			//SetPosition( GetPosition() + respawnDisplacement*(fTime) / SETS->SPAWN_ZOOM_TIME);
			
			if(m_fRespawnTime - 1 <= 1 - SETS->SPAWN_ZOOM_TIME) m_fRespawnTime = 1.01f;
		}

	}

	if(m_fFreezeTime > 0 || m_fPUFreezeTime > 0){
		float mult = 1.0f;
		if(m_pHook->m_pGrabbedObject != NULL
		   && m_pHook->m_pGrabbedObject->m_pOwner->getType() == ASTEROID 
		   && dynamic_cast<CAsteroid*>(m_pHook->m_pGrabbedObject->m_pOwner)->m_eAsteroidState == ON_FIRE){
			
			   mult = SETS->FIRE_AST_MULT;
		}
		if(m_fFreezeTime > 0) {m_fFreezeTime -= fTime * mult; m_pHook->SetFreezeTime(m_fFreezeTime);}
		if(m_fPUFreezeTime > 0) m_fPUFreezeTime -= fTime * mult;
	}

	if(m_fPUFreezeTime > 0){
		m_fPUFreezeTime -= fTime;
		m_pPUFrozenImage->SetFrame(30 - (int) (30 * m_fPUFreezeTime / SETS->PU_FREEZE_TIME));
	}

	if(this->m_fPUJellyTime > 0){
		m_fPUJellyTime -= fTime;
		if(m_iJellyFrame != 44) m_iJellyFrame++;
	} else {
		m_iJellyFrame = 44;
	}

	if(this->m_fPUShieldTime > 0){
		m_fPUShieldTime -= fTime;
		m_oPhysicsData.m_bAffectedByGravity = false;
		if ( m_fPUShieldTime <= 0.0f )
			m_oPhysicsData.m_bAffectedByGravity = true;
	}

	if(this->m_fPUSpeedTime > 0){
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

	if ( !m_pRespawnImage->IsFinished() ){
		m_pRespawnImage->UpdateFrame( fTime );
	} else if(!m_bIsReinitialized){
		m_bHandleWiiMoteEvents = true;
		SetDepth( -1.0f );
		m_fInvincibleTime = 2.0f;
		m_pHook->SetInvincibleTime( 2.0f );
		m_oPhysicsData.m_bAffectedByGravity = true;
		m_fAlpha = 1.0f;
		m_pHook->SetVisibility(1.0f);
		m_pHook->GetPhysicsData()->m_bHasCollision = true;
		GetPhysicsData()->m_bHasCollision = true;

		if ( m_pThrusterLeft )
		m_pThrusterLeft->ToggleSpawn(true, true);
		if ( m_pThrusterRight )
			m_pThrusterRight->ToggleSpawn(true, true);

		m_bIsReinitialized = true;


	}

	if ( m_iHitpoints <= 0 ){
		m_pExplosion->UpdateFrame( fTime );
	}

	if ( m_vCollisionEffects.size() > 0 )
	{
		for ( int i = m_vCollisionEffects.size() - 1; i>=0; i-- )
		{
			m_vCollisionEffects[i]->m_pEffect->UpdateFrame( fTime );
			if ( m_vCollisionEffects[i]->m_pEffect->IsFinished() )
			{
				delete m_vCollisionEffects[i]->m_pEffect;
				delete m_vCollisionEffects[i];
				m_vCollisionEffects.erase( m_vCollisionEffects.begin() + i );
			}
		}
	}

	if(m_fEMPTime > 0){
		m_fEMPTime -= fTime;
		m_pElectricImage->UpdateFrame(fTime);
	}

	if(!m_bElectroChangedThisFrame && m_fElectroTime > 0)
	{
		m_fElectroTime -= fTime;
	}

	m_bElectroChangedThisFrame = false;				// Set false for next frame

	if(m_fElectroTime == SETS->TIME_FOR_EMP)
	{
		m_fElectroTime = 0;
		m_pElectricImage->SetFrame(0);
		m_fEMPTime = SETS->EMP_TIME;
	} 

	CBaseMovableObject::Update( fTime );

}

void CPlayerObject::AffectedByFreezePU(){
	
	if(m_fPUJellyTime > 0){
		m_fPUJellyTime = 0;
	} else {
		if(m_fPUShieldTime <= 0){
			m_fFreezeTime = 0;
			m_pHook->SetFreezeTime(0);
			m_fPUFreezeTime = (float) SETS->PU_FREEZE_TIME;
			m_pPUFrozenImage->SetFrame(0);
		}
	}

}

void CPlayerObject::OnDie( CBaseObject *m_pKiller )
{
	m_pExplosion->SetAnimation(rand()%3);
	m_pExplosion->SetFrame(0);
	m_fExplosionAngle = m_fAngle;
	
	explosionPos = GetPosition();

	Vector nullVec;
	m_pHook->HandlePlayerDied();
	m_bHandleWiiMoteEvents = false;

	m_pHook->GetPhysicsData()->m_bHasCollision = false;
	m_pHook->GetPhysicsData()->m_bAffectedByGravity = false;
	GetPhysicsData()->m_bHasCollision = false;
	GetPhysicsData()->m_bAffectedByGravity = false;

	SetForceFront(nullVec);
	SetLinVelocity(nullVec);
	SetAngVelocity(nullVec);

	m_pHook->SetFreezeTime(0);
	m_fFreezeTime		= 0;	
	m_fInvincibleTime	= 4;
	m_fPUJellyTime		= 0;
	m_fPUShieldTime		= 0;
	m_fPUSpeedTime		= 0;
	m_fPUFreezeTime		= 0;
	m_fPUHealthTime		= 0;
	m_fEMPTime			= 0;
	m_fElectroTime		= 0;
	m_fRespawnTime		= 3;

	m_pHook->SetInvincibleTime(4);
	SetAlpha(0);
	m_pHook->SetVisibility(0);
	// Spawn emitter
	Vector direction = m_pKiller->GetPosition() - GetPosition();
	direction.Normalize();

	int r = rand()%4;
	CSound *pSound = NULL;
	if ( r == 0 )
		pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Explosion1.wav", RT_SOUND);
	if ( r == 1 )
		pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Explosion2.wav", RT_SOUND);
	if ( r == 2 )
		pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Explosion3.wav", RT_SOUND);
	if ( r == 3 )
		pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/Explosion4.wav", RT_SOUND);
	if ( pSound )
		pSound->Play();

	if ( m_pThrusterLeft )
		m_pThrusterLeft->ToggleSpawn(true, false);
	if ( m_pThrusterRight )
		m_pThrusterRight->ToggleSpawn(true, false);


	// Get respawnposition
	int x, y;

	CRenderer *pRenderer = CRenderer::Instance();
	CUniverse* universe = CODEManager::Instance()->m_pUniverse;
	do
	{
		x = rand()%((int) universe->m_fWidth*2) -  (int) universe->m_fWidth;
		y = rand()%((int) universe->m_fHeight*2) - (int) universe->m_fHeight;
	} while ( pRenderer->ObjectsInRange( x, y, 200 ) );

	respawnDisplacement = Vector( (float)x, (float)y, 0 ) - GetPosition();
	diePosition = GetPosition();
	timeTillDeath = 0;

	Vector n;
	m_oPhysicsData.m_pOwner->SetLinVelocity(n);
	m_oPhysicsData.m_pOwner->SetAngVelocity(n);
	SetForce(n);

}

void CPlayerObject::Respawn()
{
	m_iHitpoints = m_iMaxHitpoints;
	m_pRespawnImage->SetAnimation(0);
	m_pRespawnImage->SetFrame(0);
	m_bIsReinitialized = false;


}

void CPlayerObject::TookHealthPowerUp(){ m_iHitpoints	 = m_iMaxHitpoints; }
void CPlayerObject::TookSpeedPowerUp() { m_fPUSpeedTime  = (float) SETS->PU_SPEED_TIME; }
void CPlayerObject::TookJellyPowerUp() { m_fFreezeTime = 0; m_pHook->SetFreezeTime(0); m_fPUFreezeTime = 0; m_fPUJellyTime  = (float) SETS->PU_JELLY_TIME;  m_fPUShieldTime = 0; }
void CPlayerObject::TookShieldPowerUp(){ m_fFreezeTime = 0; m_pHook->SetFreezeTime(0); m_fPUFreezeTime = 0; m_fPUShieldTime = (float) SETS->PU_SHIELD_TIME; m_fPUJellyTime = 0; }


void CPlayerObject::IncreaseElectro(float timeIncrease)
{
	if(m_fEMPTime > 0) return;

	m_bElectroChangedThisFrame = true;

	int chance = rand() % 10;
	int length = rand() % (int) this->GetPhysicsData()->m_fRadius;
	int angle  = rand() % 360;

	if(chance < timeIncrease * 10){
		CollisionEffect *ce = new CollisionEffect();
		ce->m_pEffect = new CAnimatedTexture("media/scripts/texture_spark.txt");
		ce->m_vPosition = GetPosition() + Vector::FromAngleLength((float) angle, (float) length);
		ce->m_pOther = NULL;
		m_vCollisionEffects.push_back(ce);
	}

	m_fElectroTime += timeIncrease;
	
	if(m_fElectroTime > SETS->TIME_FOR_EMP)
		m_fElectroTime = SETS->TIME_FOR_EMP;

}



void CPlayerObject::CollideWith( CBaseObject *pOther, Vector &pos)
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

	if(oType == ICE || oType == ASTEROID || oType == SUN || oType == FIRE || oType == ORDINARY || oType == WORMHOLE || oType == ELECTRO)
		mult = dynamic_cast<CPlanet*>(pOther)->m_fDamageMult;

	if( pOther->getType() == ASTEROID)
	{

		if(m_fPUJellyTime > 0.001f && m_iJellyFrame == 44){
			m_iJellyFrame = 0;			// Start bounce-animatie
		}


		if ( !HasSpark( pOther ) )
		{
			CollisionEffect *ce = new CollisionEffect();
			ce->m_pEffect = new CAnimatedTexture("media/scripts/texture_hit.txt");
			ce->m_vPosition = pos;
			ce->m_pOther = pOther;
			m_vCollisionEffects.push_back(ce);
		}

		CAsteroid* asteroid = dynamic_cast<CAsteroid*>(pOther);
		time_t throwTime = time(NULL) - asteroid->m_fThrowTime;
		if(throwTime <= 4)
		{
			if(asteroid->m_eAsteroidState == ON_FIRE)
			{
				mult = SETS->FIRE_DAMAGE_MULT;
				m_fPUJellyTime = 0;

				CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/combust_object.wav", RT_SOUND);
				if ( pSound )
					pSound->Play();
			}
			else if(asteroid->m_eAsteroidState == FROZEN)
			{
				mult = SETS->ICE_DAMAGE_MULT;
				m_fPUFreezeTime = 0;
				m_fFreezeTime = SETS->FREEZE_TIME;
				m_pHook->SetFreezeTime(SETS->FREEZE_TIME);
				m_fPUJellyTime = 0;

				CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/freeze_object.wav", RT_SOUND);
				if ( pSound )
					pSound->Play();
			}

			//int score = 0;
			if(asteroid->m_pThrowingPlayer && asteroid->m_pThrowingPlayer != this && m_fPUJellyTime <= 0.0001f)
			{
				int damageMult = (int) (damage * mult);
				int score = (damageMult > m_iHitpoints ? m_iHitpoints : damageMult );
				asteroid->m_pThrowingPlayer->m_iScore += score;
				asteroid->m_pThrowingPlayer->m_iScore += asteroid->m_iMilliSecsInOrbit / 10;
				score += asteroid->m_iMilliSecsInOrbit / 10;
				CGameState *pState = (CGameState *)CCore::Instance()->GetActiveState();
				pState->AddScore( asteroid->m_pThrowingPlayer->GetPlayerID(), score, (int)GetX(), (int)GetY() );
			}

		}
	}

	if(m_fPUJellyTime > 0) return; 
	m_iHitpoints -= (int) (damage * mult);

	if ( m_iHitpoints <= 0 )
		m_iHitpoints = 0;

	if ( m_iHitpoints == 0 && iOldHitpoints > 0 )
	{
		CLogManager::Instance()->LogMessage( "Object died" );
		if(pOther->getType() == SHIP){
			CPlayerObject* otherPlayer = dynamic_cast<CPlayerObject*>(pOther);
			otherPlayer->m_iScore += SETS->SCORE_PUSH_DEAD;
			CGameState *pState = (CGameState *)CCore::Instance()->GetActiveState();
			pState->AddScore( otherPlayer->GetPlayerID(), SETS->SCORE_PUSH_DEAD, (int)GetX(), (int)GetY() );
		}
		
		OnDie( pOther );
	}

	//CBaseObject::CollideWith(pOther, force);
}

bool CPlayerObject::HasSpark( CBaseObject *pOther )
{
	for ( unsigned int i = 0; i<m_vCollisionEffects.size(); i++ )
	{
		if ( m_vCollisionEffects[i]->m_pOther == pOther )
			return true;
	}
	return false;
}
