#include "stdafx.h"



namespace Window
{

WinBase *WinBase::m_this = 0;

WinBase::WinBase()
{
    m_this = this;
}

WinBase::~WinBase()
{
}

int WinBase::run()
{


    static long prevTime, currTime;
    onFrameStart();

    //m_clientController->update();
    update(float(currTime - prevTime));       // update user callback

    prevTime = currTime;

    onFrameEnd();

    return 0;
}


}
