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
#include "resourcemaster.h"
#include "castmaster.h"

Walker::Walker(Context* context) : Controllable(context),
    onGround_{false},
    maxSlope_{23.0f},
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
    rigidBody_->SetLinearDamping(0.23f);
    rigidBody_->SetRestitution(0.23f);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetRollingFriction(0.0f);
    rigidBody_->SetAngularFactor(Vector3::UP);
//    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Walker, HandleNodeCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Walker, HandleNodeCollision));
}

void Walker::Update(float timeStep)
{
    Controllable::Update(timeStep);

    sinceJump_ += timeStep;

    float run{ Clamp(2.3f * actionSince_[0], 0.0f, 1.0f) * onGround_ };
    float maxWalkSpeed{ maxRunSpeed_ * (0.6f + 0.4f * run) };
    float walkThrust{ runThrust_ * (0.8f + 0.2f * run) };

    PODVector<RigidBody*> bodies{};
    rigidBody_->GetCollidingBodies(bodies);
    if (!bodies.Size())
        onGround_ = false;

    //Apply movement
    if (move_.Length() > 0.0f){
        AlignWithMovement(timeStep);

        rigidBody_->SetFriction(0.42f * onGround_);
        Vector3 force{ walkThrust * move_ };
        Vector3 planarVelocity{ rigidBody_->GetLinearVelocity() * Vector3::ONE - Vector3::UP };

        if (planarVelocity.Length() <= maxWalkSpeed
         || planarVelocity.DotProduct(force) < 0.0f)
        {
            float remainingSpeed{ Min(
                            maxWalkSpeed - planarVelocity.Length() * (1.0f - (planarVelocity.Angle(force) / 90.0f)),
                            maxWalkSpeed)};

            float maxThrust = Max(walkThrust * remainingSpeed * rigidBody_->GetMass(), 0.0f);
            if (force.Length() > maxThrust)
                force = force.Normalized() * maxThrust;

            rigidBody_->ApplyForce(force * timeStep);
        }


    } else rigidBody_->SetFriction(2.0f * onGround_);

    //Update animation and orientation
    float velocity{ rigidBody_->GetLinearVelocity().Length() };

    if (onGround_) {
        AlignWithFloor(timeStep);

        if (animCtrl_) {
            if (velocity < 0.01f) {
                animCtrl_->PlayExclusive(idleAnim_, 0, true, 0.1f);

            } else {
                animCtrl_->PlayExclusive(walkAnim_, 0, true, 0.1f);
                animCtrl_->SetSpeed(walkAnim_, velocity * 0.38f);
            }
        }
    } else {
        AlignWithVelocity(timeStep);

        if (animCtrl_) {
            animCtrl_->PlayExclusive(midairAnim_, 0, true, 0.1f);
            animCtrl_->SetSpeed(midairAnim_, velocity * 0.23f);
        }
    }
}
void Walker::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;
    MemoryBuffer contacts{ eventData[NodeCollisionStart::P_CONTACTS].GetBuffer() };

    MemoryBuffer contactsCopy{contacts};
    CheckOnGround(contactsCopy);
}
void Walker::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    MemoryBuffer contacts{ eventData[NodeCollision::P_CONTACTS].GetBuffer() };

    MemoryBuffer contactsCopy{contacts};
    CheckOnGround(contactsCopy);
}
void Walker::CheckOnGround(MemoryBuffer& contacts)
{
    while (!contacts.IsEof())
    {
        Vector3 contactPosition{ contacts.ReadVector3() };
        Vector3 contactNormal{ contacts.ReadVector3() };
        /*float contactDistance = */contacts.ReadFloat();
        /*float contactImpulse = */contacts.ReadFloat();

        if (contactNormal.Angle(Vector3::UP) < maxSlope_){

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
    footRay.origin_ = node_->GetPosition() - node_->GetUp() * (collider_->GetSize().y_ - collider_->GetSize().x_) * 0.5f;
    footRay.direction_ = Vector3::DOWN;
    if (GetSubsystem<CastMaster>()->PhysicsRayCast(hitResults, footRay, 0.8f, 1)
     && (hitResults[0].normal_.Angle(Vector3::UP)) < maxSlope_) {

        float angleDelta{ node_->GetUp().Angle(hitResults[0].normal_)};
        if (angleDelta > 1.0f) {
            node_->RotateAround(footRay.origin_, Quaternion::IDENTITY.Slerp(Quaternion(node_->GetUp(), hitResults[0].normal_), Clamp(timeStep * 23.0f, 0.0f, 1.0f)), TS_WORLD);
        }
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

        node_->GetOrCreateComponent<SoundSource>()->Play(RM->GetSample("Jump"));
    } else if (doubleJumper_ && !doubleJumped_){
        doubleJumped_ = true;
        Vector3 jumpImpulse{Vector3::UP * jumpForce_ * 0.5f +
                    rigidBody_->GetLinearVelocity().Normalized() * jumpForce_ * 0.0666f};
        rigidBody_->ApplyImpulse(jumpImpulse);
    }
}


