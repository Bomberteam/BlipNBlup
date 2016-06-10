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

#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>
#include "walker.h"

using namespace Urho3D;

enum class FishState{ STAND, WALK, RUN, JUMP, FALL, OUT};

class Fish : public Walker
{
    URHO3D_OBJECT(Fish, Walker);
public:
    Fish(Context* context);
    static void RegisterObject(Context* context);

    virtual void OnNodeSet(Node* node);
    void BecomeBlup();
    virtual void Update(float timeStep);

protected:
    float bubbleInterval_;
    float sinceBubble_;

    virtual void HandleAction(int actionId);
private:
    float blink_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollision(StringHash eventType, VariantMap &eventData);

    void Blink();
    void BlowBubble();
};

#endif // PLAYER_H
