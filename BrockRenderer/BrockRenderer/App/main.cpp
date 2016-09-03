#include "stdafx.h"

int main(int argc, const char **argv)
{

try
{
    Window::WinApp app(argc, argv);
	Triangle t;
	t.p[0] = vec3(0.5,-0.5,0.5);
	t.p[1] = vec3(-0.5,-0.5,0.5);
	t.p[2] = vec3(0.0,0.5,0.5);
	g_scene->AddTriangles("", t);
    return app.run();
}
catch (...)
{
	assert(0);
	return 1;
}
}