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

#include "catchable.h"
#include "bubble.h"

Catchable::Catchable(Context* context) : LogicComponent(context),
    catched_{false}
{

}

void Catchable::RegisterObject(Context* context)
{
    context->RegisterFactory<Catchable>();
}

bool Catchable::Catch(Bubble* bubble)
{
    if (!catched_){
        catched_ = true;
        if (node_->HasComponent<RigidBody>()){
            node_->GetComponent<RigidBody>()->SetEnabled(false);
        }
        node_->SetParent(bubble->GetNode());
        node_->SetPosition(Vector3::ZERO);
        return true;
    }
    return false;
}
void Catchable::Release()
{
    node_->SetParent(MC->GetScene());
    catched_ = false;
}
