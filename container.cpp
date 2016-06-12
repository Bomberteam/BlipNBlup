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

#include "container.h"
#include "catchable.h"

Container::Container(Context* context) : LogicComponent(context),
  empty_{true},
  escapable_{true},
  containTime_{1.0f},
  sinceContain_{containTime_}
{
    SetUpdateEventMask(USE_UPDATE);
}

void Container::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateComponent<StaticModel>();
    rigidBody_ = node_->CreateComponent<RigidBody>();
    collider_ = node_->CreateComponent<CollisionShape>();
}
void Container::Update(float timeStep)
{
    if (empty_ || !escapable_)
        return;

    sinceContain_ += timeStep;
    if (sinceContain_ > containTime_)
        Release();
}

bool Container::Contain(Catchable* catchable)
{
    if (catchable->Catch(this)){
        empty_ = false;
        sinceContain_ = 0.0f;

        catchable_ = catchable;
        containTime_ = catchable_->GetReleaseTime();
        return true;
    } else return false;
}
void Container::Release()
{
    if (!empty_){
        empty_ = true;
        catchable_->Release();
        catchable_ = nullptr;

        node_->SetEnabled(false);
    }
}
