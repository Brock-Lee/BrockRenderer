#include "stdafx.h"
#include <thread>
#include <mutex>
using namespace std;
Context* g_context;
Timer* g_timer;
namespace{
	void VertexProcessAndRasterize(Context *context, const vector<Triangle>& triangles, std::vector<Fragment> &fragments)
	{
		for (auto &t : triangles)
			context->DrawTriangle(t, fragments);
	}
	vec4 FragmentProcess(PSIN &fragment, const Uniform &uniformState)
	{
		static const vec3 ambientLight(0.3);
		static const float shineness = 100.0;

		vec3 color;
		fragment.normal.Normalize();
		if (uniformState.pTexture)
			color = vec3((float*)(&uniformState.pTexture->Sample(fragment.uv)));
		else
			color = uniformState.pMaterial->materialDiffuse;
		vec3 diffuse = color * max(fragment.normal.Dot(g_scene->sun_dir), 0.0);
		vec3 ambient = ambientLight  * color;
		vec3 midRay = ((fragment.viewPosition*-1.0).Normalize() + g_scene->sun_dir).Normalize();
		vec3 specular(0.0);
		float cosTheta = max(midRay.Dot(fragment.normal), 0.0);
		if (cosTheta > 0.90)  // reduce some pow time-cost
			specular = vec3(pow(cosTheta, shineness));
		return vec4(diffuse + ambient + specular, 1.0);
	}
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (msg == WM_CREATE)
		{
		}
		else
		{
			if (msg == WM_DESTROY)
			{
			}
			return WinApp::Instance()->handleMessage(hwnd, msg, wp, lp);
		}

		return DefWindowProc(hwnd, msg, wp, lp);
	}
}
Context::Context() :m_lineMode(false), m_bufferFlag(0), m_backFaceCulling(true), m_preZTest(true)
{
	memset(m_pixels, 0, sizeof(m_pixels));
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;


	const TCHAR *className = TEXT("wc.MainWindow");

	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = className;
	wcex.hIcon = 0;
	wcex.hIconSm = 0;
	wcex.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
	wcex.hInstance = GetModuleHandle(0);
	wcex.lpfnWndProc = (WNDPROC)WndProc;

	RegisterClassEx(&wcex);

	RECT viewportRect;
	viewportRect.left = 0;
	viewportRect.top = 0;
	viewportRect.right = fixedViewportX;
	viewportRect.bottom = fixedViewportY;

	AdjustWindowRect(&viewportRect, WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX, false);

	HDC desktopDC = GetDC(GetDesktopWindow());
	int screenWidth = GetDeviceCaps(desktopDC, HORZRES);
	int screenHeight = GetDeviceCaps(desktopDC, VERTRES);
	ReleaseDC(GetDesktopWindow(), desktopDC);

	m_hWnd = CreateWindowEx(0, className, L"Main window", WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX,
		screenWidth / 2 - fixedViewportX / 2, screenHeight / 2 - fixedViewportY / 2,
		viewportRect.right - viewportRect.left + 1,
		viewportRect.bottom - viewportRect.top + 1,
		0, 0, GetModuleHandle(0), nullptr);

	if (!m_hWnd)
	{
		DWORD e = GetLastError();
		std::ofstream LOG("error.log");
		LOG << e;
		LOG.close();
		UnregisterClass(className, GetModuleHandle(0));
	}


	m_hDC = GetDC(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOW);

}
void Context::SwapBuffer()
{
	m_bufferFlag = 1 - m_bufferFlag;
}

void Context::Flush()
{
	if (!m_hDC)
		return;

	static BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = fixedViewportX;
	bmi.bmiHeader.biHeight = -fixedViewportY;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;

	static const int textHeight = 10;
	SetDIBitsToDevice(m_hDC,
		0, 0,
		fixedViewportX, fixedViewportY + textHeight,
		0, 0,
		0, fixedViewportY - textHeight,
		m_pixels,
		&bmi,
		DIB_RGB_COLORS);

	//Text output
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << g_timer->Elapse();
	std::string timeCost;
	ss >> timeCost;
	timeCost += "ms/Frame";

	ss.clear();
	ss << g_scene->m_triangleCount;
	std::string triangleCntStr;
	ss >> triangleCntStr;
	triangleCntStr += " Triangles";

	const std::string controlStr = "UP:q  Down:e  Left:a  right:d  Forward:w  Backward:s  Rotate: L-Button Drag&Move  Line/Fill: f  ";
	std::string allStr = timeCost + "    " + triangleCntStr + "    " + controlStr;
	WCHAR w_str[200];
	MultiByteToWideChar(CP_ACP, 0, allStr.c_str(), -1, w_str, allStr.size());
	TextOut(m_hDC, 0, 0, w_str, allStr.size());
}

