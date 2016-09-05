#pragma once
struct VSOUT{
	// need perspective interpolation
	vec3 normaldw;
	vec2 uvdw;
	float w;
	vec3 viewPositiondw;
	// no need
	vec3 ndc;
};
inline VSOUT PerspectiveInterp(VSOUT a, VSOUT b, float t)
{
	VSOUT result;
	result.w = PerspectiveInterp(NoZero(a.w), NoZero(b.w), t);
	result.ndc = Interpolate(a.ndc, b.ndc, t);
	result.normaldw = Interpolate(a.normaldw, b.normaldw, t);
	result.uvdw= Interpolate(a.uvdw, b.uvdw, t);
	result.viewPositiondw = Interpolate(a.viewPositiondw, b.viewPositiondw, t);
	return result;
}

struct PSIN{
	vec3 normal;
	vec2 uv;
	vec3 viewPosition;
	PSIN(vec3 n, vec2 u, vec3 v):normal(n), uv(u), viewPosition(v){}
};

typedef vec4 PSOUT;