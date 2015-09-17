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
#include "helper.h"

using namespace Urho3D;

class InputMaster;
class BnBCam;
class Player;

typedef struct GameWorld
{
    SharedPtr<BnBCam> camera;
    SharedPtr<Scene> scene_;
    SharedPtr<Node> voidNode;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;
} GameWorld;

typedef struct HitInfo
{
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
    OBJECT(MasterControl);
    friend class InputMaster;
public:
    MasterControl(Context* context);
    GameWorld world_;
    SharedPtr<ResourceCache> cache_;
    SharedPtr<Graphics> graphics_;

    Player* GetPlayer(int id = 1);
    bool PlayerIsAlive(int id = 1);
    bool PlayerIsHuman(int id = 1);

    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void Exit();
private:
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;

    Player* blip_;
    Player* blup_;

    void CreateConsoleAndDebugHud();
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void CreatePlatform(const Vector3 pos);
    void UpdateCursor(float timeStep);
    bool CursorRayCast(float maxDistance, PODVector<RayQueryResult> &hitResults);

    bool paused_;
};

#endif // MASTERCONTROL_H
