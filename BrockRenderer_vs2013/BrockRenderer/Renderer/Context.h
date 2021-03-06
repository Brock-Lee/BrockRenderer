#pragma once
class Context
{
public:
	Context();
	~Context();
    static const unsigned int fixedViewportX= 1024, fixedViewportY=768;
	unsigned char m_pixels[2][fixedViewportY][fixedViewportX][4];
	bool m_lineMode;
	bool m_backFaceCulling;
	bool m_preZTest;
	void DrawTriangle(const Triangle& triangle, std::vector<Fragment>& fragments);
	void FillPixel(int x, int y, vec4 &color, float depth);
	bool DepthTest(int x, int y, float depth);
	void Draw();
	void Clear();
	void Flush();
	void SwapBuffer();
	
private:
	float m_depth[fixedViewportY][fixedViewportX];
	int m_bufferFlag;
	HWND m_hWnd;
	HDC  m_hDC;

	void DrawPoints();
	void DrawLine(const vec3 &a, const vec3 &b);
	std::vector<std::pair<Uniform, std::vector<Fragment>>> m_fragments;
	void DrawClippedTriangle(std::vector<VSOUT> triangle, std::vector<Fragment>& fragments);
	void DrawFlatTriangle(const std::vector<VSOUT> &triangle, int dir, std::vector<Fragment>& fragments);
	void DrawTriangles();

	//�߿�ģʽ
	void FillLine(float x0, float y0, float z0, float x1, float y1, float z1);
	//ɨ����
	void ScanLine(VSOUT& a, VSOUT& b, std::vector<Fragment>& fragments);
	vec4 ClampPoint(const vec4 &a, const vec4 & b, const vec4 &p);
	int NDC2ScreenX(float x);
	int NDC2ScreenY(float y);
};




extern Context* g_context;