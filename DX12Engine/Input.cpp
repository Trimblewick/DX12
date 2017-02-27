#include "Input.h"

Input::Input()
{
}




Input::~Input()
{
}


void Input::Initialize()
{
	int i;

	// Initialize all the keys to being released and not pressed.
	for (i = 0; i<256; i++)
	{
		this->keys[i] = false;
	}

	return;
}


void Input::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	this->keys[input] = true;
	
	return;
}


void Input::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	this->keys[input] = false;
	
	return;
}


bool Input::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return this->keys[key];
}
