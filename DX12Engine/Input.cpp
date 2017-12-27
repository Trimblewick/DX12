#include "Input.h"

Input::Input()
{
	this->UpdateClientRectPos();
	SetCursorPos(m_fWindowCenterPosX, m_fWindowCenterPosY);
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
	if (keyIndex >= 0 && keyIndex <= 256)
		m_bKeys[keyIndex] = true;
	
	return;
}


void Input::KeyUp(unsigned int const keyIndex)
{
	// If a key is released then clear that state in the key array.
	if (keyIndex >= 0 && keyIndex <= 256)
		m_bKeys[keyIndex] = false;
	
	return;
}


bool Input::IsKeyDown(unsigned int const keyIndex)
{
	// Return what state the key is in (pressed/not pressed).
	//assert(keyIndex >= 0 && keyIndex <= 256);
		
	return m_bKeys[keyIndex];
	
}

void Input::UpdateMouse()
{
	POINT p;

	if (GetCursorPos(&p))
	{
		float x = p.x;
		float y = p.y;

		m_mouseDelta.x = x - m_fWindowCenterPosX;
		m_mouseDelta.y = y - m_fWindowCenterPosY;
		

		if (!IsKeyDown(Input::NINE))
			SetCursorPos(m_fWindowCenterPosX, m_fWindowCenterPosY);
	}

	return;
}

void Input::UpdateClientRectPos()
{
	RECT rect = WindowClass::GetClientRect();
	m_fWindowCenterPosX = rect.left + (WindowClass::GetWidth() / 2.0f);
	m_fWindowCenterPosY = rect.top + (WindowClass::GetHeight() / 2.0f);

}

DirectX::XMFLOAT2 Input::GetMouseDelta()
{
	return m_mouseDelta;
}
