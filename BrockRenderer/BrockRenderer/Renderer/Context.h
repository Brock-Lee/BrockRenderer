#pragma once
class Context
{
public:
	Context();
    static const unsigned int fixedViewportX= 800, fixedViewportY=800;
	unsigned char m_pixels[fixedViewportY][fixedViewportX][4];
	float m_depth[fixedViewportY][fixedViewportX];
	void Draw();
	void Clear();
private:
	void DrawPoints();
	void DrawLines();
	void DrawTriangles();
	void FillPixel(vec2 pos, vec4 color, float depth);
};




extern Context* g_context;