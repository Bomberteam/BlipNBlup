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

#include "player.h"

Player::Player(Context *context, MasterControl *masterControl, bool blip) : Object(context),
    masterControl_{masterControl},
    blip_{blip},
    blink_{0.0f},
    walkSpeed_{230.0f},
    movement_{Vector2::ZERO}
{
    rootNode_ = masterControl->world_.scene_->CreateChild("Player");
    if (blip_) rootNode_->SetName("Blip");
    else rootNode_->SetName("Blup");


    model_ = rootNode_->CreateComponent<AnimatedModel>();
    if (blip_) model_->SetModel(masterControl->cache_->GetResource<Model>("Resources/Models/Blip.mdl"));
    else model_->SetModel(masterControl->cache_->GetResource<Model>("Resources/Models/Blup.mdl"));
    model_->SetMaterial(masterControl->cache_->GetResource<Material>("Resources/Materials/VCol.xml"));
    model_->SetCastShadows(true);
    model_->SetMorphWeight(1, !blip_);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Resources/Models/Walk.ani", 0, true);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    collider_ = rootNode_->CreateComponent<CollisionShape>();
    collider_->SetCapsule(3.0f, 3.0f, Vector3::UP*1.5f);

    SubscribeToEvent(E_UPDATE, HANDLER(Player, HandleUpdate));
}

void Player::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
    rootNode_->Rotate(Quaternion(timeStep * 23.0f, Vector3::UP));
    //animCtrl_->SetSpeed("Resources/Models/Walk.ani", rigidBody_->GetLinearVelocity().Length());
    if (blink_ < 0.0001f && !Random(23)) blink_ = 1.0f;
    blink_ *= 0.9f;
    model_->SetMorphWeight(0, blink_);

    rigidBody_->ApplyForce(timeStep * walkSpeed_ * Vector3(movement_.x_, 0.0f, movement_.y_));
}
