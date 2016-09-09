#pragma once
class Context
{
public:
	Context();
    static const unsigned int fixedViewportX= 800, fixedViewportY=800;
	unsigned char m_pixels[2][fixedViewportY][fixedViewportX][4];
	bool m_lineMode;
	bool m_backFaceCulling;
	int m_bufferFlag;
	void Render();
	void DrawTriangle(const Triangle& triangle, std::vector<Fragment>& fragments);
	void FillPixel(int x, int y, vec4 &color, float depth);
	bool DepthTest(int x, int y, float depth);
private:

	float m_depth[fixedViewportY][fixedViewportX];
	Window::WindowsViewport* m_vp;
	void Draw();
	void Clear();
	void DrawPoints();
	void DrawLine(const vec3 &a, const vec3 &b);
	std::vector<std::pair<Uniform, std::vector<Fragment>>> m_fragments;
	void DrawClippedTriangle(std::vector<VSOUT> triangle, std::vector<Fragment>& fragments);
	void DrawFlatTriangle(const std::vector<VSOUT> &triangle, int dir, std::vector<Fragment>& fragments);
	void DrawTriangles();

	//线框模式
	void FillLine(float x0, float y0, float z0, float x1, float y1, float z1);
	//扫描线
	void ScanLine(VSOUT& a, VSOUT& b, std::vector<Fragment>& fragments);
	vec4 ClampPoint(const vec4 &a, const vec4 & b, const vec4 &p);
	int NDC2ScreenX(float x);
	int NDC2ScreenY(float y);
};




extern Context* g_context;