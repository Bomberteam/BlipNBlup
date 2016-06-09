#ifndef WALKER_H
#define WALKER_H

#include <Urho3D/Urho3D.h>
#include "controllable.h"

class Walker : public Controllable
{
    URHO3D_OBJECT(Walker, Controllable);
public:
    Walker(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

protected:
    StaticModel* model_;
    RigidBody* rigidBody_;
    CollisionShape* collider_;
    AnimationController* animCtrl_;

    float maxRunSpeed_;
    float runThrust_;

    float jumpForce_;
    float jumpInterval_;

    bool onGround_;
    bool doubleJumper_;
    bool doubleJumped_;
    bool jumpReleased_;
    float sinceJump_;

    void Jump();
    void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
private:
    void AlignWithFloor(float timeStep);
    void AlignWithMovement(float timeStep);
    void ClampPitch(Quaternion& rot);
    void CheckOnGround(MemoryBuffer& contacts);
};

#endif // WALKER_H
