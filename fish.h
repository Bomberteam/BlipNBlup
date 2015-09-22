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

#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>
#include "mastercontrol.h"
#include "helper.h"

using namespace Urho3D;

class Fish : public Object
{
    friend class MasterControl;
    OBJECT(Fish);
public:
    Fish(Context *context, MasterControl *masterControl, bool blip = true);

    Vector3 GetPosition() { return rootNode_->GetWorldPosition(); }
    bool IsAlive() { return alive_; }
    bool IsHuman() { return human_; }
    void SetMovement(const Vector2 movement);
    void Move(float timeStep);
    void Jump();
    void JumpRelease() { if (onGround_) jumpReleased_ = true; }
    void Run(const bool run) { running_ = run; }
private:
    MasterControl * masterControl_;

    Node* rootNode_;
    AnimatedModel* model_;
    RigidBody* rigidBody_;
    CollisionShape* collider_;
    AnimationController* animCtrl_;
    bool blip_;
    bool human_;
    bool alive_;
    bool running_;
    bool onGround_;
    bool doubleJumped_;
    bool jumpReleased_;
    float sinceJumped_;
    float blink_;

    float maxWalkSpeed_;
    float runThrust_;
    Vector2 movement_;

    float jumpForce_;
    float jumpInterval_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollisionStart(StringHash eventType, VariantMap &eventData);
    void Blink();
    void Think();
};

#endif // PLAYER_H
