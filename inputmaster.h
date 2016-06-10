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

#ifndef INPUTMASTER_H
#define INPUTMASTER_H

#include <Urho3D/Urho3D.h>
#include "controllable.h"
#include "luckey.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

enum class MasterInputAction { UP, RIGHT, DOWN, LEFT, CONFIRM, CANCEL, PAUSE, MENU, SCREENSHOT };
enum class PlayerInputAction { FORWARD, BACK, LEFT, RIGHT, UP, DOWN, RUN, JUMP, BUBBLE };

struct InputActions {
    Vector<MasterInputAction> master_;
    HashMap<int, Vector<PlayerInputAction>> player_;
};

class InputMaster : public Object
{
    URHO3D_OBJECT(InputMaster, Object);
public:
    InputMaster(Context* context);
    void SetPlayerControl(int player, Controllable* controllable);
private:
    HashMap<int, MasterInputAction> keyBindingsMaster_;
    HashMap<int, MasterInputAction> buttonBindingsMaster_;
    HashMap<int, HashMap<int, PlayerInputAction>> keyBindingsPlayer_;
    HashMap<int, HashMap<int, PlayerInputAction>> buttonBindingsPlayer_;

    HashMap<int, Controllable*> controlledByPlayer_;

    Vector<int> pressedKeys_;
    Vector<LucKey::SixaxisButton> pressedJoystickButtons_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleJoyButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleJoyButtonUp(StringHash eventType, VariantMap &eventData);

    void HandleActions(const InputActions &actions);
    void HandlePlayerAction(PlayerInputAction action, CharacterID player = BLIP);
    Vector3 GetMoveFromActions(Vector<PlayerInputAction>* actions);
};

#endif // INPUTMASTER_H
