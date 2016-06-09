#ifndef CASTMASTER_H
#define CASTMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class CastMaster : public Object
{
    URHO3D_OBJECT(CastMaster, Object);
public:
    CastMaster(Context* context);
    bool PhysicsRayCast(PODVector<PhysicsRaycastResult>& hitResults, const Ray ray, const float distance, const unsigned collisionMask);
};

#endif // CASTMASTER_H
