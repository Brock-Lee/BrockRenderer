#include "stdafx.h"
#include "ThirdParty/FreeImagePlus.h"
namespace{
	int PositiveMod(int j, int m)
	{
		int i=j%m;
		if(i<0) i += m; 
		return i;
	}
}
vec4 Texture::Sample( vec2 uv )
{
	vec2i uvi( floor(uv.x * layers[0].width), floor(uv.y * layers[0].height) );
	float tx = uv.x * layers[0].width - uvi.x;
	float ty = uv.y * layers[0].height - uvi.y;

	uvi.x = PositiveMod(uvi.x, layers[0].width);
	uvi.y = PositiveMod(uvi.y, layers[0].height);
	return layers[0].LinearSample(uvi.x, uvi.y, tx, ty);
}

Texture::Texture( std::string filename )
{
	fipImage image;
	image.load(filename.data());
	image.convertTo32Bits();
	unsigned int bytes = image.getHeight() * image.getWidth() * 4;
	unsigned char* pixels = new unsigned char[bytes];
	memcpy(pixels, image.accessPixels(), bytes);
	layers.push_back(Layer(image.getWidth() , image.getHeight(), pixels));
}

vec4 Texture::Layer::LinearSample(int w, int h, float tx, float ty)
{
	vec4uc v0(pixels[h * width *4 + w * 4]);
	vec4uc v1(pixels[h * width *4 + (w+1) * 4]);
	vec4uc v2(pixels[(h+1) * width *4 + w * 4]);
	vec4uc v12(pixels[(h+1) * width *4 + (w+1) * 4]);
	vec4 v0f, v1f, v2f, v12f;
	v0f = v0;
	v1f = v1;
	v2f = v2;
	v12f = v12;
	vec4 result = v0f * (1-tx)* (1-ty) + v1f * tx * (1-ty) + v2f * (1-tx) * ty + v12f * tx * ty;
	return result/256.0;
}