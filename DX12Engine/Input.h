#ifndef INPUT_H
#define INPUT_H

#include <iostream>

class Input 
{
	
public:
	Input();
	//InputClass(const InputClass&);
	~Input();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool keys[256];
};

#endif