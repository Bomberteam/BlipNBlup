/* Blip 'n Blup
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

#include "fish.h"

Fish::Fish(Context *context) : Controllable(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Fish::RegisterObject(Context* context)
{
    context->RegisterFactory<Fish>();
}

void Fish::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(CACHE->GetResource<Model>("Models/Blip.mdl"));
    model_->SetMaterial(CACHE->GetResource<Material>("Materials/VCol.xml"));
    model_->SetCastShadows(true);

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(2.0f);
    rigidBody_->SetFriction(1.0f);
    rigidBody_->SetLinearDamping(0.7f);
    rigidBody_->SetRestitution(0.23f);
    rigidBody_->SetAngularDamping(0.8f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    collider_ = node_->CreateComponent<CollisionShape>();
    collider_->SetCylinder(0.5f, 1.0f);
}
void Fish::BecomeBlup()
{
    model_->SetModel(CACHE->GetResource<Model>("Models/Blup.mdl"));
    node_->Translate(Vector3::RIGHT);
}
void Fish::Update(float timeStep)
{
    Quaternion rot{node_->GetRotation()};
    Quaternion targetRot{}; targetRot.FromLookRotation(move_);
    rot = rot.Slerp(targetRot, Clamp(timeStep * 5.0f, 0.0f, 1.0f));
    node_->SetRotation(rot);

    if (move_.Length() > 0.0f){
        rigidBody_->SetFriction(0.1f);

        rigidBody_->ApplyForce((600.0f * actions_[0] + 500.0f) * move_ * timeStep);
    }
}
void Fish::HandleAction(int actionId)
{
    switch (actionId){
    case 0:
        break;
    case 1: Jump();
        break;
    case 2: //Bubble();
        break;
    case 3:
        break;
    default: break;
    }
}

void Fish::Jump()
{
    rigidBody_->ApplyImpulse(Vector3::UP * 23.0f);
}
