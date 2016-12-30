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
#include "catchable.h"
#include "resourcemaster.h"
#include "effectmaster.h"

Bubble::Bubble(Context* context) : Container(context)
{
}
void Bubble::RegisterObject(Context* context)
{
    context->RegisterFactory<Bubble>();
}

void Bubble::OnNodeSet(Node *node)
{
    Container::OnNodeSet(node);
    node_->CreateComponent<Wind>();

    node_->SetRotation(Quaternion(Random(360.0f), Random(360.0f), Random(360.0f)));
    FX->ScaleTo(node_, 1.0f, 0.1f);

    model_->SetModel(RM->GetModel("Bubble"));
    model_->SetMaterial(RM->GetMaterial("Bubble"));
    model_->SetCastShadows(true);

    rigidBody_->SetCollisionLayer(LAYER(1));
    rigidBody_->SetCollisionMask(LAYER(0) + LAYER(1) + LAYER(2));
    rigidBody_->SetRollingFriction(0.42f);
    rigidBody_->SetLinearRestThreshold(0.0f);
    UpdateRigidBody();

    collider_->SetSphere(1.0f);


    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Bubble, HandleNodeCollisionStart));
}
void Bubble::Update(float timeStep)
{
    Container::Update(timeStep);

    if (!IsEmpty()){
        rigidBody_->ApplyForce((node_->GetChild(static_cast<unsigned>(0))->GetWorldPosition() - node_->GetPosition()) * timeStep * 420.0f * rigidBody_->GetMass());
    } else {
        rigidBody_->ApplyTorque(Vector3(13.0f * timeStep, 23.0f * timeStep, 34.0f * timeStep));
    }
}

void Bubble::UpdateRigidBody(float otherMass)
{
    if (otherMass == 0.0f){
        rigidBody_->SetMass(BUBBLE_WEIGHT);
        rigidBody_->SetLinearDamping(0.9f);
        rigidBody_->SetAngularDamping(0.1f);
        rigidBody_->SetGravityOverride(Vector3::UP * 9.0f);
        rigidBody_->ResetForces();
        rigidBody_->SetAngularVelocity(Vector3::ZERO);
        rigidBody_->SetLinearVelocity(Vector3::ZERO);
        rigidBody_->SetRestitution(0.5f);

        rigidBody_->ReAddBodyToWorld();
    } else {
        rigidBody_->SetMass(Max(0.5f * otherMass - BUBBLE_WEIGHT, BUBBLE_WEIGHT));
        rigidBody_->SetLinearDamping(0.8f);
        rigidBody_->SetAngularDamping(0.2f);
        rigidBody_->SetGravityOverride(Vector3::ZERO);
        rigidBody_->ResetForces();
        rigidBody_->SetAngularVelocity(Vector3::ZERO);
        rigidBody_->SetLinearVelocity(Vector3::ZERO);
        rigidBody_->SetRestitution(1.0f);

        rigidBody_->ReAddBodyToWorld();
    }
}

void Bubble::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    RigidBody* otherBody{static_cast<RigidBody*>(eventData[NodeCollisionStart::P_OTHERBODY].GetPtr())};
    Node* otherNode{static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr())};

    MemoryBuffer contacts(eventData[NodeCollision::P_CONTACTS].GetBuffer());
    /*Vector3 contactPosition{*/contacts.ReadVector3();//};
    /*Vector3 contactNormal{*/contacts.ReadVector3();//};
    /*float contactDistance{*/contacts.ReadFloat();//};
    float contactImpulse{contacts.ReadFloat()};

    //Try to contain if catchable
    if (IsEmpty() && contactImpulse > 0.5f && otherNode->HasComponent<Catchable>()) {

        float otherMass{otherBody->GetMass()};
        Catchable* catchable{otherNode->GetComponent<Catchable>()};
        if (Contain(catchable)) {

            UpdateRigidBody(otherMass);
            FX->TransformTo(node_,
                            0.5f * (node_->GetPosition() + catchable->GetNode()->GetWorldPosition()),
                            node_->GetRotation(), 0.1f);
            return;
        }
    }
}
