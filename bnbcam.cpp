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

BnBCam::BnBCam(Context *context, MasterControl *masterControl):
    Object(context)
{
    masterControl_ = masterControl;

    rootNode_ = masterControl_->world_.scene_->CreateChild("Camera");
    camera_ = rootNode_->CreateComponent<Camera>();
    camera_->SetFarClip(1024.0f);
    camera_->SetFov(60.0f);
    rootNode_->SetPosition(Vector3(0.0f, 7.0f, -10.0f));
    rootNode_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone = rootNode_->CreateComponent<Zone>();
    zone->SetAmbientColor(Color(0.2f, 0.23f, 0.42f));

    SetupViewport();

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(BnBCam, HandleSceneUpdate));
}

void BnBCam::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, masterControl_->world_.scene_, camera_));
    viewport_ = viewport;

    Renderer* renderer = GetSubsystem<Renderer>();
    effectRenderPath_ = viewport_->GetRenderPath()->Clone();
    effectRenderPath_->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath_->SetEnabled("FXAA3", true);
    viewport_->SetRenderPath(effectRenderPath_);
    renderer->SetViewport(0, viewport_);
}

void BnBCam::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    float timeStep = eventData[SceneUpdate::P_TIMESTEP].GetFloat();
    Vector3 playerPosition = masterControl_->GetPlayer()->GetPosition();
    Vector3 playerDeltaPos = playerPosition - rootNode_->GetWorldPosition();
    Vector3 direction = rootNode_->GetDirection();
    float targetAngle = direction.Angle(playerDeltaPos);
    Quaternion newRot{};
    newRot.FromLookRotation(playerDeltaPos);
    rootNode_->SetRotation(rootNode_->GetRotation().Slerp(newRot, Clamp(timeStep * targetAngle * 0.23f, 0.0f, 1.0f)));

    rootNode_->Translate((timeStep * Clamp(playerDeltaPos.Length()-23.0f, -5.0f, 5.0f)) * Vector3::FORWARD * 5.0f, TS_LOCAL);
    float targetAltitude = playerPosition.y_ + 12.0f;
    float altitudeDelta = targetAltitude - rootNode_->GetPosition().y_;
    rootNode_->Translate((timeStep * altitudeDelta) * Vector3::UP, TS_WORLD);
}

Vector3 BnBCam::GetWorldPosition()
{
    return rootNode_->GetWorldPosition();
}

Quaternion BnBCam::GetRotation()
{
    return rootNode_->GetRotation();
}

