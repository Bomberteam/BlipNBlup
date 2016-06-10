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
#include "castmaster.h"


Flyer::Flyer(Context* context) : Controllable(context),
    altitude_{2.0f}
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
    //Hover
    rigidBody_->ApplyForce(Vector3::UP * flyThrust_ * 10.0f *
                           (altitude_ - node_->GetPosition().y_) *
                           MC->Sine(2.3f, 0.7f, 1.0f) * timeStep);
    //Fly
    if (rigidBody_->GetLinearVelocity().Length() < maxFlySpeed_
            || (rigidBody_->GetLinearVelocity().Normalized() + move_.Normalized()).Length() < 1.0f)
    {
        rigidBody_->ApplyForce(move_ * flyThrust_ * timeStep);
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
        altitude_ = results[0].position_.y_ + 3.0f;
}
