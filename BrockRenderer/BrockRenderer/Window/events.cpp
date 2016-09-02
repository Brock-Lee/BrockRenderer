#include "stdafx.h"

namespace Window
{

KeyboardEvent::KeyboardEvent(unsigned long keycode)
{
    setKeyCode(keycode);
}

void KeyboardEvent::setKeyCode(unsigned long keycode)
{
    m_keycode = (KEY_CODE)keycode;  // windows keycode
}

KEY_CODE KeyboardEvent::keycode() const
{
    return m_keycode;
}

}
