#include "stdafx.h"


void init()
{
	g_scene = new Scene();
	g_scene->LoadModel("Data/cj6.obj");
	g_timer = new Timer();
	g_camera = new Camera();
	g_context = new Context();
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, char *, int cmdShow)
{
	init();
	// render loop begins
	WinApp::Instance()->run();
	return 0;
}