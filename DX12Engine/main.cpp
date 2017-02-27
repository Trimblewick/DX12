
#include <Windows.h>
#include <exception>
#include "SystemClass.h"


//LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system;
	system = new SystemClass();
	if (!system->Initialize())
	{
		std::cout << "system init failiure";
	}

	system->Run();
	


	// End application
	//system->Shutdown();
	delete system;
	system = 0;
	
	return 0;
}