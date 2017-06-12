#include "Input.h"

Input::Input()
{
}

Input::~Input()
{
}


bool Input::Initialize()
{
	// Initialize all the keys to being released and not pressed.
	for (unsigned int i = 0; i < 256; ++i)
	{
		m_bKeys[i] = false;
	}

	return true;
}


void Input::KeyDown(unsigned int const keyIndex)
{
	// If a key is pressed then save that state in the key array.
	m_bKeys[keyIndex] = true;
	
	return;
}


void Input::KeyUp(unsigned int const keyIndex)
{
	// If a key is released then clear that state in the key array.
	int test = keyIndex;
	m_bKeys[keyIndex] = false;
	
	return;
}


bool Input::IsKeyDown(unsigned int const keyIndex)
{
	// Return what state the key is in (pressed/not pressed).
	return m_bKeys[keyIndex];
}
