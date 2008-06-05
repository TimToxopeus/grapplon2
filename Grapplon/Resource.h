#pragma once

#include <string>

enum RTYPE
{
	RT_GENERIC, // Generic resource, unloaded
	RT_TEXTURE, // ResourceType Texture
	RT_SOUND, // ResourceType Sound
};

class IResource
{
protected:
	std::string m_szName;
	RTYPE m_eType;

public:
	IResource( std::string szName ) { m_szName = szName; m_eType = RT_GENERIC; }
	virtual ~IResource() {}

	RTYPE GetType() { return m_eType; }
	std::string GetName() { return m_szName; }
};
