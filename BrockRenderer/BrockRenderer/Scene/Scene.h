#pragma once

class Scene{
	struct Triangle{
		vec3 p[3];
		vec3 normal;
		vec2 uv;
	};
public:
	void AddPoint(const vec3 &p);
	void AddLine(const std::pair<vec3,vec3> &line);
	std::vector<vec3> m_points;
	std::unordered_map<std::string, std::vector<Triangle>> m_triangles;
};

extern Scene* g_scene; 
