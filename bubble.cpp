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

#include "bubble.h"
#include "wind.h"
#include "resourcemaster.h"

Bubble::Bubble(Context* context) : LogicComponent(context)
{
}
void Bubble::RegisterObject(Context* context)
{
    context->RegisterFactory<Bubble>();
}

void Bubble::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(RM->GetModel("Bubble"));
    model_->SetMaterial(RM->GetMaterial("Bubble"));

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetCollisionMask(LAYER(0) + LAYER(2));
    rigidBody_->SetMass(0.1f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetGravityOverride(Vector3::UP * 9.0f);

    collider_ = node_->CreateComponent<CollisionShape>();
    collider_->SetSphere(1.0f);

    node_->CreateComponent<Wind>();
}
void Bubble::Update(float timeStep)
{
    node_->Rotate(Quaternion(13.0f * timeStep, 23.0f * timeStep, 34.0f * timeStep));
}
