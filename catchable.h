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

#ifndef CATCHABLE_H
#define CATCHABLE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Bubble;

class Catchable : public LogicComponent
{
    URHO3D_OBJECT(Catchable, LogicComponent);
public:
    Catchable(Context* context);
    static void RegisterObject(Context *context);
    bool IsCaught() const { return caught_; }
    bool Catch(Bubble* bubble);
    void Release();
private:
    bool caught_;
};

URHO3D_EVENT(E_CATCH, Catch)
{
    URHO3D_PARAM(P_NODE, Node);
}
URHO3D_EVENT(E_RELEASE, Release){}

#endif // CATCHABLE_H
