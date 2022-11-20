#pragma once

struct cplane_t
{
    vec3 normal;
    float dist;
    uint8_t type;   // for fast side tests
    uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
    uint8_t pad[2];
};

struct surface_t
{
    char* name; //0x0000
    int16_t surface_prop; //0x0004
    uint16_t flags; //0x0006
};

struct Ray_t
{
    VectorAligned  m_Start;  // starting point, centered within the extents
    VectorAligned  m_Delta;  // direction + length of the ray
    VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
    VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
    const void* m_pWorldAxisTransform;
    bool m_IsRay;  // are the extents zero?
    bool m_IsSwept;     // is delta != 0?

    Ray_t() : m_pWorldAxisTransform(NULL) {}

    void Init(vec3 const& start, vec3 const& end)
    {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents.Init();

        m_pWorldAxisTransform = NULL;
        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.Init();
        m_Start = start;
    }

    void Init(vec3 const& start, vec3 const& end, vec3 const& mins, vec3 const& maxs)
    {
        m_Delta = end - start;

        m_pWorldAxisTransform = NULL;
        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents = maxs - mins;
        m_Extents *= 0.5f;
        m_IsRay = (m_Extents.LengthSqr() < 1e-6);

        // Offset m_Start to be in the center of the box...
        m_StartOffset = maxs + mins;
        m_StartOffset *= 0.5f;
        m_Start = start + m_StartOffset;
        m_StartOffset *= -1.0f;
    }

    vec3 InvDelta() const
    {
        vec3 vecInvDelta;
        for (int iAxis = 0; iAxis < 3; ++iAxis) {
            if (m_Delta[iAxis] != 0.0f) {
                vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
            }
            else {
                vecInvDelta[iAxis] = FLT_MAX;
            }
        }
        return vecInvDelta;
    }
};

enum class TraceType
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,
    TRACE_ENTITIES_ONLY,
    TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
    virtual bool ShouldHitEntity(void* pEntity, int contentsMask) = 0;
    virtual TraceType GetTraceType() const = 0;
};


// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
    bool ShouldHitEntity(void* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == pSkip);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
    void* pSkip;
};

#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define   ALL_VISIBLE_CONTENTS          (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800
#define   CONTENTS_TEAM2                0x1000
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000

#define   CONTENTS_MONSTER              0x2000000
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)  // everything that is normally solid
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)  // bullets see these as solid
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

class CBaseTrace
{
public:
    bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
    bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
    bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
    bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
    bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

    // these members are aligned!!
    vec3         startpos;            // start position
    vec3         endpos;              // final position
    cplane_t       plane;               // surface normal at impact

    float          fraction;            // 시간 완료, 1.0 = 아무 것도 치지 않음

    int            contents;            // contents on other side of surface hit
    unsigned short dispFlags;           // displacement flags for marking surfaces with data

    bool           allsolid;            // true이면 평면이 유효하지 않습니다.
    bool           startsolid;          // true인 경우 초기 지점은 솔리드 영역에 있었습니다.

    CBaseTrace() {}

};

class CGameTrace : public CBaseTrace
{
public:
    bool DidHitWorld() const;
    bool DidHitNonWorldEntity() const;
    int GetEntityIndex() const;
    bool DidHit() const;
    bool IsVisible() const;

public:

    float               fractionleftsolid;  // time we left a solid, only valid if we started in solid
    surface_t          surface;            // surface hit (impact surface)
    int                 hitgroup;           // 0 == generic, non-zero is specific body part
    short               physicsbone;        // physics bone hit by trace in studio
    unsigned short      worldSurfaceIndex;  // Index of the msurface2_t, if applicable
    //IClientEntity* hit_entity;
    void* hit_entity;
    int                 hitbox;                       // box hit by trace in studio

    CGameTrace() {}

private:
    // No copy constructors allowed
    CGameTrace(const CGameTrace& other) :
        fractionleftsolid(other.fractionleftsolid),
        surface(other.surface),
        hitgroup(other.hitgroup),
        physicsbone(other.physicsbone),
        worldSurfaceIndex(other.worldSurfaceIndex),
        hit_entity(other.hit_entity),
        hitbox(other.hitbox)
    {
        startpos = other.startpos;
        endpos = other.endpos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        dispFlags = other.dispFlags;
        allsolid = other.allsolid;
        startsolid = other.startsolid;
    }

    CGameTrace& operator=(const CGameTrace& other)
    {
        startpos = other.startpos;
        endpos = other.endpos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        dispFlags = other.dispFlags;
        allsolid = other.allsolid;
        startsolid = other.startsolid;
        fractionleftsolid = other.fractionleftsolid;
        surface = other.surface;
        hitgroup = other.hitgroup;
        physicsbone = other.physicsbone;
        worldSurfaceIndex = other.worldSurfaceIndex;
        hit_entity = other.hit_entity;
        hitbox = other.hitbox;
        return *this;
    }
};

inline bool CGameTrace::DidHit() const
{
    return fraction < 1 || allsolid || startsolid;
}

inline bool CGameTrace::IsVisible() const
{
    return fraction > 0.97f;
}

class IEngineTrace
{
public:
    virtual void  fn0() = 0;
    virtual void  fn1() = 0;
    virtual void  fn2() = 0;
    virtual void  fn3() = 0;
    virtual void  fn4() = 0;
    virtual void  TraceRay(Ray_t& ray, unsigned int fMask, CTraceFilter* pTraceFilter, CGameTrace* pTrace) = 0;
};

typedef void(__fastcall* tTraceRay)(void* thisptr, void* not_edx, Ray_t& ray, unsigned int fMask, CTraceFilter* pTraceFilter, CGameTrace* pTrace);

inline IEngineTrace* EngineTrace;
bool IsVisible(Ent* lp, Ent* ent, int boneID = 8);