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

#ifndef BUBBLE_H
#define BUBBLE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

#define BUBBLE_WEIGHT 0.1f

class Catchable;

class Bubble : public LogicComponent
{
    URHO3D_OBJECT(Bubble, LogicComponent);
public:
    Bubble(Context* context);
    static void RegisterObject(Context* context);

    virtual void OnNodeSet(Node* node);

    void Update(float timeStep);
    void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);

    bool IsEmpty() const { return empty_; }
private:
    bool empty_;
    SharedPtr<StaticModel> model_;
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<CollisionShape> collider_;

    Catchable* catchable_;
    void UpdateRigidBody(float otherMass = 0.0f);
};

#endif // BUBBLE_H
