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

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context),
    masterControl_{masterControl},
    input_{GetSubsystem<Input>()}
{
    SubscribeToEvent(E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, HANDLER(InputMaster, HandleJoyButtonUp));

    SubscribeToEvent(E_UPDATE, HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    int key = eventData[KeyDown::P_KEY].GetInt();
    if (!pressedKeys_.Contains(key)) pressedKeys_.Push(key);

    switch (key){
    case KEY_ESC:{
        masterControl_->Exit();
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
{
    int key = eventData[KeyUp::P_KEY].GetInt();
    if (pressedKeys_.Contains(key)) pressedKeys_.Remove(key);
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{
    JoystickButton button = static_cast<JoystickButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt());
    if (!pressedJoyButtons_.Contains(button)) pressedJoyButtons_.Push(button);
}
void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{
    JoystickButton button = static_cast<JoystickButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt());
    if (pressedJoyButtons_.Contains(button)) pressedJoyButtons_.Remove(button);
}
