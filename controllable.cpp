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

#include "controllable.h"

Controllable::Controllable(Context* context) : LogicComponent(context),
    randomizer_{Random()},
    controlled_{false},
    move_{},
    aim_{},
    maxPitch_{90.0f},
    minPitch_{0.0f},

    actions_{},
    actionSince_{},

    model_{},
    rigidBody_{},
    collider_{},
    animCtrl_{}
{
    for (int a{0}; a < 4; ++a)
        actionSince_[a] = 0.0f;
}
void Controllable::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateChild("Model")->CreateComponent<AnimatedModel>();
    animCtrl_ = model_->GetNode()->CreateComponent<AnimationController>();

    rigidBody_ = node_->CreateComponent<RigidBody>();
    collider_ = node_->CreateComponent<CollisionShape>();

    model_->SetCastShadows(true);
}
void Controllable::Update(float timeStep)
{
    for (int a{0}; a < static_cast<int>(actions_.size()); ++a){

        if (actions_[a])
            actionSince_[a] += timeStep;
    }
}

void Controllable::SetMove(Vector3 move)
{
    if (move.Length() > 1.0f)
        move.Normalize();
    move_ = move;
}

void Controllable::SetActions(std::bitset<4> actions)
{
    if (actions == actions_)
        return;
    else
        for (int i{0}; i < static_cast<int>(actions.size()); ++i){

            if (actions[i] != actions_[i]){
                actions_[i] = actions[i];

                if (actions[i])
                    HandleAction(i);
                else
                    actionSince_[i] = 0.0f;
            }
        }
}
void Controllable::HandleAction(int actionId)
{ (void)actionId;
}

void Controllable::AlignWithMovement(float timeStep)
{
    Quaternion rot{node_->GetRotation()};
    Quaternion targetRot{};
    targetRot.FromLookRotation(move_);
    rot = rot.Slerp(targetRot, Clamp(timeStep * 23.0f, 0.0f, 1.0f));
    node_->SetRotation(rot);
}
void Controllable::AlignWithVelocity(float timeStep)
{
    Quaternion targetRot{};
    Quaternion rot{node_->GetRotation()};
    targetRot.FromLookRotation(rigidBody_->GetLinearVelocity());
    ClampPitch(targetRot);
    float horizontalVelocity{(rigidBody_->GetLinearVelocity() * Vector3(1.0f, 0.0f, 1.0f)).Length()};
    node_->SetRotation(rot.Slerp(targetRot, Clamp(timeStep * horizontalVelocity, 0.0f, 1.0f)));
}

void Controllable::ClampPitch(Quaternion& rot)
{
    float maxCorrection{rot.EulerAngles().x_ - maxPitch_};
    if (maxCorrection > 0.0f)
        rot = Quaternion(-maxCorrection, node_->GetRight()) * rot;

    float minCorrection{rot.EulerAngles().x_ - minPitch_};
    if (minCorrection < 0.0f)
        rot = Quaternion(-minCorrection, node_->GetRight()) * rot;
}



