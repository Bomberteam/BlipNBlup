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

#include "flyer.h"
#include "catchable.h"
#include "castmaster.h"


Flyer::Flyer(Context* context) : Controllable(context),
    altitude_{2.0f},
    flyHeight_{2.3f + randomizer_}
{
}

void Flyer::OnNodeSet(Node* node)
{
    Controllable::OnNodeSet(node);

    rigidBody_->SetLinearRestThreshold(0.0f);
    rigidBody_->SetLinearDamping(0.5f);
    rigidBody_->SetRestitution(0.23f);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetRollingFriction(0.5f);
    rigidBody_->SetAngularFactor(Vector3::UP);
}

void Flyer::Update(float timeStep)
{
    if (node_->HasComponent<Catchable>()
     && node_->GetComponent<Catchable>()->IsCaught())
        return;

    //Hover
    rigidBody_->ApplyForce(Vector3::UP * flyThrust_ * 10.0f *
                           pow(altitude_ - node_->GetPosition().y_ - rigidBody_->GetLinearVelocity().y_ * 0.1f, 0.5f) *
                           MC->Sine(1.0f - 0.23f * randomizer_, 1.0f, 2.0f, randomizer_) * timeStep);
    //Fly
    if (rigidBody_->GetLinearVelocity().Length() < maxFlySpeed_
            || (rigidBody_->GetLinearVelocity().Normalized() + move_.Normalized()).Length() < 1.0f)
    {
        Vector3 velocity{rigidBody_->GetLinearVelocity()};
        Vector3 force{move_ * flyThrust_};
        float remainingFlySpeed{maxFlySpeed_ - velocity.Length() * (1.0f-(velocity.Angle(force)/90.0f))};
        float maxThrust = Max(flyThrust_ * remainingFlySpeed, 0.0f);
        if (force.Length() > maxThrust)
            force = force.Normalized() * maxThrust;

        rigidBody_->ApplyForce(force * timeStep);
    }
    CorrectAltitude();
    AlignWithMovement(timeStep);
}

void Flyer::CorrectAltitude()
{
    PODVector<PhysicsRaycastResult> results{};
    Ray floorRay(node_->GetPosition()
                 + Vector3::UP * 5.0f,
                 Vector3::DOWN);
    GetSubsystem<CastMaster>()->PhysicsRayCast(results, floorRay, 1024.0f, LAYER(0));

    if (results.Size())
        altitude_ = results[0].position_.y_ + flyHeight_;
}
