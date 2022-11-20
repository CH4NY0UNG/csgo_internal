#include "inc.h"

int GetMaxPlayer()
{
    return *(int*)(*(intptr_t*)(enginedll + offsets::dwClientState) + offsets::dwClientState_MaxPlayer);
}

vec3* GetMyAngle()
{
    return (vec3*)(*(intptr_t*)(enginedll + offsets::dwClientState) + offsets::dwClientState_ViewAngles);
}

DWORD GetViewMatrix()
{
    return (DWORD)(clientdll + offsets::dwViewMatrix);
}

void GlowESP(Ent* ent, float r, float g, float b, float a)
{
    glowObj = *(uintptr_t*)(clientdll + offsets::dwGlowObjectManager);
    if (!glowObj) return;

    int idx = ent->GlowIndex;
    *(float*)(glowObj + idx * 0x38 + 0x8) = r;
    *(float*)(glowObj + idx * 0x38 + 0xC) = g;
    *(float*)(glowObj + idx * 0x38 + 0x10) = b;
    *(float*)(glowObj + idx * 0x38 + 0x14) = a;

    *(int*)(glowObj + idx * 0x38 + 0x28) = 1;
    *(int*)(glowObj + idx * 0x38 + 0x29) = 0;
}