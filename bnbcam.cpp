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

#include "fish.h"
#include "bnbcam.h"
#include "castmaster.h"

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
    camera_->SetFov(90.0f);
    node_->SetPosition(Vector3(0.0f, 7.0f, -10.0f));
    node_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone{ node_->CreateComponent<Zone>() };
    zone->SetAmbientColor(Color(0.5f, 0.6f, 0.8f));

    SetupViewport();

}

void BnBCam::SetupViewport()
{

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport* viewport{ new Viewport(context_, MC->GetScene(), camera_) };

    RenderPath* effectRenderPath{viewport->GetRenderPath()};
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);

    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.42f);
    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.9f, 0.23f));
    effectRenderPath->SetEnabled("BloomHDR", true);

    viewport->SetRenderPath(effectRenderPath);
    RENDERER->SetViewport(0, viewport);
}

void BnBCam::Update(float timeStep)
{
    float t{ Clamp( 235.0f * timeStep, 0.0f, 1.0f) };

    Vector3 targetPos{ MC->GetBlip()->GetNode()->GetPosition() };
    float targetDistance{ LucKey::Distance(node_->GetPosition(), targetPos) };
    float distance{ Lerp(targetDistance, 6.0f, t) };
    float minAltDelta{ 2.3f };
    float maxAltDelta{ 5.5f };

    float altDelta{ node_->GetPosition().y_ - targetPos.y_ };
    if (altDelta < minAltDelta) {
        node_->Translate(Vector3::UP * (minAltDelta - altDelta) * t, TS_WORLD);
    } else if (altDelta > maxAltDelta) {
        node_->Translate(Vector3::UP * (maxAltDelta - altDelta) * t, TS_WORLD);
    }

    node_->LookAt(targetPos);

    PhysicsRaycastResult result{};
    Ray ray{ targetPos, (node_->GetPosition() - targetPos).Normalized() };
    float closest{ distance };
    if (GetSubsystem<CastMaster>()->PhysicsSphereCast(result, ray, 0.23f, distance, LAYER(0))) {

        float hitDistance{ LucKey::Distance(result.position_, node_->GetPosition()) };
        if (hitDistance < closest && hitDistance > 0.5f) {
            closest = hitDistance;
        }
    }

    distance = closest;
    if (targetDistance != distance) {
        node_->Translate(Vector3::FORWARD * (targetDistance - distance) * t);
    }
}
