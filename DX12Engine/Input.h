#ifndef INPUT_H
#define INPUT_H

#include <iostream>

class Input 
{
	
public:
	Input();
	~Input();

	void Initialize();

	void KeyDown(unsigned int const keyIndex);
	void KeyUp(unsigned int const keyIndex);

	bool IsKeyDown(unsigned int const keyIndex);

private:
	bool m_bKeys[256];
};

#endif