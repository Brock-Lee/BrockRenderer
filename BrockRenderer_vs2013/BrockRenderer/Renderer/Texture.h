#pragma once
class Texture
{
public:
	struct Layer{
		int width,height;
		unsigned char* pixels;
		Layer( unsigned int w, unsigned int h, unsigned char* p):width(w), height(h), pixels(p){};
		vec4 LinearSample(int w, int h, float tx, float ty);
	};
	std::vector<Layer> layers;
	Texture(std::string filename);
	vec4 Sample(vec2 uv);
};

