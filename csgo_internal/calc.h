#pragma once

inline RECT clientSize;
const static float PI = 3.14159265359;

template<typename T>
bool InRange(T value, int min, int max)
{
	if (value >= min && value < max) return true;
	return false;
}

int GetRandomNum(int min, int max);
int GetSmoothX(float dist, float size);
int GetSmoothY(float dist, float size);
RECT GetClientSize(LPCSTR className, LPCSTR titleName);
float Magnitude(vec3 vec);
float Distance(vec3 src, vec3 dst);
vec3 CalcAngle(vec3 src, vec3 dst);
vec3 CalcReverseAngle(vec3 src, vec3 dst);
bool W2S(vec3 pos, vec2& screen);