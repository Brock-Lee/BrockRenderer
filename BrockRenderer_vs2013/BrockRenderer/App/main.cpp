#include "stdafx.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, char *, int cmdShow)
{
	g_scene = new Scene();
	g_scene->LoadModel("Data/cj6.obj");
	g_timer = new Timer();
	g_camera = new Camera();
	g_context = new Context();
	WinApp::Instance()->run();
	return 0;
}