#pragma once
class Context
{
public:
	Context();
    static const unsigned int fixedViewportX= 800, fixedViewportY=800;
	unsigned char m_pixels[2][fixedViewportY][fixedViewportX][4];
	float m_depth[fixedViewportY][fixedViewportX];
	bool m_lineMode;
	int m_bufferFlag;
	Window::WindowsViewport* m_vp;
	void Render();
private:
	void Draw();
	void Clear();
	void DrawPoints();
	void DrawLine(const vec3 &a, const vec3 &b);
	void DrawTriangle(const Triangle& triangle);
	void DrawTopTriangle(const std::vector<VSOUT> &triangle);
	void DrawBottomTriangle(const std::vector<VSOUT> &triangle);
	void DrawTriangles();
	void FillPixel(int x, int y, vec4 color, float depth);
	void FillLine(float x0, float y0, float z0, float x1, float y1, float z1);
	void FillLine(const VSOUT& a, const VSOUT& b);
	vec4 ClampPoint(const vec4 &a, const vec4 & b, const vec4 &p);
	int NDC2ScreenX(float x);
	int NDC2ScreenY(float y);
};




extern Context* g_context;