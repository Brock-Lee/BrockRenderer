#include "stdafx.h"

Context* g_context;
Context::Context():m_lineMode(true),m_bufferFlag(0)
{
	m_vp = new Window::WindowsViewport( fixedViewportX, fixedViewportY);
	memset(m_pixels, 0, sizeof(m_pixels));
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;
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
					DrawLine( iter->second.at(t).p[i], iter->second.at(t).p[(i+1)%3]);
		}
	}
	else
	{

	}
}
