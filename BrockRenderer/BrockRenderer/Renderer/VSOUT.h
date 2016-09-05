#pragma once
struct VSOUT{
	// need perspective interpolation
	vec3 normaldw;
	vec2 uvdw;
	float w;
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
	return result;
}

struct PSIN{
	vec3 normal;
	vec2 uv;
};