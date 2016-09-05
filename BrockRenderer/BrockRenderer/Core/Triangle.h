#pragma once
struct Vertex{
	vec3 position;
	vec3 normal;
	vec2 uv;
};
struct Triangle{
	Vertex v[3];
};