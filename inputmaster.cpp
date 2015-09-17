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

#include "inputmaster.h"
#include "bnbcam.h"
#include "player.h"

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context),
    masterControl_{masterControl},
    input_{GetSubsystem<Input>()}
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(JoystickButton::DPAD_UP)]    = MasterInputAction::UP;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(JoystickButton::DPAD_RIGHT)] = MasterInputAction::RIGHT;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(JoystickButton::DPAD_DOWN)]  = MasterInputAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(JoystickButton::DPAD_LEFT)]  = MasterInputAction::LEFT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(JoystickButton::CROSS)]      = MasterInputAction::CONFIRM;
    keyBindingsMaster_[KEY_ESC]   = buttonBindingsMaster_[static_cast<int>(JoystickButton::CIRCLE)]     = MasterInputAction::CANCEL;
    keyBindingsMaster_[KEY_PAUSE]  = buttonBindingsMaster_[static_cast<int>(JoystickButton::START)]      = MasterInputAction::PAUSE;
    keyBindingsMaster_[KEY_ESC]    = MasterInputAction::MENU;

    keyBindingsPlayer1_[KEY_W] = keyBindingsPlayer1_[KEY_UP]    = PlayerInputAction::UP;
    keyBindingsPlayer1_[KEY_D] = keyBindingsPlayer1_[KEY_RIGHT] = PlayerInputAction::RIGHT;
    keyBindingsPlayer1_[KEY_S] = keyBindingsPlayer1_[KEY_DOWN]  = PlayerInputAction::DOWN;
    keyBindingsPlayer1_[KEY_A] = keyBindingsPlayer1_[KEY_LEFT]  = PlayerInputAction::LEFT;
    keyBindingsPlayer1_[KEY_C] = keyBindingsPlayer1_[KEY_LSHIFT] = PlayerInputAction::RUN;
    keyBindingsPlayer1_[KEY_V] = keyBindingsPlayer1_[KEY_ALT]   = PlayerInputAction::JUMP;
    keyBindingsPlayer1_[KEY_B] = keyBindingsPlayer1_[KEY_SPACE] = PlayerInputAction::BUBBLE;

    keyBindingsPlayer2_[KEY_KP_8]    = PlayerInputAction::UP;
    keyBindingsPlayer2_[KEY_KP_6]    = PlayerInputAction::RIGHT;
    keyBindingsPlayer2_[KEY_KP_5]    = PlayerInputAction::DOWN;
    keyBindingsPlayer2_[KEY_KP_4]    = PlayerInputAction::LEFT;
    keyBindingsPlayer2_[KEY_RETURN] = PlayerInputAction::RUN;
    keyBindingsPlayer2_[KEY_RSHIFT]  = PlayerInputAction::JUMP;
    keyBindingsPlayer2_[KEY_RCTRL]   = PlayerInputAction::BUBBLE;

    SubscribeToEvent(E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_UPDATE, HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    InputActions activeActions{};
    //Convert key presses to actions
    for (int k = 0; k < static_cast<int>(pressedKeys_.Size()); k++){
        int key = pressedKeys_[k];
        if (keyBindingsMaster_.Contains(key)){
            MasterInputAction action = keyBindingsMaster_[key];
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        if (keyBindingsPlayer1_.Contains(key)){
            PlayerInputAction action = keyBindingsPlayer1_[key];
            if (!activeActions.player1_.Contains(action))
                activeActions.player1_.Push(action);
        }
        if (keyBindingsPlayer2_.Contains(key)){
            PlayerInputAction action = keyBindingsPlayer2_[key];
            if (!activeActions.player2_.Contains(action))
                activeActions.player2_.Push(action);
        }
    }
    HandleActions(activeActions);
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    if (actions.master_.Contains(MasterInputAction::MENU)) masterControl_->Exit();

    //Handle player actions
    Vector2 player1Movement
            = (Vector2::RIGHT *
            (actions.player1_.Contains(PlayerInputAction::RIGHT) -
            actions.player1_.Contains(PlayerInputAction::LEFT)))
            + (Vector2::UP *
               (actions.player1_.Contains(PlayerInputAction::UP) -
               actions.player1_.Contains(PlayerInputAction::DOWN)));
    Vector2 player2Movement
            = (Vector2::RIGHT *
               (actions.player2_.Contains(PlayerInputAction::RIGHT) -
               actions.player2_.Contains(PlayerInputAction::LEFT)))
            + (Vector2::UP *
               (actions.player2_.Contains(PlayerInputAction::UP) -
               actions.player2_.Contains(PlayerInputAction::DOWN)));

    if (masterControl_->PlayerIsAlive(BLIP) && masterControl_->PlayerIsHuman(BLIP)){
        Player* player1 = masterControl_->GetPlayer(BLIP);
        player1->Run(actions.player1_.Contains(PlayerInputAction::RUN));
        player1->SetMovement(player1Movement);
        if (actions.player1_.Contains(PlayerInputAction::JUMP)) player1->Jump();
    }
    if (masterControl_->PlayerIsAlive(BLUP) && masterControl_->PlayerIsHuman(BLUP)){
        Player* player2 = masterControl_->GetPlayer(BLUP);
        player2->Run(actions.player2_.Contains(PlayerInputAction::RUN));
        player2->SetMovement(player2Movement);
        if (actions.player2_.Contains(PlayerInputAction::JUMP)) player2->Jump();
    }
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    int key = eventData[KeyDown::P_KEY].GetInt();
    if (!pressedKeys_.Contains(key)) pressedKeys_.Push(key);

//    switch (key){
//    case KEY_ESC:{
//        masterControl_->Exit();
//    } break;
//    case KEY_9:{
//        Image screenshot(context_);
//        Graphics* graphics = GetSubsystem<Graphics>();
//        graphics->TakeScreenShot(screenshot);
//        //Here we save in the Data folder with date and time appended
//        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
//                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
//        //Log::Write(1, fileName);
//        screenshot.SavePNG(fileName);
//    } break;
//    default: break;
//    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{
    int key = eventData[KeyUp::P_KEY].GetInt();
    if (pressedKeys_.Contains(key)) pressedKeys_.Remove(key);
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{
    JoystickButton button = static_cast<JoystickButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt());
    if (!pressedJoystickButtons_.Contains(button)) pressedJoystickButtons_.Push(button);
}
void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{
    JoystickButton button = static_cast<JoystickButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt());
    if (pressedJoystickButtons_.Contains(button)) pressedJoystickButtons_.Remove(button);
}

