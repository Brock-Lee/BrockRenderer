#pragma once

class WinApp
{
private:
	static WinApp *m_this;
	static std::mutex m_mutex;
	WinApp();

	virtual void onMouseEvent(const MouseEvent &ev);
	virtual void onKeyPressed(const KeyboardEvent &ev);
	virtual void onKeyReleased(const KeyboardEvent &ev);
	virtual void onResize(int w, int h);

	virtual void onFrameStart();
	virtual void update();
	virtual void onFrameEnd();

public:
	static WinApp *Instance();
    virtual ~WinApp();
	// I/O
	virtual LRESULT WINAPI handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	// Render Loop
    virtual void run();
};

