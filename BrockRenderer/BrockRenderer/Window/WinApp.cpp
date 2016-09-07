#include "stdafx.h"
#include <fstream>
namespace Window
{

LRESULT WINAPI WinApp::handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static Window::MouseEvent mouseEvent;
    static Window::KeyboardEvent keyboardEvent(0);

    switch (msg)
    {
    case WM_KEYDOWN:
        memset(&keyboardEvent, 0, sizeof(Window::KeyboardEvent));
        keyboardEvent.setKeyCode(wp);

        onKeyPressed(keyboardEvent);

        return 0;

    case WM_KEYUP:
        memset(&keyboardEvent, 0, sizeof(Window::KeyboardEvent));
        keyboardEvent.setKeyCode(wp);

        onKeyReleased(keyboardEvent);
        return 0;

    case WM_MOUSEMOVE:
        memset(&mouseEvent, 0, sizeof(Window::MouseEvent));
        mouseEvent.x = LOWORD(lp);
        mouseEvent.y = HIWORD(lp);
        mouseEvent.buttonsState = wp & (MK_LBUTTON | MK_RBUTTON);
        if (wp & MK_MBUTTON)
            mouseEvent.buttonsState |= Window::MouseEvent::MIDDLE_PRESSED;
		
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
    
	//m_clientController = std::make_shared<base::Controller>(argc, argv);
	g_camera = new Camera;
	g_context = new Context();
	g_context->Render();
   // m_clientController->createViewport<WindowsViewport>();
}
WinApp::~WinApp()
{

}

void WinApp::onResize(int w, int h)
{
  //  m_clientController->resize(w, h);
}

int WinApp::run()
{
    MSG msg;
    int retVal = 0;

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
                return msg.wParam;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            retVal = WinBase::run();
            if (retVal)
                return retVal;
        }
    }

}

bool MOVING = false;
void WinApp::onMouseEvent( const MouseEvent &ev )
{
	static int yaw, pitch;
	static int prevx, prevy;

	if (ev.isLeftPressed())
	{
		yaw = prevx - ev.x;            // inverted
		pitch = prevy - ev.y;

		if(MOVING)
		{
			g_camera->m_viewMatrix = g_camera->m_viewMatrix * mat4::GetRotateY(DegToRad(-yaw/5.0))* mat4::GetRotateX(DegToRad(pitch/5.0));
		}
		MOVING = true;
		// m_playerCamera->setEulerAnglesRotation(yaw, pitch, 0.f);
	}
	else
	{
		MOVING = false;
		return;
	}
	prevx = ev.x;
	prevy = ev.y;

	g_context->Render();
}

void WinApp::onKeyPressed( const KeyboardEvent &ev )
{
	static const float velocity = 1.0;
	//    static math::vec3 ds;
	//    static math::vec3 position;

	// ds.zero();
	// position = m_playerCamera->getPosition();

	if (ev.keycode() == Window::KEY_KEY_W)
	{
		g_camera->Move(g_camera->GetZ()*velocity);

		g_context->Render();
	}
	else if (ev.keycode() == Window::KEY_KEY_S)
	{
		//ds = m_playerCamera->getDirection() * velocity;
		//       position -= ds;
		g_camera->Move(g_camera->GetZ()*-velocity);

		g_context->Render();
	}
	else if (ev.keycode() == Window::KEY_KEY_D)
	{
		//ds = m_playerCamera->getRightVector() * velocity;
		//        position += ds;
		g_camera->Move(g_camera->GetX()*velocity);

		g_context->Render();
	}
	else if (ev.keycode() == Window::KEY_KEY_A)
	{
		g_camera->Move(g_camera->GetX()*-velocity);

		g_context->Render();
	}
	else if (ev.keycode() == Window::KEY_KEY_Q)
	{
		g_camera->Move(g_camera->GetY()*velocity);

		g_context->Render();
	}
	else if (ev.keycode() == Window::KEY_KEY_E)
	{
		g_camera->Move(g_camera->GetY()*-velocity);

		g_context->Render();
	}
	else
		return;
}

void WinApp::onFrameStart()
{

}

void WinApp::onFrameEnd()
{

}

void WinApp::onKeyReleased( const KeyboardEvent &ev )
{

}

void WinApp::update( float dt )
{

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CREATE)
    {
        LPCREATESTRUCT cs = (LPCREATESTRUCT)lp;
        WinApp *param = (WinApp*)cs->lpCreateParams;
        SetWindowLong(hwnd, GWL_USERDATA, (long)param);
    }
    else
    {
        if (msg == WM_DESTROY)
        {

        }

        WinApp* app = (WinApp*)GetWindowLong(hwnd, GWL_USERDATA);
        if (app)
            return app->handleMessage(hwnd, msg, wp, lp);
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

WindowsViewport::WindowsViewport(int width, int height):m_width(width), m_height(height)
{
    const TCHAR *className = TEXT("wc.MainWindow");

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = className;
    wcex.hIcon = 0;
    wcex.hIconSm = 0;
    wcex.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
    wcex.hInstance = GetModuleHandle(0);//app->getAppInstance();
    wcex.lpfnWndProc = (WNDPROC)WndProc;

	RegisterClassEx(&wcex);

    RECT viewportRect;
    viewportRect.left = 0;
    viewportRect.top = 0;
    viewportRect.right = width;
    viewportRect.bottom = height;

    AdjustWindowRect(&viewportRect, WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX, false);

    HDC desktopDC = GetDC(GetDesktopWindow());
    int screenWidth = GetDeviceCaps(desktopDC, HORZRES);
    int screenHeight = GetDeviceCaps(desktopDC, VERTRES);
    ReleaseDC(GetDesktopWindow(), desktopDC);

    m_hWnd = CreateWindowEx(0, className, L"Main window", WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX,
        screenWidth / 2 - width / 2, screenHeight / 2 - height / 2,
        viewportRect.right - viewportRect.left + 1,
        viewportRect.bottom - viewportRect.top + 1,
        0, 0, GetModuleHandle(0), WinBase::instance());

    if (!m_hWnd)
    {
		DWORD e = GetLastError();
		std::ofstream LOG("error.log");
		LOG<< e ;
		LOG.close();
        UnregisterClass(className, GetModuleHandle(0));
    }


    m_hDC = GetDC(m_hWnd);

    ShowWindow(m_hWnd, SW_SHOW);
}

WindowsViewport::~WindowsViewport()
{
    if (m_hDC)
        ReleaseDC(m_hWnd, m_hDC);

    if (m_hWnd)
        DestroyWindow(m_hWnd);
}

void WindowsViewport::flush( unsigned char * pixels)
{
    if (!m_hDC)
        return;

    static BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));

    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = m_width;
    bmi.bmiHeader.biHeight      = -m_height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = 0;

    SetDIBitsToDevice(m_hDC,
        0, 0,
        m_width, m_height,
        0, 0,
        0, m_height,
        pixels,
        &bmi,
        DIB_RGB_COLORS);
}

}