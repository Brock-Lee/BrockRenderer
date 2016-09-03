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

template <typename T>
T Interpolate(T a, T b, float factor)
{
	return a*(1-factor) + b*factor;
}

typedef int OutCode;

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (xmin, ymin), and (xmax, ymax)

inline OutCode ComputeOutCode(float x, float y, float xmin=-1, float xmax=1, float ymin=-1, float ymax=1)
{
	OutCode code;

	code = INSIDE;          // initialised as being inside of [[clip window]]

	if (x < xmin)           // to the left of clip window
		code |= LEFT;
	else if (x > xmax)      // to the right of clip window
		code |= RIGHT;
	if (y < ymin)           // below the clip window
		code |= BOTTOM;
	else if (y > ymax)      // above the clip window
		code |= TOP;

	return code;
}
