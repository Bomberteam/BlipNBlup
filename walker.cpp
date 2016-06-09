#include "walker.h"
#include "castmaster.h"

Walker::Walker(Context* context) : Controllable(context),
    onGround_{false},
    doubleJumper_{false},
    doubleJumped_{false},
    sinceJump_{0.0f}
{
    SetUpdateEventMask(USE_UPDATE);

}

void Walker::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateComponent<StaticModel>();
    rigidBody_ = node_->CreateComponent<RigidBody>();
    collider_ = node_->CreateComponent<CollisionShape>();

    model_->SetCastShadows(true);

    rigidBody_->SetLinearDamping(0.5f);
    rigidBody_->SetRestitution(0.23f);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetRollingFriction(0.5f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Walker, HandleNodeCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Walker, HandleNodeCollision));
}

void Walker::Update(float timeStep)
{
    sinceJump_ += timeStep;

    bool run{actions_[0] * onGround_};
    float walkMultiplier{0.6f + 0.4f * run};
    float maxWalkSpeed{maxRunSpeed_ * walkMultiplier};

    PODVector<RigidBody*> bodies{};
    rigidBody_->GetCollidingBodies(bodies);
    if (!bodies.Size())
        onGround_ = false;

    //Apply movement
    if (move_.Length() > 0.0f){
        Quaternion rot{node_->GetRotation()};
        Quaternion targetRot{};
        targetRot.FromLookRotation(move_);
        rot = rot.Slerp(targetRot, Clamp(timeStep * 23.0f, 0.0f, 1.0f));
        node_->SetRotation(rot);

        rigidBody_->SetFriction(0.5f * onGround_);
        Vector3 force{runThrust_ * walkMultiplier * move_ * timeStep};

        if (rigidBody_->GetLinearVelocity().Length() < maxWalkSpeed
         || (rigidBody_->GetLinearVelocity().Normalized() + force.Normalized()).Length() < 1.0f)
        {
            rigidBody_->ApplyForce(force);
        }


    } else rigidBody_->SetFriction(2.0f * onGround_);

    if (onGround_)
        AlignWithFloor(timeStep);
    else {
        AlignWithMovement(timeStep);
    }
}
void Walker::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;
    MemoryBuffer contacts(eventData[NodeCollisionStart::P_CONTACTS].GetBuffer());

    MemoryBuffer contactsCopy{contacts};
    CheckOnGround(contactsCopy);
}
void Walker::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    MemoryBuffer contacts(eventData[NodeCollision::P_CONTACTS].GetBuffer());

    MemoryBuffer contactsCopy{contacts};
    CheckOnGround(contactsCopy);
}
void Walker::CheckOnGround(MemoryBuffer& contacts)
{
    while (!contacts.IsEof())
    {
        Vector3 contactPosition{contacts.ReadVector3()};
        Vector3 contactNormal{contacts.ReadVector3()};
        /*float contactDistance = */contacts.ReadFloat();
        /*float contactImpulse = */contacts.ReadFloat();

        if ((node_->GetPosition().y_ - contactPosition.y_) > collider_->GetSize().x_/2.0f && sinceJump_ > jumpInterval_){

            onGround_ = true;
            doubleJumped_ = false;
            return;
        }
    }
}

void Walker::AlignWithFloor(float timeStep)
{
    PODVector<PhysicsRaycastResult> hitResults{};
    Ray footRay{};
    footRay.origin_ = node_->GetPosition() - node_->GetUp() * collider_->GetSize().y_ * 0.5f;
    footRay.direction_ = Vector3::DOWN;
    if (GetSubsystem<CastMaster>()->PhysicsRayCast(hitResults, footRay, 0.23f, 1)
     && (hitResults[0].normal_.Angle(Vector3::UP)) < 42.0f){

        Quaternion targetRot{};
        Quaternion rot{node_->GetRotation()};
        targetRot.FromLookRotation(Quaternion(90.0f, node_->GetRight()) * hitResults[0].normal_, hitResults[0].normal_);
        ClampPitch(targetRot);
        float angleDelta{rot.EulerAngles().Angle(targetRot.EulerAngles())};
        if (angleDelta > 1.0f)
            ///Should rotate around hit point
            node_->SetRotation(rot.Slerp(targetRot, Clamp(timeStep * (3.0f + 2.0f * angleDelta), 0.0f, 1.0f)));
    }
}
void Walker::AlignWithMovement(float timeStep)
{
    Quaternion targetRot{};
    Quaternion rot{node_->GetRotation()};
    targetRot.FromLookRotation(rigidBody_->GetLinearVelocity());
    ClampPitch(targetRot);
    float horizontalVelocity{(rigidBody_->GetLinearVelocity() * Vector3(1.0f, 0.0f, 1.0f)).Length()};
    node_->SetRotation(rot.Slerp(targetRot, Clamp(timeStep * 2.3f * horizontalVelocity, 0.0f, 1.0f)));
}

void Walker::Jump()
{
    if (sinceJump_ < jumpInterval_)
        return;

    else if (onGround_){
        sinceJump_ = 0.0f;
        onGround_ = false;
        Vector3 jumpImpulse{Vector3::UP * jumpForce_ +
                    rigidBody_->GetLinearVelocity() * jumpForce_ * 0.023f};
        rigidBody_->ApplyImpulse(jumpImpulse);
    } else if (doubleJumper_ && !doubleJumped_){
        doubleJumped_ = true;
        Vector3 jumpImpulse{Vector3::UP * jumpForce_ * 0.5f +
                    rigidBody_->GetLinearVelocity().Normalized() * jumpForce_ * 0.0666f};
        rigidBody_->ApplyImpulse(jumpImpulse);
    }
}

void Walker::ClampPitch(Quaternion& rot)
{
    float correction{rot.EulerAngles().x_ - 60.0f};
    if (correction > 0.0f)
        rot = Quaternion(-correction, node_->GetRight()) * rot;
}
