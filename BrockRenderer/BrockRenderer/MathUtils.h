#pragma once
#define PAI 3.1415926
inline float Trunc(float x)
{
	return x - std::floor(x);
}

inline int Round(float x)
{
	return (Trunc(x)>0.5)?std::ceil(x):std::floor(x);
};


inline float Clamp(float a, float b, float x)
{
	return max(a, min(b, x));
}

inline unsigned char FloatToByte(float x)
{
	return Clamp(0, 255, x * 255);
}

inline float DegToRad(float deg)
{
	return deg/180.0 * PAI;
}