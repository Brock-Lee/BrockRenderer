#include "stdafx.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, char *, int cmdShow)
{

try
{
	g_scene = new Scene();
	Triangle t;
	t.v[0].position = vec3(0.5,-0.5,0.5);
	t.v[1].position = vec3(-0.5,-0.5,0.5);
	t.v[2].position = vec3(0.0,0.5,0.5);
	t.v[0].uv = vec2(0.5,-0.5);
	t.v[1].uv = vec2(-0.5,-0.5);
	t.v[2].uv = vec2(0.0, 0.5);
	g_scene->AddTriangles("", t);
    Window::WinApp app;

    return app.run();
}
catch (...)
{
	assert(0);
	return 1;
}
}