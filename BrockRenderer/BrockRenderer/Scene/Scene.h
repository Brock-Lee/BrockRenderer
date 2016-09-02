#pragma once

class Scene{

public:
	void AddPoint(const vec3 &p);
	void AddTriangles(const std::string texture,const Triangle &triangle);
	std::vector<vec3> m_points;
	std::unordered_map<std::string, std::vector<Triangle>> m_triangles;
};

extern Scene* g_scene; 