void Context::Draw()
{
	DrawPoints();
	DrawTriangles();
}
void Context::DrawPoints()
{
	for(int i=0; i<g_scene->m_points.size(); i++)
	{
		vec3 &p = g_scene->m_points[i];
		vec4 pProjCoord = vec4(p,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
		for(int i=0; i<3; i++)
			pProjCoord[i] /= pProjCoord.w;
		if(pProjCoord.x >=-1.0 && pProjCoord.x<=1.0 && pProjCoord.y >=-1.0 && pProjCoord.y<=1.0 && pProjCoord.z>=0.0 && pProjCoord.z<=1.0)
			FillPixel(NDC2ScreenX(pProjCoord.x), NDC2ScreenY(pProjCoord.y), vec4(1.0), pProjCoord.z);
	}
}
//Line Clipping
void Context::DrawLine(const vec3 &a, const vec3 &b)
{
	vec4 aProjCoord = vec4(a,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
	vec4 bProjCoord = vec4(b,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
	
	float nearz = g_camera->m_nearZ;
	float farz = g_camera->m_farZ;
	if((aProjCoord.w<nearz && bProjCoord.w<nearz) ||(aProjCoord.w>farz && bProjCoord.w>farz))
		return;
	
	aProjCoord = ClampPoint(aProjCoord, bProjCoord, aProjCoord);
	bProjCoord = ClampPoint(aProjCoord, bProjCoord, bProjCoord);
	vec3 aNDC = vec3((float*)(&aProjCoord))/aProjCoord.w;
	vec3 bNDC = vec3((float*)(&bProjCoord))/bProjCoord.w;

	float x0 = aNDC.x, y0 = aNDC.y, x1 = bNDC.x, y1 = bNDC.y;
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	OutCode outcode0 = ComputeOutCode(x0, y0);
	OutCode outcode1 = ComputeOutCode(x1, y1);
	bool accept = false;
	
	while (true) {
		if (!(outcode0 | outcode1)) { // Bitwise OR is 0. Trivially accept and get out of loop
			accept = true;
			break;
		} else if (outcode0 & outcode1) { // Bitwise AND is not 0. Trivially reject and get out of loop
			break;
		} else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			float x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

			// find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (1 - y0) / (y1 - y0);
				y = 1;
			} else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (-1 - y0) / (y1 - y0);
				y = -1;
			} else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (1 - x0) / (x1 - x0);
				x = 1;
			} else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (-1 - x0) / (x1 - x0);
				x = -1;
			}

			// move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = ComputeOutCode(x0, y0);
			} else {
				x1 = x;
				y1 = y;
				outcode1 = ComputeOutCode(x1, y1);
			}
		}
	}
	if (accept) {
		float z0,z1;
		if(aNDC.x!=bNDC.x)
		{
			z0 = Interpolate(aNDC.z, bNDC.z, (x0-aNDC.x)/(bNDC.x-aNDC.x));
			z1 = Interpolate(aNDC.z, bNDC.z, (x1-aNDC.x)/(bNDC.x-aNDC.x));
		}
		else if(aNDC.y!= bNDC.y)
		{
			z0 = Interpolate(aNDC.z, bNDC.z, (y0-aNDC.y)/(bNDC.y-aNDC.y));
			z1 = Interpolate(aNDC.z, bNDC.z, (y1-aNDC.y)/(bNDC.y-aNDC.y));
		}
		else
		{
			z0 = z1 = aNDC.z;
		}
		FillLine(x0, y0, z0, x1, y1, z1); 
	}

}
//Bresenham line algorithm
void Context::FillLine(float xNDC0, float yNDC0, float zNDC0, float xNDC1, float yNDC1, float zNDC1)
{

	int x0 = NDC2ScreenX(xNDC0);
	int y0 = NDC2ScreenY(yNDC0);
	int x1 = NDC2ScreenX(xNDC1);
	int y1 = NDC2ScreenY(yNDC1);
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	float dzdx = ((x1-x0)!=0)?(zNDC1 - zNDC0)/ (x1-x0)*(dx/float(dx+dy)): 1.0;
	dzdx *= sx;
	float dzdy = ((y1-y0)!=0)?(zNDC1 - zNDC0)/ (y1-y0)*(dy/float(dx+dy)) : 1.0;
	dzdy *= sy;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;){
		FillPixel(x0,y0,vec4(1.0), zNDC0);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; zNDC0+=dzdx;}
		if (e2 < dy) { err += dx; y0 += sy; zNDC0+=dzdy;}
	}

}

