#include "stdafx.h"
#include <fstream>

WinApp* WinApp::m_this = nullptr;
std::mutex WinApp::m_mutex;
WinApp* WinApp::Instance()
{
	if (m_this == nullptr)
	{
		m_mutex.lock();
		if (m_this == nullptr)
			m_this = new WinApp();
		m_mutex.unlock();
	}
	return m_this;
}
LRESULT WINAPI WinApp::handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static MouseEvent mouseEvent;
    static KeyboardEvent keyboardEvent(0);

    switch (msg)
    {
    case WM_KEYDOWN:
        memset(&keyboardEvent, 0, sizeof(KeyboardEvent));
        keyboardEvent.setKeyCode(wp);

        onKeyPressed(keyboardEvent);

        return 0;

    case WM_KEYUP:
        memset(&keyboardEvent, 0, sizeof(KeyboardEvent));
        keyboardEvent.setKeyCode(wp);

        onKeyReleased(keyboardEvent);
        return 0;

    case WM_MOUSEMOVE:
        memset(&mouseEvent, 0, sizeof(MouseEvent));
        mouseEvent.x = LOWORD(lp);
        mouseEvent.y = HIWORD(lp);
        mouseEvent.buttonsState = wp & (MK_LBUTTON | MK_RBUTTON);
        if (wp & MK_MBUTTON)
            mouseEvent.buttonsState |= MIDDLE_PRESSED;
		
        onMouseEvent(mouseEvent);
		
		
        return 0;
	
    case WM_MOUSEWHEEL:

        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        onResize(LOWORD(lp), HIWORD(lp));
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

WinApp::WinApp()
{
}
WinApp::~WinApp()
{

}

void WinApp::onResize(int w, int h)
{

}

void WinApp::run()
{
    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
                return;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			onFrameStart();
			update();       // update user callback
			onFrameEnd();
        }
    }

}


void WinApp::onMouseEvent( const MouseEvent &ev )
{
	static int yaw, pitch;
	static int prevx, prevy;
	static bool MOVING = false;
	if (ev.isLeftPressed())
	{
		yaw = prevx - ev.x;         
		pitch = prevy - ev.y;

		if(MOVING)
		{
			g_camera->m_viewMatrix = g_camera->m_viewMatrix * mat4::GetRotateY(DegToRad(-yaw/5.0))* mat4::GetRotateX(DegToRad(pitch/5.0));
		}
		MOVING = true;
	}
	else
	{
		MOVING = false;
		return;
	}
	prevx = ev.x;
	prevy = ev.y;
}

void WinApp::onKeyPressed( const KeyboardEvent &ev )
{
	float velocity = g_scene->m_aabb.GetMaxLength()/50.0;

	if (ev.keycode() == KEY_KEY_W)
	{
		g_camera->Move(g_camera->GetZ()*velocity);
	}
	else if (ev.keycode() == KEY_KEY_S)
	{
		g_camera->Move(g_camera->GetZ()*-velocity);
	}
	else if (ev.keycode() == KEY_KEY_D)
	{
		g_camera->Move(g_camera->GetX()*velocity);
	}
	else if (ev.keycode() == KEY_KEY_A)
	{
		g_camera->Move(g_camera->GetX()*-velocity);
	}
	else if (ev.keycode() == KEY_KEY_Q)
	{
		g_camera->Move(g_camera->GetY()*velocity);
	}
	else if (ev.keycode() == KEY_KEY_E)
	{
		g_camera->Move(g_camera->GetY()*-velocity);
	}
	else if (ev.keycode() == KEY_KEY_F)
	{
		g_context->m_lineMode = !g_context->m_lineMode;
	}
	else if (ev.keycode() == KEY_ESCAPE)
	{
		::exit(0);
	}
	else
		return;
}

void WinApp::onFrameStart()
{
	g_timer->Start();
	g_context->Clear();
}

void WinApp::onFrameEnd()
{ 
	g_timer->Stop();
	g_context->Flush();
	g_context->SwapBuffer();
}

void WinApp::onKeyReleased( const KeyboardEvent &ev )
{

}

void WinApp::update()
{
	g_context->Draw();
}


