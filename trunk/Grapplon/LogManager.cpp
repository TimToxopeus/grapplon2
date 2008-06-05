#include "LogManager.h"
#include <stdio.h>
#include <sstream>
#include <time.h>

CLogManager *CLogManager::m_pInstance = 0;

CLogManager::CLogManager()
{
	// Reset log
	FILE *pFile = fopen("log.txt", "wt+");
	if ( pFile )
		fclose(pFile);
}

CLogManager::~CLogManager()
{
}

std::string itoa2(const int x)
{
  std::ostringstream o;
  if (!(o << x)) return "ERROR";
  return o.str();
}

std::string ftoa2(const float x)
{
  std::ostringstream o;
  if (!(o << x)) return "ERROR";
  return o.str();
}

void CLogManager::LogMessage( std::string message )
{
	FILE *pFile = fopen("log.txt", "at+");
	if ( pFile )
	{
		//Find the current time
		time_t curtime = time(0);

		//convert it to tm
		tm now=*localtime(&curtime);

		//Format string determines the conversion specification's behaviour
		const char format[]="%H:%M:%S";

		//strftime - converts date and time to a string
		char dest[32]={0}; // Buffer
		if (strftime(dest, sizeof(dest)-1, format, &now)>0)
			fprintf( pFile, "[%s] %s\n", dest, message.c_str() );
		else // Still print log message if time is unavailable..
			fprintf( pFile, "%s\n", message.c_str() );

		fclose(pFile);
	}
}
