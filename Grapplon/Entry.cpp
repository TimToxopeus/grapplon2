//#pragma comment( lib, "emapi" )

#include <windows.h>

#include "Core.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT cmdShow )
{
	CCore *pCore = CCore::Instance();
	if ( pCore->SystemsInit() )
		pCore->Run();
	pCore->SystemsDestroy();
	CCore::Destroy();
	return 0;
}
