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

#include "inputmaster.h"
#include "bnbcam.h"
#include "fish.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context) : Object(context)
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(SB_DPAD_UP)]    = MasterInputAction::UP;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(SB_DPAD_DOWN)]  = MasterInputAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(SB_DPAD_LEFT)]  = MasterInputAction::LEFT;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(SB_DPAD_RIGHT)] = MasterInputAction::RIGHT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(SB_CROSS)]      = MasterInputAction::CONFIRM;
    keyBindingsMaster_[KEY_ESCAPE] = buttonBindingsMaster_[static_cast<int>(SB_CIRCLE)]     = MasterInputAction::CANCEL;
    keyBindingsMaster_[KEY_PAUSE]  = buttonBindingsMaster_[static_cast<int>(SB_START)]      = MasterInputAction::PAUSE;
    keyBindingsMaster_[KEY_ESCAPE] = MasterInputAction::MENU;

    keyBindingsPlayer_[1][KEY_W] = keyBindingsPlayer_[1][KEY_UP]     = PlayerInputAction::FORWARD;
    keyBindingsPlayer_[1][KEY_S] = keyBindingsPlayer_[1][KEY_DOWN]   = PlayerInputAction::BACK;
    keyBindingsPlayer_[1][KEY_A] = keyBindingsPlayer_[1][KEY_LEFT]   = PlayerInputAction::LEFT;
    keyBindingsPlayer_[1][KEY_D] = keyBindingsPlayer_[1][KEY_RIGHT]  = PlayerInputAction::RIGHT;
    keyBindingsPlayer_[1][KEY_C] = keyBindingsPlayer_[1][KEY_LSHIFT] = PlayerInputAction::RUN;
    keyBindingsPlayer_[1][KEY_V] = keyBindingsPlayer_[1][KEY_ALT]    = PlayerInputAction::JUMP;
    keyBindingsPlayer_[1][KEY_B] = keyBindingsPlayer_[1][KEY_SPACE]  = PlayerInputAction::BUBBLE;

    keyBindingsPlayer_[2][KEY_KP_8]   = PlayerInputAction::FORWARD;
    keyBindingsPlayer_[2][KEY_KP_5]   = PlayerInputAction::BACK;
    keyBindingsPlayer_[2][KEY_KP_2]   = PlayerInputAction::BACK;
    keyBindingsPlayer_[2][KEY_KP_4]   = PlayerInputAction::LEFT;
    keyBindingsPlayer_[2][KEY_KP_6]   = PlayerInputAction::RIGHT;
    keyBindingsPlayer_[2][KEY_RETURN] = PlayerInputAction::RUN;
    keyBindingsPlayer_[2][KEY_RSHIFT] = PlayerInputAction::JUMP;
    keyBindingsPlayer_[2][KEY_RCTRL]  = PlayerInputAction::BUBBLE;

    buttonBindingsPlayer_[1][SB_DPAD_UP]    = PlayerInputAction::FORWARD;
    buttonBindingsPlayer_[1][SB_DPAD_DOWN]  = PlayerInputAction::BACK;
    buttonBindingsPlayer_[1][SB_DPAD_LEFT]  = PlayerInputAction::LEFT;
    buttonBindingsPlayer_[1][SB_DPAD_RIGHT] = PlayerInputAction::RIGHT;
    buttonBindingsPlayer_[1][SB_SQUARE]     = PlayerInputAction::RUN;
    buttonBindingsPlayer_[1][SB_CROSS]      = PlayerInputAction::JUMP;
    buttonBindingsPlayer_[1][SB_CIRCLE]     = PlayerInputAction::BUBBLE;
    buttonBindingsPlayer_[1][SB_TRIANGLE]   = PlayerInputAction::KICK;

    buttonBindingsPlayer_[2][SB_DPAD_UP]    = PlayerInputAction::FORWARD;
    buttonBindingsPlayer_[2][SB_DPAD_DOWN]  = PlayerInputAction::BACK;
    buttonBindingsPlayer_[2][SB_DPAD_LEFT]  = PlayerInputAction::LEFT;
    buttonBindingsPlayer_[2][SB_DPAD_RIGHT] = PlayerInputAction::RIGHT;
    buttonBindingsPlayer_[2][SB_SQUARE]     = PlayerInputAction::RUN;
    buttonBindingsPlayer_[2][SB_CROSS]      = PlayerInputAction::JUMP;
    buttonBindingsPlayer_[2][SB_CIRCLE]     = PlayerInputAction::BUBBLE;
    buttonBindingsPlayer_[2][SB_TRIANGLE]   = PlayerInputAction::KICK;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    InputActions activeActions{};
    for (int p : MC->GetPlayers()){
        Vector<PlayerInputAction> emptyActions{};
        activeActions.player_[p] = emptyActions;
    }

    //Convert key presses to actions
    for (int key : pressedKeys_){
        //Check for master key presses
        if (keyBindingsMaster_.Contains(key)){
            MasterInputAction action{keyBindingsMaster_[key]};
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        //Check for player key presses
        for (int p : MC->GetPlayers())
            if (keyBindingsPlayer_[p].Contains(key)){
                PlayerInputAction action{keyBindingsPlayer_[p][key]};
                if (!activeActions.player_[p].Contains(action))
                    activeActions.player_[p].Push(action);
            }
    }
    //Check for joystick button presses
    for (int p : MC->GetPlayers()){

        for (int button : pressedJoystickButtons_[p-1])
            if (buttonBindingsPlayer_[p].Contains(button)){
                PlayerInputAction action{ buttonBindingsPlayer_[p][button]};
                if (!activeActions.player_[p].Contains(action))
                    activeActions.player_[p].Push(action);
            }
    }

    //Handle the registered actions
    HandleActions(activeActions);
}

