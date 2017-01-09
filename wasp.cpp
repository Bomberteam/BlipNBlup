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

#include "wasp.h"
#include "catchable.h"
#include "rage.h"
#include "castmaster.h"
#include "resourcemaster.h"

Wasp::Wasp(Context* context) : Flyer(context)
{
    maxFlySpeed_ = 13.0f;
    flyThrust_ = 500.0f;

    chargeForce_ = 5000.0f;
    chargeInterval_ = 5.0f;

    SetUpdateEventMask(USE_UPDATE);
}

void Wasp::RegisterObject(Context* context)
{
    context->RegisterFactory<Wasp>();
}

void Wasp::OnNodeSet(Node *node)
{   Flyer::OnNodeSet(node);

    Catchable*  catchable{ node_->CreateComponent<Catchable>() };
    rage_ = node_->CreateComponent<Rage>();
    rage_->SetCooldown(10.0f);

    catchable->SetReleaseTime(23.0f);
    SubscribeToEvent(node_, E_CATCH, URHO3D_HANDLER(Wasp, OnCatched));
    SubscribeToEvent(node_, E_RELEASE, URHO3D_HANDLER(Wasp, OnReleased));

    node_->Translate(Quaternion(Random(360.0f), Vector3::UP) * Random(0.5f, 4.2f) * Vector3::FORWARD + Vector3::UP * Random(5.0f, 7.0f));
    node_->Rotate(Quaternion(Random(360.0f), Vector3::UP));

    model_->SetModel(RM->GetModel("Wasp"));
    model_->SetMaterial(RM->GetMaterial("VColOutline")->Clone());

    rigidBody_->SetCollisionLayer(LAYER(3));
    rigidBody_->SetCollisionMask(LAYER(0) + LAYER(1) + LAYER(2) + LAYER(3));
    rigidBody_->SetMass(2.0f);
    collider_->SetSphere(1.0f);

    animCtrl_->Play("Models/Wasp_Fly.ani", 0, true, 0.0f);
    animCtrl_->SetTime("Models/Wasp_Fly.ani", randomizer_);
}

void Wasp::Update(float timeStep)
{
    move_ = Quaternion(5.0f - 2.0f * rage_->GetAnger(), Vector3::UP) * node_->GetDirection(); ///Should be handled by AI component

    Flyer::Update(timeStep);
    animCtrl_->SetSpeed("Models/Wasp_Fly.ani", 1.0f + rigidBody_->GetLinearVelocity().Length());

    model_->GetMaterial()->SetShaderParameter("MatEmissiveColor", Color::RED * MC->Sine(2.3f, 0.0f, 0.5f * rage_->GetAnger(), randomizer_));
    maxFlySpeed_ = 13.0f + 7.0f * rage_->GetAnger();
    flyThrust_ = 500.0f + 200.0f * rage_->GetAnger();
}

void Wasp::OnCatched(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    animCtrl_->PlayExclusive("Models/Wasp_Caught.ani", 0, true, 0.1f);

}
void Wasp::OnReleased(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    rage_->FillAnger();

    animCtrl_->PlayExclusive("Models/Wasp_Fly.ani", 0, true, 0.1f);
    animCtrl_->SetSpeed("Models/Wasp_Fly.ani", 5.0f);
}
