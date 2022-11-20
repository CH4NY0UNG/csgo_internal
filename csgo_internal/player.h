#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class Ent
{
public:
    union
    {
        DEFINE_MEMBER_N(int, Team, offsets::m_iTeamNum);
        DEFINE_MEMBER_N(int, Health, offsets::m_iHealth);
        DEFINE_MEMBER_N(vec3, ViewOffset, offsets::m_vecViewOffset);
        DEFINE_MEMBER_N(vec3, Velocity, offsets::m_vecVelocity);
        DEFINE_MEMBER_N(vec3, Origin, offsets::m_vecOrigin);
        DEFINE_MEMBER_N(int, Alive, offsets::m_lifeState);
        DEFINE_MEMBER_N(int, Spotted, offsets::m_bSpotted);
        DEFINE_MEMBER_N(int, SpottedByMask, offsets::m_bSpottedByMask);
        DEFINE_MEMBER_N(DWORD, BoneMatrix, offsets::m_dwBoneMatrix);
        DEFINE_MEMBER_N(vec3, PunchAngle, offsets::m_aimPunchAngle);
        DEFINE_MEMBER_N(int, FOV, offsets::m_iFOVStart);
        DEFINE_MEMBER_N(int, IsLocalPlayer, offsets::m_bIsLocalPlayer);
        DEFINE_MEMBER_N(int, IsScoped, offsets::m_bIsScoped);
        DEFINE_MEMBER_N(bool, IsDefusing, offsets::m_bIsDefusing);
        DEFINE_MEMBER_N(int, IsShotFire, offsets::m_iShotsFired);
        DEFINE_MEMBER_N(int, GlowIndex, offsets::m_iGlowIndex);
    };

    int GetDormant()
    {
        return *(int*)((uintptr_t)this + offsets::m_bDormant);
    }

    bool IsValidPlayer()
    {
        if (this && this->GetDormant() == 0 && this->Health > 0)
            return true;

        return false;
    }

    vec3 GetBone(int boneID = 8)
    {
        uintptr_t matrix = this->BoneMatrix;
        vec3 bonePos;
        bonePos.x = *(float*)(matrix + 0x30 * boneID + 0x0C);
        bonePos.y = *(float*)(matrix + 0x30 * boneID + 0x1C);
        bonePos.z = *(float*)(matrix + 0x30 * boneID + 0x2C);
        return bonePos;
    }
};

class EntBase
{
public:
    class Ent* ent; //0x0000
    char pad_0004[12]; //0x0004
}; //Size: 0x0010

class EntList
{
public:
    EntBase entBaseList[64]; //0x0000
}; //Size: 0x0200

inline uintptr_t glowObj = 0;
inline vector<int> boneList = { 8, 7, 0, 11, 12, 39, 40 };

int GetMaxPlayer();
vec3* GetMyAngle();
DWORD GetViewMatrix();
void GlowESP(Ent* ent, float r, float g, float b, float a);