void InputMaster::SetPlayerControl(int player, Controllable* controllable)
{
    if (controlledByPlayer_.Contains(player))
        controlledByPlayer_[player]->ResetInput();
    controlledByPlayer_[player] = controllable;
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    for (MasterInputAction action : actions.master_){
        switch (action){
        case MasterInputAction::UP:                 break;
        case MasterInputAction::DOWN:               break;
        case MasterInputAction::LEFT:               break;
        case MasterInputAction::RIGHT:              break;
        case MasterInputAction::CONFIRM:            break;
        case MasterInputAction::CANCEL:             break;
        case MasterInputAction::PAUSE:              break;
        case MasterInputAction::MENU: MC->Exit();   break;
        default: break;
        }
    }


    //Handle player actions
    for (int p : MC->GetPlayers()){
        auto playerInputActions = actions.player_[p];

        Controllable* controlled{controlledByPlayer_[p]};
        if (controlled){

            Vector3 stickMove{ Vector3(leftStickPosition_[p-1].x_, 0.0f, leftStickPosition_[p-1].y_) };
            CorrectForCameraYaw(stickMove);

            controlled->SetMove(GetMoveFromActions(playerInputActions) + stickMove);

            std::bitset<4>restActions{};
            restActions[0] = playerInputActions->Contains(PlayerInputAction::RUN);
            restActions[1] = playerInputActions->Contains(PlayerInputAction::JUMP);
            restActions[2] = playerInputActions->Contains(PlayerInputAction::BUBBLE);
            restActions[3] = playerInputActions->Contains(PlayerInputAction::KICK);

            controlled->SetActions(restActions);
        }
    }
}

Vector3 InputMaster::GetMoveFromActions(Vector<PlayerInputAction>* actions)
{
    Vector3 move{Vector3::RIGHT *
                (actions->Contains(PlayerInputAction::RIGHT) -
                 actions->Contains(PlayerInputAction::LEFT))

                + Vector3::UP *
                (actions->Contains(PlayerInputAction::UP) -
                 actions->Contains(PlayerInputAction::DOWN))

                + Vector3::FORWARD *
                (actions->Contains(PlayerInputAction::FORWARD) -
                 actions->Contains(PlayerInputAction::BACK))
                };

    CorrectForCameraYaw(move);

    return move;
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{eventData[KeyDown::P_KEY].GetInt()};

    if (!pressedKeys_.Contains(key))
        pressedKeys_.Push(key);

    switch (key){
    case KEY_ESCAPE:{
        MC->Exit();
    } break;
    case KEY_9:{
        Image screenshot(context_);
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    } break;
    default: break;
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{ eventData[KeyUp::P_KEY].GetInt() };

    if (pressedKeys_.Contains(key))
        pressedKeys_.Remove(key);
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };

    if (!pressedJoystickButtons_[joystickId].Contains(button))
        pressedJoystickButtons_[joystickId].Push(button);
}
void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };

    if (pressedJoystickButtons_[joystickId].Contains(button))
        pressedJoystickButtons_[joystickId].Remove(button);
}
void InputMaster::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickAxisMove::P_JOYSTICKID].GetInt() };
    int axis{ eventData[JoystickAxisMove::P_AXIS].GetInt() };
    float position{ eventData[JoystickAxisMove::P_POSITION].GetFloat() };

    if (axis == 0){
        leftStickPosition_[joystickId].x_ = position;
    } else if (axis == 1) {
        leftStickPosition_[joystickId].y_ = -position;
    } else if (axis == 2) {
        rightStickPosition_[joystickId].x_ = position;
    } else if (axis == 3) {
        rightStickPosition_[joystickId].y_ = -position;
    }
}

void InputMaster::CorrectForCameraYaw(Vector3& vec3)
{
    Vector3 camRot{MC->GetCamera()->GetRotation().EulerAngles()};
    vec3 = Quaternion(camRot.y_, Vector3::UP) * vec3;
}
