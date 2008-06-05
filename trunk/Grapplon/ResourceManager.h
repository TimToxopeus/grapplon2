#pragma once

#include <vector>
#include <string>

// Forward declaration
class IResource;
enum RTYPE;

class CResourceManager
{
private:
	static CResourceManager *m_pInstance;
	CResourceManager();
	virtual ~CResourceManager();

	std::vector<IResource *> m_vResources;

	IResource *LoadTexture( std::string name );
	IResource *LoadSound( std::string name );

public:
	static CResourceManager *Instance() { if ( !m_pInstance ) m_pInstance = new CResourceManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	IResource *GetResource( std::string name, RTYPE resourceType );
};
