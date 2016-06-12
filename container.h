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

#ifndef CONTAINER_H
#define CONTAINER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Catchable;

class Container : public LogicComponent
{
    URHO3D_OBJECT(Container, LogicComponent);
public:
    Container(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

    bool IsEmpty() const { return empty_; }
protected:
    bool Contain(Catchable* catchable);
    SharedPtr<StaticModel> model_;
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<CollisionShape> collider_;

    void Release();
private:
    Catchable* catchable_;

    bool empty_;
    bool escapable_;

    float containTime_;
    float sinceContain_;
};

#endif // CONTAINER_H
