#include "stdafx.h"

Scene* g_scene;
Camera* g_camera;
using namespace std;
void Scene::AddPoint( const vec3 &p )
{
	m_points.push_back(p);
}

void Scene::AddTriangles(const std::string &texture,const Triangle &triangle)
{
	m_triangles[texture].push_back(triangle);
}

void Scene::LoadModel(const std::string &filename)
{
}
