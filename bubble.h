#ifndef BUBBLE_H
#define BUBBLE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Bubble : public LogicComponent
{
    URHO3D_OBJECT(Bubble, LogicComponent);
public:
    Bubble(Context* context);
    static void RegisterObject(Context* context);

    virtual void OnNodeSet(Node* node);

    void Update(float timeStep);
private:
    SharedPtr<StaticModel> model_;
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<CollisionShape> collider_;
};

#endif // BUBBLE_H
