#pragma once

#include <SDL.h>
#include <string>

class CAnimatedTexture;

enum ObjectType { UNSET = 0, STATE, HUD, PARTICLESYSTEM, SHIP, HOOK, ORDINARY, ASTEROID, SUN, ICE, BROKEN, CHAINLINK, FIRE, POWERUP };

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
	std::string getTypeStr(){ 
		if(m_eType == UNSET)
			return "UNSET";
		if(m_eType == STATE)
			return "STATE";
		if(m_eType == PARTICLESYSTEM)
			return "PARTICLESYSTEM";
		if(m_eType == SHIP)
			return "SHIP";
		if(m_eType == HOOK)
			return "HOOK";
		if(m_eType == ORDINARY)
			return "ORDINARY";
		if(m_eType == ICE)
			return "ICE";
		if(m_eType == SUN)
			return "SUN";
		if(m_eType == BROKEN)
			return "BROKEN";
		if(m_eType == CHAINLINK)
			return "CHAINLINK";
	}

	virtual bool ShouldBeDeleted() { return m_bDeleteMe; }
};
