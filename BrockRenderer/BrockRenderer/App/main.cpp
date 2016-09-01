#include "stdafx.h"

int main(int argc, const char **argv)
{

try
{
    Window::WinApp app(argc, argv);

    return app.run();
}
catch (...)
{
	assert(0);
	return 1;
}
}