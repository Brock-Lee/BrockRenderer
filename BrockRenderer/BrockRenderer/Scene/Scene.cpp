#include "stdafx.h"

Scene* g_scene;
Camera* g_camera;
void Scene::AddPoint( const vec3 &p )
{
	m_points.push_back(p);
}

void Scene::AddTriangles(const string texture)
{
	m_lines.push_back(line);
}
