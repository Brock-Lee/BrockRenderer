#include "stdafx.h"

Camera::Camera()
{
	LookAt(vec3(0,0,0), vec3(0,0,1), vec3(0,1,0));
	Perspective(PAI*0.8, 1, 0.1, 100);
}

void Camera::LookAt( vec3 pos, vec3 dir, vec3 up )
{
	vec3 right= up.Cross(dir);
	vec3 upReal = dir.Cross(right);
	m_position = pos;
	m_viewMatrix = mat4(right, upReal, dir, pos);
}

void Camera::Perspective( float right, float left, float top, float down, float near, float far )
{

}

void Camera::Perspective( float fov, float aspect, float nearz, float farz )
{
	m_fov = fov;
	m_aspect = aspect;
	m_nearZ = nearz;
	m_farZ = farz;
	
	m_projMatrix.Zero();
	m_projMatrix[0][0] = 1 / (tan(fov * 0.5f) * aspect) ;
	m_projMatrix[1][1] = 1 / tan(fov * 0.5f) ;
	m_projMatrix[2][2] = farz / (farz - nearz) ;
	m_projMatrix[2][3] = 1.0f; 
	m_projMatrix[3][2] = (nearz * farz) / (nearz - farz);
}

void Camera::Move(vec3 v)
{
	mat4 t;
	t[3][0] = -v.x;
	t[3][1] = -v.y;
	t[3][2] = -v.z;
	m_viewMatrix = m_viewMatrix * t;
	m_position += v;
}

vec3 Camera::GetZ()
{
	return vec3(m_viewMatrix[0][2], m_viewMatrix[1][2],m_viewMatrix[2][2]);
}

vec3 Camera::GetPosition()
{
	return m_position;
}