void Context::ScanLine(VSOUT& a, VSOUT& b, vector<Fragment>& fragments)
{
	if(a.ndc.x > b.ndc.x)
		swap(a, b);
	if(a.ndc.y < -1 || a.ndc.y> 1 || b.ndc.x<-1 || a.ndc.x>1)
		return;

	int y = std::floor((a.ndc.y+1.0)/2.0 * fixedViewportY);
	int x0 = std::floor((a.ndc.x+1.0)/2.0 * fixedViewportX);
	int x1 = std::floor((b.ndc.x+1.0)/2.0 * fixedViewportX);

	int x= max(int(0), x0);

	for(;x<=x1 && x<fixedViewportX; x++){
		VSOUT temp = PerspectiveInterp(a, b, (x*2.0/fixedViewportX -1.0 -a.ndc.x)/ NoZero(b.ndc.x - a.ndc.x));

		/*
		//pre-z
		if( !DepthTest(x, y, temp.ndc.z) )
			continue;
		//Fragment Shading
		FillPixel(x,y, FragmentShading(PSIN(temp.normaldw * temp.w, temp.uvdw*temp.w, temp.viewPositiondw*temp.w)), temp.ndc.z);*/
		fragments.emplace_back(PSIN(temp.normaldw * temp.w, temp.uvdw*temp.w, temp.viewPositiondw*temp.w), x, y, temp.ndc.z);
	}
}

void Context::Clear()
{
	memset(m_pixels[m_bufferFlag], 0, sizeof(m_pixels)/2);
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;
}

bool Context::DepthTest( int x, int y, float depth )
{
	return depth < m_depth[fixedViewportY-1-y][x];
}

void Context::FillPixel(int x, int y, vec4 &color, float depth)
{
	//bufferMutex.lock();
	if (!DepthTest(x, y, depth))
	{
	//	bufferMutex.unlock();
		return;
	}
	m_depth[fixedViewportY - 1 - y][x] = depth;
	for(int i=0; i<4; i++)
		m_pixels[m_bufferFlag][fixedViewportY-1-y][x][i]  = FloatToByte(color[i]);
	
	//bufferMutex.unlock();
}

vec4 Context::ClampPoint(const vec4 &a, const vec4 & b, const vec4 &p)
{
	if((p.w>=g_camera->m_nearZ && p.w <= g_camera->m_farZ) || a.w == b.w)
		return p;
	else
	{
		float w = Clamp(g_camera->m_nearZ, g_camera->m_farZ, p.w);
		return Interpolate(a, b, (w-a.w) / (b.w-a.w));
	}
}

int Context::NDC2ScreenX(float x)
{
	return max(min(std::floor((x+1.0)/2.0 * fixedViewportX), fixedViewportX-1),0);
}

int Context::NDC2ScreenY(float y)
{
	return max(min(std::floor((y+1.0)/2.0 * fixedViewportY), fixedViewportY-1), 0);
}

void Context::DrawTriangles()
{
	if(m_lineMode)
	{
		auto iter = g_scene->m_triangles.begin();
		for(; iter!=g_scene->m_triangles.end(); iter++)
		{
			for(int t=0; t<iter->second.size(); t++)
				for(int i=0; i<3; i++)
					DrawLine( iter->second.at(t).v[i].position, iter->second.at(t).v[(i+1)%3].position);
		}
	}
	else
	{
		vector<thread> vertexThread(g_scene->m_triangles.size());
		auto iter = g_scene->m_triangles.begin();
		unsigned int threadID = 0;
		m_fragments.resize(g_scene->m_triangles.size());
		for(; iter!=g_scene->m_triangles.end(); iter++)
		{
			m_fragments[threadID] = make_pair(Uniform{ g_scene->m_textures[iter->first], g_scene->m_materials[iter->first] }, vector<Fragment>());
			auto &v = m_fragments[threadID].second;
			vertexThread[threadID] = thread(VertexProcessAndRasterize, this, std::ref(iter->second), std::ref(v));
			threadID++;
		}
		for (auto &t : vertexThread)
			t.join();

		for (auto &fragmentsShading : m_fragments)
		{	
			for (auto &frag : fragmentsShading.second)
			{
				if (m_preZTest && DepthTest(frag.x, frag.y, frag.depth))  // pre-Z
					FillPixel(frag.x, frag.y, FragmentProcess(frag.psin, fragmentsShading.first), frag.depth);
			}
		}

		m_fragments.clear();
	}
}

