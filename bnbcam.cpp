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

#include "fish.h"
#include "bnbcam.h"

BnBCam::BnBCam(Context *context):
    Controllable(context)
{
}
void BnBCam::RegisterObject(Context* context)
{
    context->RegisterFactory<BnBCam>();
}

void BnBCam::OnNodeSet(Node *node)
{(void)node;

    camera_ = node_->CreateComponent<Camera>();
    camera_->SetFarClip(1024.0f);
    camera_->SetFov(60.0f);
    node_->SetPosition(Vector3(0.0f, 7.0f, -10.0f));
    node_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone{node_->CreateComponent<Zone>()};
    zone->SetAmbientColor(Color(0.2f, 0.23f, 0.42f));


}
void BnBCam::DelayedStart()
{
    SetupViewport();

}


void BnBCam::SetupViewport()
{

}
