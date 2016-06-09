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

#ifndef COURSECAM_H
#define COURSECAM_H

#include <Urho3D/Urho3D.h>

#include "controllable.h"
#include "luckey.h"

using namespace Urho3D;

class BnBCam : public Controllable
{
    URHO3D_OBJECT(BnBCam, Controllable);
public:
    BnBCam(Context *context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    Quaternion GetRotation() const { return node_->GetRotation(); }
    Camera* GetCamera() const { return camera_.Get(); }
    virtual void DelayedStart();
private:
    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath_;

    float yaw_;
    float pitch_;
    float roll_;
    float yawDelta_;
    float pitchDelta_;
    float forceMultiplier;

    void SetupViewport();
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
};

#endif // TEMPLATECAM_H
