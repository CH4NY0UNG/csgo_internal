#include "inc.h"

bool IsVisible(Ent* lp, Ent* ent, int boneID)
{
    CGameTrace trace;
    Ray_t ray;
    CTraceFilter tracefilter;
    tracefilter.pSkip = (void*)lp;

    vec3 mPos = lp->Origin + lp->ViewOffset;
    vec3 tPos = ent->GetBone(boneID);
    ray.Init(mPos, tPos);

    EngineTrace->TraceRay(ray, MASK_VISIBLE | CONTENTS_HITBOX, &tracefilter, &trace);
    if (trace.hitgroup != 0 || trace.IsVisible()) return true;

    return false;
}
