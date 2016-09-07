#pragma once
class Texture;
class Material;
class Scene{
struct AABB{
	vec3 minxyz;
	vec3 maxxyz;
	void Update(vec3 p)
	{
		for(int i=0; i<3; i++)
		{
			minxyz[i] = min(minxyz[i], p[i]);
			maxxyz[i] = max(maxxyz[i], p[i]);
		}
	}
	AABB():minxyz(1000000000.0),maxxyz(-1000000000.0){}
};
public:
	vec3 sun_dir;
	Scene():sun_dir(0, 1, 1){
		sun_dir = sun_dir.Normalize();
	}
	AABB m_aabb;
	std::unordered_map<std::string, Texture*> m_textures;
	std::unordered_map<std::string, Material*> m_materials;
	void AddPoint(const vec3 &p);
	void AddTriangles(const std::string &texture,const Triangle &triangle);
	std::vector<vec3> m_points;
	std::unordered_map<std::string, std::vector<Triangle>> m_triangles;
	void LoadModel(const std::string &filename);
	void LoadTexture(const std::string &filename);
};

extern Scene* g_scene; 
