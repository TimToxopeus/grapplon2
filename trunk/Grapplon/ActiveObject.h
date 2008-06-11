#pragma once

#include <SDL.h>
#include <string>

class CAnimatedTexture;

enum ObjectType { UNSET = 0, STATE, HUD, PARTICLESYSTEM, SHIP, HOOK, ORDINARY, ASTEROID, SUN, ICE, BROKEN, CHAINLINK, FIRE, POWERUP, WORMHOLE, ELECTRO };

class IActiveObject
{
private:
	float m_fDepth;
	float m_fScale;

protected:
	ObjectType m_eType;
	std::string ObjectTypeStr;
	float m_fAlpha;
	bool m_bDeleteMe;

public:
	IActiveObject();
	virtual ~IActiveObject();

	void RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha = -1.0f );
	void RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha = -1.0f );
	virtual void Render() = 0;

	virtual void Update( float fTime ) = 0;
	virtual float GetDepth() const { return m_fDepth; }
	virtual void SetDepth( float fDepth );

	virtual float GetScale() const { return m_fScale; }
	virtual void SetScale( float fScale ) { m_fScale = fScale; }

	virtual float GetAlpha() const { return m_fAlpha; }
	virtual void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }

	virtual ObjectType getType() { return m_eType; }
	std::string getTypeStr()
	{
		switch ( m_eType )
		{
		default:
		case UNSET:
			return "UNSET";
		case STATE:
			return "STATE";
		case HUD:
			return "HUD";
		case PARTICLESYSTEM:
			return "PARTICLESYSTEM";
		case SHIP:
			return "SHIP";
		case HOOK:
			return "HOOK";
		case ORDINARY:
			return "ORDINARY";
		case ASTEROID:
			return "ASTEROID";
		case SUN:
			return "SUN";
		case ICE:
			return "ICE";
		case BROKEN:
			return "BROKEN";
		case CHAINLINK:
			return "CHAINLINK";
		case FIRE:
			return "FIRE";
		case POWERUP:
			return "POWERUP";
		case WORMHOLE:
			return "WORMHOLE";
		case ELECTRO:
			return "ELECTRO";
		}
	}


	virtual bool ShouldBeDeleted() { return m_bDeleteMe; }
};
