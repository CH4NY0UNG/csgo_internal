#include "inc.h"

int GetRandomNum(int min, int max)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> r(min, max);
    return r(gen);
}

int GetSmoothX(float dist, float size)
{
    if (dist >= size * 0.5) return 1000;
    if (dist >= size * 0.3) return 8;
    if (dist >= size * 0.2) return 6;
    if (dist >= size * 0.1) return 4;
    if (dist >= size * 0.08) return 3;
    if (dist >= size * 0.07) return 2;
    if (dist >= size * 0.05) return 1;
    if (dist < size * 0.05) return 1000;
    return 1000;
}

int GetSmoothY(float dist, float size)
{
    if (dist >= size * 0.4) return 1000;
    if (dist >= size * 0.3) return 15;
    if (dist >= size * 0.2) return 13;
    if (dist >= size * 0.1) return 9;
    if (dist >= size * 0.07) return 7;
    if (dist >= size * 0.05) return 5;
    if (dist >= size * 0.04) return 3;
    if (dist < size * 0.04) return 1000;
    return 1000;
}

RECT GetClientSize(LPCSTR className, LPCSTR titleName)
{
    RECT rect;
    GetClientRect(FindWindow(className, titleName), &rect);
    return rect;
}

float Magnitude(vec3 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float Distance(vec3 src, vec3 dst)
{
    vec3 delta = dst - src;
    return Magnitude(delta);
}

vec3 CalcAngle(vec3 src, vec3 dst)
{
    vec3 angle;
    angle.x = -atan2f(dst.x - src.x, dst.y - src.y) / PI * 180.0f + 180.0f;
    angle.y = asinf((dst.z - src.z) / Distance(src, dst)) * 180.0f / PI;
    angle.z = 0.0f;

    return angle;
}

vec3 CalcReverseAngle(vec3 src, vec3 dst)
{
    vec3 angles;
    angles.x = -asin((dst.z - src.z) / Distance(src, dst)) * (180 / PI);
    angles.y = atan2(dst.y - src.y, dst.x - src.x) * (180 / PI);
    angles.z = 0.0f;
    return angles;
}

bool W2S(vec3 pos, vec2& screen)
{
    float matrix[16];
    LI_FN(memcpy).cached()(&matrix, (PBYTE*)GetViewMatrix(), sizeof(matrix));

    vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f)
        return false;

    vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    clientSize = GetClientSize(XOR("Valve001"), 0);
    screen.x = (clientSize.right / 2 * NDC.x) + (NDC.x + clientSize.right / 2);
    screen.y = -(clientSize.bottom / 2 * NDC.y) + (NDC.y + clientSize.bottom / 2);
    return true;
}