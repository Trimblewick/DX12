
#include <Windows.h>
#include <exception>
#include "SystemClass.h"


int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system;
	system = new SystemClass();
	if (system->Initialize())
	{
		system->Run();
	}


	// End application
	system->Shutdown();
	delete system;
	system = 0;
	
	return 0;
}