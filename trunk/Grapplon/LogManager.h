#pragma once

#include <string>
extern std::string itoa2(const int x);
extern std::string ftoa2(const float x);

class CLogManager
{
private:
	static CLogManager *m_pInstance;
	CLogManager();
	virtual ~CLogManager();

public:
	static CLogManager *Instance() { if ( !m_pInstance ) m_pInstance = new CLogManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void LogMessage( std::string message );
};
