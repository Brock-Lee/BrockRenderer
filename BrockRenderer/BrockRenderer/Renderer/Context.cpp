#include "stdafx.h"

using namespace std;
Context* g_context;
Context::Context():m_lineMode(false),m_bufferFlag(0)
{
	memset(m_pixels, 0, sizeof(m_pixels));
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;
	m_vp = new Window::WindowsViewport( fixedViewportX, fixedViewportY);
}

void Context::Render()
{
	Clear();			
	Draw();
	m_vp->flush(&m_pixels[m_bufferFlag][0][0][0]);
	m_bufferFlag = 1-m_bufferFlag;
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

			// Now find the intersection point;
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

			// Now we move outside point to intersection point to clip
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

void Context::ScanLine(VSOUT& a, VSOUT& b )
{
	if(a.ndc.x > b.ndc.x)
		swap(a, b);
	if(a.ndc.y < -1 || a.ndc.y> 1 || b.ndc.x<-1 || a.ndc.x>1)
		return;

	int y = std::floor((a.ndc.y+1.0)/2.0 * fixedViewportY);
	int x0 = std::floor((a.ndc.x+1.0)/2.0 * fixedViewportY);
	int x1 = std::floor((b.ndc.x+1.0)/2.0 * fixedViewportY);
	int x= max(int(0), x0);

	for(;;){
		VSOUT temp = PerspectiveInterp(a, b, (x*2.0/fixedViewportX -1.0 -a.ndc.x)/ NoZero(b.ndc.x - a.ndc.x));

		FillPixel(x,y,vec4(1.0), temp.ndc.z);//FragmentShading(PSIN(temp.normaldw * temp.w, temp.uvdw*temp.w, temp.viewPositiondw*temp.w)), temp.ndc.z);
			//white?vec4(1.0):vec4(0.0), temp.w);
		if (x==x1 || x==fixedViewportX-1) break;
		x += 1;
	}
}

void Context::Clear()
{
	memset(m_pixels[m_bufferFlag], 0, sizeof(m_pixels)/2);
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;
}


void Context::FillPixel(int x, int y, vec4 color, float depth)
{
	if(depth > m_depth[fixedViewportY-1-y][x])
		return;
	for(int i=0; i<4; i++)
		m_pixels[m_bufferFlag][fixedViewportY-1-y][x][i]  = FloatToByte(color[i]);
	m_depth[fixedViewportY-1-y][x] = depth;
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
	return min(std::floor((x+1.0)/2.0 * fixedViewportX), fixedViewportX-1);
}

int Context::NDC2ScreenY(float y)
{
	return min(std::floor((y+1.0)/2.0 * fixedViewportY), fixedViewportY-1);
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
		auto iter = g_scene->m_triangles.begin();
		for(; iter!=g_scene->m_triangles.end(); iter++)
		{
			for(int t=0; t<iter->second.size(); t++)
				DrawTriangle(iter->second.at(t));
		}
		/*
		VertexProcess(Triangle * vertexBuffer, VSOUT* vsout);
		TriangleRasterization(VSOUT* vsout);
		FragmentProcess();*/
	}
}

PSOUT Context::FragmentShading(PSIN fragment)
{
	vec3 diffuse = fragment.normal.Dot(g_scene->sun_dir) * 1.0;
	static vec3 ambient(0.2);
	vec3 midRay = ((fragment.viewPosition*-1.0).Normalize() + g_scene->sun_dir).Normalize();
	vec3 specular = vec3(pow( double(midRay.Dot(fragment.normal)), 400.0)) *0.0;
	return vec4(diffuse + ambient + specular, 1.0);
}

void Context::DrawTriangle( const Triangle& triangle )
{
	/*
	vector<VSOUT> vertices(3);
	for(int i=0; i<3; i++)
	{
		vec4 projCoord = vec4(triangle.v[i].position,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
		vertices[i].ndc = vec3((float*)&projCoord) / projCoord.w;
		vertices[i].w = projCoord.w;
		vertices[i].uvdw = triangle.v[i].uv/ projCoord.w;
		vertices[i].normaldw = triangle.v[i].normal/ projCoord.w;
		vertices[i].viewPositiondw = triangle.v[i].position - g_camera->GetPosition() / projCoord.w;
	}*/
	// BackFace Culling
	if( (g_camera->m_position-triangle.v[0].position).Dot(triangle.v[0].normal) < 0 )
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
	if(verticesPre[2].w < g_camera->m_nearZ)
		return;
	else if(verticesPre[1].w < g_camera->m_nearZ)
	{
		verticesPre[0] = Interp(verticesPre[2], verticesPre[0], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[0].w - verticesPre[2].w ) );
		verticesPre[1] = Interp(verticesPre[2], verticesPre[1], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[1].w - verticesPre[2].w ) );
		DrawClippedTriangle(verticesPre);
	}
	else if( verticesPre[0].w < g_camera->m_nearZ)
	{
		VSOUT v01 = Interp(verticesPre[1], verticesPre[0], (g_camera->m_nearZ - verticesPre[1].w) / (verticesPre[0].w - verticesPre[1].w ) );
		VSOUT v02 = Interp(verticesPre[2], verticesPre[0], (g_camera->m_nearZ - verticesPre[2].w) / (verticesPre[0].w - verticesPre[2].w ) );
		verticesPre[0] = v01;
		DrawClippedTriangle(verticesPre);
		verticesPre[1] = v02;
		DrawClippedTriangle(verticesPre);
	}
	else
		DrawClippedTriangle(verticesPre);
	//TODO:: Backface Clipping
	//TODO:: NearPlane Clipping
}
void Context::DrawClippedTriangle( std::vector<VSOUT> vertices )
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
	DrawFlatTriangle(verticesTop, 1);

	vector<VSOUT> &verticesBottom = verticesTop;
	verticesBottom[0] = vertices[2];
	DrawFlatTriangle(verticesBottom, -1);
}
void Context::DrawFlatTriangle( const std::vector<VSOUT> &triangle, int dir)
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
	float dt = (t2-t)/ NoZero(abs(screenC[1].y-y0));
	while(true)
	{
		if(t>=0.0 && t<=1.0)
		{
			VSOUT a = PerspectiveInterp(triangle[0], triangle[1], t);
			VSOUT b = PerspectiveInterp(triangle[0], triangle[2], t);
			ScanLine(a,b);
		}
		if(y0 == screenC[1].y)
			return;
		y0 -= dir;
		t += dt;

	}
}


