#ifndef BASEAPP_H
#define BASEAPP_H

namespace Window
{

struct MouseEvent;
struct KeyboardEvent;

class WinBase
{
protected:
    static WinBase *m_this;

    static void exit() { ::exit(0); }

    //! This func calls every frame.
    /** Receives dt in msecs*/
    virtual void update(float dt) = 0;

public:
    static WinBase *instance() { return m_this; }

    WinBase();
    virtual ~WinBase();

    virtual void onFrameStart() = 0;
    virtual void onFrameEnd() = 0;

    virtual void onMouseEvent(const MouseEvent &ev) = 0;

    virtual void onKeyPressed(const KeyboardEvent &ev) = 0;
    virtual void onKeyReleased(const KeyboardEvent &ev) = 0;

    virtual void onResize(int w, int h) = 0;

    // TODO: rename to run frame
    virtual int run();
};

}

#endif // BASEAPP_H
