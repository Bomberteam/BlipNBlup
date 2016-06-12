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

#ifndef WALKER_H
#define WALKER_H

#include <Urho3D/Urho3D.h>
#include "controllable.h"

class Walker : public Controllable
{
    URHO3D_OBJECT(Walker, Controllable);
public:
    Walker(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

protected:
    float maxRunSpeed_;
    float runThrust_;
    float jumpForce_;


    bool onGround_;
    float maxSlope_;
    bool doubleJumper_;
    bool doubleJumped_;
    float jumpInterval_;
    float sinceJump_;

    void Jump();

    void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
private:
    void CheckOnGround(MemoryBuffer& contacts);
    void AlignWithFloor(float timeStep);
};

#endif // WALKER_H
