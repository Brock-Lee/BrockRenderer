#pragma once

class Camera{

public:
	Camera();
	mat4 m_viewMatrix;
	mat4 m_projMatrix;
	void LookAt(vec3 pos, vec3 dir, vec3 up);
	void Perspective(float right, float left, float top, float down, float near, float far);
	void Perspective(float fov, float aspect, float nearz, float farz);
	vec3 m_position;
	vec3 m_up;
	vec3 m_dir;
	vec3 m_right;
	//float m_yaw, m_pitch, m_roll;
	//! Field of view. Default is 90 grad.
	float m_fov, m_aspect;
	//! Near Z-clipping plane position.
	float m_nearZ;
	//! Far Z-clipping plane position.
	float m_farZ;
};
extern Camera* g_camera; 
