#ifndef BASEAPPWIN_H
#define BASEAPPWIN_H


namespace Window
{
class WindowsViewport
{
	HWND m_hWnd;
	HDC  m_hDC;
	//! Screen width.
	int m_width;
	//! Screen height.
	int m_height;
public:
	WindowsViewport(int width, int height);
	~WindowsViewport();

	void flush(unsigned char * pixels);
};
class WinApp : public WinBase
{
protected:
    virtual void update(float dt);
public:
    WinApp();
    virtual ~WinApp();

    virtual LRESULT WINAPI handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    virtual void onFrameStart();
    virtual void onFrameEnd();

    virtual void onMouseEvent(const MouseEvent &ev);

    virtual void onKeyPressed(const KeyboardEvent &ev);
    virtual void onKeyReleased(const KeyboardEvent &ev);

    virtual void onResize(int w, int h);

    int run();
};

}

#endif // BASEAPPWIN_H