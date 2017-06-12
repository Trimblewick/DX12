#pragma once

class Input 
{
	
public:
	Input();
	~Input();

	bool Initialize();

	void KeyDown(unsigned int const keyIndex);
	void KeyUp(unsigned int const keyIndex);

	bool IsKeyDown(unsigned int const keyIndex);

private:

	bool m_bKeys[256];
};
