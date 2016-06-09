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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H


#include <Urho3D/Urho3D.h>
#include "luckey.h"

using namespace Urho3D;

class InputMaster;
class BnBCam;
class Fish;

typedef struct GameWorld {

    SharedPtr<Node> voidNode;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;

} GameWorld;

typedef struct HitInfo {

    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;

} HitInfo;

enum CharacterID { BLIP = 1, BLUP };

namespace {
StringHash const N_VOID = StringHash("Void");
StringHash const N_CURSOR = StringHash("Cursor");
}

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
public:
    MasterControl(Context* context);

    GameWorld world_;

    Vector<int> GetPlayers() const {
        Vector<int> players;
        players.Push(1);
        players.Push(2);
        return players;
    }

    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void Exit();


    Scene* GetScene() { return scene_; }

    Fish* GetBlip() { return blip_.Get(); }
    Fish* GetBlup() { return blup_.Get(); }

    float Sine(const float freq, const float min, const float max, const float shift = 0.0f);
    float Cosine(const float freq, const float min, const float max, const float shift = 0.0f);
private:
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;

    SharedPtr<BnBCam> camera_;
    SharedPtr<Scene> scene_;

    SharedPtr<Fish> blip_;
    SharedPtr<Fish> blup_;

    void CreateConsoleAndDebugHud();
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void CreatePlatform(const Vector3 pos);
    void UpdateCursor(float timeStep);
    bool CursorRayCast(float maxDistance, PODVector<RayQueryResult> &hitResults);

    bool paused_;
    bool drawDebug_{false};
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
};

#endif // MASTERCONTROL_H
