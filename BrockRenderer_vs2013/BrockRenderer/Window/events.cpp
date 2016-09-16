#include "stdafx.h"

KeyboardEvent::KeyboardEvent(unsigned long keycode)
{
    setKeyCode(keycode);
}

void KeyboardEvent::setKeyCode(unsigned long keycode)
{
    m_keycode = (KEY_CODE)keycode; 
}

KEY_CODE KeyboardEvent::keycode() const
{
    return m_keycode;
}