void Context::DrawTriangle(const Triangle& triangle, std::vector<Fragment>& fragments)
{
	// BackFace Culling
	if(m_backFaceCulling && (g_camera->m_position-triangle.v[0].position).Dot(triangle.face_normal) < 0.0 )
		return;
	vector<VSOUT> verticesPre(3);
	for(int i=0; i<3; i++)
	{
		vec4 projCoord = vec4(triangle.v[i].position,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
		verticesPre[i].ndc = vec3((float*)&projCoord);
		verticesPre[i].w = projCoord.w;
		verticesPre[i].uvdw = triangle.v[i].uv;
		verticesPre[i].normaldw = triangle.v[i].normal;
		verticesPre[i].viewPositiondw = triangle.v[i].position - g_camera->GetPosition();
	}
	sort(verticesPre.begin(), verticesPre.end(),
		[](const VSOUT &a, const VSOUT &b){ return a.w < b.w;}
	);

	// near Z Clipping
	if(verticesPre[2].w < g_camera->m_nearZ)
		return;
	else if(verticesPre[1].w < g_camera->m_nearZ)
	{
		verticesPre[0] = Interp(verticesPre[2], verticesPre[0], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[0].w - verticesPre[2].w ) );
		verticesPre[1] = Interp(verticesPre[2], verticesPre[1], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[1].w - verticesPre[2].w ) );
		DrawClippedTriangle(verticesPre, fragments);
	}
	else if( verticesPre[0].w < g_camera->m_nearZ)
	{
		VSOUT v01 = Interp(verticesPre[1], verticesPre[0], (g_camera->m_nearZ - verticesPre[1].w) / (verticesPre[0].w - verticesPre[1].w ) );
		VSOUT v02 = Interp(verticesPre[2], verticesPre[0], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[0].w - verticesPre[2].w ) );
		verticesPre[0] = v01;
		DrawClippedTriangle(verticesPre, fragments);
		verticesPre[1] = v02;
		DrawClippedTriangle(verticesPre, fragments);
	}
	else
		DrawClippedTriangle(verticesPre, fragments);
}
void Context::DrawClippedTriangle(std::vector<VSOUT> vertices, std::vector<Fragment>& fragments)
{
	for(int i=0; i<3; i++)
	{
		vertices[i].DivideByW();
	}
	sort(vertices.begin(), vertices.end(),
		[](const VSOUT &a, const VSOUT &b){ return a.ndc.y > b.ndc.y;}
	);
	if(vertices[0].ndc.y == vertices[2].ndc.y)  // Ë®Æ½Ïß
		return;
	vector<VSOUT> verticesTop = vertices;
	verticesTop[2] = PerspectiveInterp(vertices[0], vertices[2], (vertices[1].ndc.y-vertices[0].ndc.y)/(vertices[2].ndc.y-vertices[0].ndc.y));
	DrawFlatTriangle(verticesTop, 1, fragments);

	vector<VSOUT> &verticesBottom = verticesTop;
	verticesBottom[0] = vertices[2];
	DrawFlatTriangle(verticesBottom, -1, fragments);
}
void Context::DrawFlatTriangle(const std::vector<VSOUT> &triangle, int dir, std::vector<Fragment>& fragments)
{
	//TODO:: precision details
	if(triangle[1].ndc.y - triangle[0].ndc.y == 0.0)
		return;
	vec2i screenC[3];
	for(int i=0; i<3; i++)
		screenC[i] = vec2i(NDC2ScreenX(triangle[i].ndc.x), NDC2ScreenY(triangle[i].ndc.y));
	int y0 = screenC[0].y;
	float t = ( (y0 + 0.5)*2.0/fixedViewportY-1.0-triangle[0].ndc.y) / NoZero(triangle[1].ndc.y - triangle[0].ndc.y);
	float t2 = ( (screenC[1].y + 0.5)*2.0/fixedViewportY-1.0-triangle[0].ndc.y) / NoZero(triangle[1].ndc.y - triangle[0].ndc.y);
	t2 = min(1.0, t2);
	float dt = abs((2.0/fixedViewportY) / NoZero(triangle[1].ndc.y - triangle[0].ndc.y));//(t2-t)/ NoZero(abs(screenC[1].y-y0));
	while(true)
	{
		if(t>=0.0 && t<=1.0)
		{
			VSOUT a = PerspectiveInterp(triangle[0], triangle[1], t);
			VSOUT b = PerspectiveInterp(triangle[0], triangle[2], t);
			ScanLine(a,b, fragments);
		}
		if(y0 == screenC[1].y)
			return;
		y0 -= dir;
		t += dt;

	}
}


Context::~Context()
{
	if (m_hDC)
		ReleaseDC(m_hWnd, m_hDC);

	if (m_hWnd)
		DestroyWindow(m_hWnd);
}
