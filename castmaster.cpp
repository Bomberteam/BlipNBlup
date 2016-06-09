#include "castmaster.h"


CastMaster::CastMaster(Context* context) : Object(context)
{

}

bool CastMaster::PhysicsRayCast(PODVector<PhysicsRaycastResult> &hitResults, const Ray ray,
                                   const float distance, const unsigned collisionMask)
{
    if (distance > 1.0e-9)
        MC->GetScene()->GetComponent<PhysicsWorld>()->Raycast(hitResults, ray, distance, collisionMask);

    return (hitResults.Size() > 0);
}
