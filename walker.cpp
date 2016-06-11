/* Blip 'n Blup
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "walker.h"
#include "castmaster.h"

Walker::Walker(Context* context) : Controllable(context),
    onGround_{false},
    doubleJumper_{false},
    doubleJumped_{false},
    sinceJump_{0.0f}
{
    maxPitch_ = 60.0f;
    minPitch_ = -80.0f;
    SetUpdateEventMask(USE_UPDATE);
}

void Walker::OnNodeSet(Node *node)
{
    Controllable::OnNodeSet(node);

    rigidBody_->SetLinearRestThreshold(0.0f);
    rigidBody_->SetLinearDamping(0.5f);
    rigidBody_->SetRestitution(0.23f);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetRollingFriction(0.0f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Walker, HandleNodeCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Walker, HandleNodeCollision));
}

void Walker::Update(float timeStep)
{
    sinceJump_ += timeStep;

    bool run{actions_[0] * onGround_};
    float maxWalkSpeed{maxRunSpeed_ * (0.6f + 0.4f * run)};
    float walkThrust{runThrust_ * (0.8f + 0.2f * run)};

    PODVector<RigidBody*> bodies{};
    rigidBody_->GetCollidingBodies(bodies);
    if (!bodies.Size())
        onGround_ = false;

    //Apply movement
    if (move_.Length() > 0.0f){
        AlignWithMovement(timeStep);

        rigidBody_->SetFriction(1.0f * onGround_);
        Vector3 force{walkThrust * move_};
        Vector3 planarVelocity{rigidBody_->GetLinearVelocity() * Vector3::ONE-Vector3::UP};

        if (planarVelocity.Length() <= maxWalkSpeed
         || planarVelocity.DotProduct(force) < 0.0f)
        {
            float remainingSpeed{Clamp(
                            maxWalkSpeed - planarVelocity.Length() * (1.0f-(planarVelocity.Angle(force)/90.0f)),
                            0.0f, maxWalkSpeed)};
            float maxThrust = Max(walkThrust * remainingSpeed, 0.0f);
            if (force.Length() > maxThrust)
                force = force.Normalized() * maxThrust;

            rigidBody_->ApplyForce(force * timeStep);
        }


    } else rigidBody_->SetFriction(2.0f * onGround_);

    if (onGround_)
        AlignWithFloor(timeStep);
    else {
        AlignWithVelocity(timeStep);
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

        if ((node_->GetPosition().y_ - contactPosition.y_) > collider_->GetSize().x_/2.0f
         && sinceJump_ > jumpInterval_){

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
            node_->SetRotation(rot.Slerp(targetRot, Clamp(timeStep * (3.0f + angleDelta), 0.0f, 1.0f)));
    }
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


