#include "stdafx.h"

Context* g_context;
Context::Context()
{
	Clear();
}

void Context::Draw()
{
	DrawPoints();
	DrawLines();
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
			FillPixel(vec2(pProjCoord.x, pProjCoord.y), vec4(1.0), pProjCoord.z);
	}
}

void Context::DrawLines()
{
	for(int i=0; i<g_scene->m_lines.size(); i++)
	{
		vec3 &p1 = g_scene->m_lines[i].first;
		vec3 &p2 = g_scene->m_lines[i].second;
		vec4 pProjCoord = vec4(p,1.0) * g_camera->m_viewMatrix * g_camera->m_projMatrix;
		for(int i=0; i<3; i++)
			pProjCoord[i] /= pProjCoord.w;
		if(pProjCoord.x >=-1.0 && pProjCoord.x<=1.0 && pProjCoord.y >=-1.0 && pProjCoord.y<=1.0 && pProjCoord.z>=0.0 && pProjCoord.z<=1.0)
			FillPixel(vec2(pProjCoord.x, pProjCoord.y), vec4(1.0), pProjCoord.z);
	}
}


void Context::Clear()
{
	memset(m_pixels, 0, sizeof(m_pixels));
	for(int i=0; i<fixedViewportY; i++)
		for(int j=0; j<fixedViewportX; j++)
			m_depth[i][j] = 1.0;
}

void Context::FillPixel( vec2 pos, vec4 color, float depth )
{
	int x = Round((pos.x+1.0)/2.0 * fixedViewportX);
	int y = Round( (pos.y+1.0)/2.0 * fixedViewportY);
	for(int i=0; i<4; i++)
		m_pixels[y][x][i]  = FloatToByte(color[i]);
}

void Context::DrawTriangles()
{

}
