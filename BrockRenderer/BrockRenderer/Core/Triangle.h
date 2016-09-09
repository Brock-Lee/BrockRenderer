#pragma once
struct Vertex{
	vec3 position;
	vec3 normal;
	vec2 uv;
};
struct Triangle{
	Vertex v[3];
	vec3 face_normal;
	void GenFaceNormal()
	{
		face_normal = (v[1].position-v[0].position).Cross(v[2].position-v[0].position).Normalize();
	}
};