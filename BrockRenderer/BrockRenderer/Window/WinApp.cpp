#include "stdafx.h"

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

WinApp::WinApp(int argc, const char **argv)
{
    //m_clientController = std::make_shared<base::Controller>(argc, argv);
	m_vp = new Window::WindowsViewport(1000,1000);
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

unsigned char pixels[1000][1000][4];
void WinApp::onMouseEvent( const MouseEvent &ev )
{
	static int yaw, pitch;
	static int prevx, prevy;

	if (ev.isLeftPressed())
	{
		yaw -= prevx - ev.x;            // inverted
		pitch -= prevy - ev.y;

		if (abs(yaw) > 360) yaw %= 360;
		if (abs(pitch) > 360) pitch %= 360;

		// m_playerCamera->setEulerAnglesRotation(yaw, pitch, 0.f);
	}

	prevx = ev.x;
	prevy = ev.y;
	//	memset(pixels,0, sizeof(pixels));
	pixels[ev.x ][ev.y][0] = 255;
	pixels[ev.x ][ev.y][1] = 0;
	pixels[ev.x ][ev.y][2] = 0;
	pixels[ev.x ][ev.y][3] = 255;
	m_vp->flush(&pixels[0][0][0]);
}

void WinApp::onKeyPressed( const KeyboardEvent &ev )
{
	static const float velocity = 100.f;
	//    static math::vec3 ds;
	//    static math::vec3 position;

	// ds.zero();
	// position = m_playerCamera->getPosition();

	if (ev.keycode() == Window::KEY_KEY_W)
	{
		//ds = m_playerCamera->getDirection() * velocity;
		//        position += ds;
	}
	else if (ev.keycode() == Window::KEY_KEY_S)
	{
		//ds = m_playerCamera->getDirection() * velocity;
		//       position -= ds;
	}
	else if (ev.keycode() == Window::KEY_KEY_D)
	{
		//ds = m_playerCamera->getRightVector() * velocity;
		//        position += ds;
	}
	else if (ev.keycode() == Window::KEY_KEY_A)
	{
		// ds = m_playerCamera->getRightVector() * velocity;
		//        position -= ds;
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

	assert(RegisterClassEx(&wcex));

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
		auto e = GetLastError();
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