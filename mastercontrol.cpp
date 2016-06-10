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

#include "mastercontrol.h"
#include "bnbcam.h"
#include "fish.h"
#include "bubble.h"
#include "inputmaster.h"
#include "castmaster.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_{false}
{
    BnBCam::RegisterObject(context_);
    Fish::RegisterObject(context_);
    Bubble::RegisterObject(context_);
}

void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "Blip 'n Blup: Skyward Adventures";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"blipnblup.log";
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
}
void MasterControl::Start()
{
    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new CastMaster(context_));

    INPUT->SetMouseMode(MM_FREE);

    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();

    Sound* music{CACHE->GetResource<Sound>("Resources/Music/XL Ant - No-Trace Land.ogg")};
    music->SetLooped(true);
    SoundSource* musicSource{scene_->CreateComponent<SoundSource>()};
    musicSource->SetGain(0.32f);
    musicSource->SetSoundType(SOUND_MUSIC);
//    musicSource->Play(music);
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}
void MasterControl::Exit()
{
    engine_->Exit();
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));

}

void MasterControl::CreateConsoleAndDebugHud()
{
    Console* console{engine_->CreateConsole()};
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    DebugHud* debugHud{engine_->CreateDebugHud()};
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
//    cache_ = GetSubsystem<ResourceCache>();
//    UI* ui = GetSubsystem<UI>();

//    world_.cursor.uiCursor = new Cursor(context_);
//    world_.cursor.uiCursor->SetVisible(false);
//    ui->SetCursor(world_.cursor.uiCursor);
//    world_.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);
}

void MasterControl::CreateScene()
{
    scene_ = new Scene(context_);

    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    scene_->CreateComponent<PhysicsWorld>();
    scene_->GetComponent<PhysicsWorld>()->SetGravity(Vector3::DOWN*42.0f);

    //Add sky
    Node* skyNode{scene_->CreateChild("Sky")};
    Skybox* skybox{skyNode->CreateComponent<Skybox>()};
    skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Skybox.xml"));
    skybox->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.5f, 0.6f, 1.0f));

    //Add ground
    Node* groundNode{scene_->CreateChild("Ground")};
    groundNode->SetScale(1.0f);
    groundNode->Translate(Vector3(-1.0f, -0.5f, 0.0f));
    StaticModel* groundModel{groundNode->CreateComponent<StaticModel>()};
    Model* phyisicalModel{CACHE->GetResource<Model>("Resources/Models/Level1_physical.mdl")};
    groundModel->SetModel(phyisicalModel);
    groundModel->SetMaterial(CACHE->GetResource<Material>("Resources/Materials/VCol.xml"));
    groundModel->SetCastShadows(true);
    RigidBody* groundBody{groundNode->CreateComponent<RigidBody>()};
    groundBody->SetCollisionLayer(1);
    groundBody->SetFriction(0.8f);
    CollisionShape* groundCollider{groundNode->CreateComponent<CollisionShape>()};
    groundCollider->SetTriangleMesh(phyisicalModel);//SetBox(Vector3(23.0f, 2.0f, 23.0f), Vector3::DOWN);

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{scene_->CreateChild("DirectionalLight")};
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light{lightNode->CreateComponent<Light>()};
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.9f);
    light->SetColor(Color(0.8f, 0.95f, 0.9f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000125f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create fish
    Node* blipNode{scene_->CreateChild("Blip")};
    blip_ = blipNode->CreateComponent<Fish>();
    GetSubsystem<InputMaster>()->SetPlayerControl(1, blip_);

    Node* blupNode{scene_->CreateChild("Blup")};
    blup_ = blupNode->CreateComponent<Fish>();
    blup_->BecomeBlup();
    GetSubsystem<InputMaster>()->SetPlayerControl(2, blup_);

    //Create camera
    Node* camNode{scene_->CreateChild("Camera")};
    camera_ = camNode->CreateComponent<BnBCam>();

}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;
    if (INPUT->GetKeyPress(KEY_SPACE))
        drawDebug_ = !drawDebug_;


    if (drawDebug_)
    {
        scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
    }
}





float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{freq * scene_->GetElapsedTime() + shift};
    float add{0.5f * (min + max)};
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    float phase{freq * scene_->GetElapsedTime() + shift};
    float add{0.5f * (min + max)};
    return LucKey::Cosine(phase) * 0.5f * (max - min) + add;
}
