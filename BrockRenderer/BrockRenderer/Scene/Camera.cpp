#include "stdafx.h"

Camera::Camera()
{
	LookAt(vec3(0,0,0), vec3(0,0,1), vec3(0,1,0));
	Perspective(PAI*0.9, 1, 0.1, 10);
}

void Camera::LookAt( vec3 pos, vec3 dir, vec3 up )
{
	m_right= up.Cross(dir);
	m_up = dir.Cross(m_right);
	m_dir = dir;
	m_position = pos;

	m_viewMatrix = mat4(m_right,m_up,m_dir,m_position);
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
