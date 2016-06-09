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
#include "inputmaster.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_{false}
{
    Fish::RegisterObject(context_);
    BnBCam::RegisterObject(context_);
}

void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "Blip 'n Blup: Skyward Adventures";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"blipnblup.log";
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
}
void MasterControl::Start()
{
    context_->RegisterSubsystem(new InputMaster(context_));

    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();

    Sound* music{CACHE->GetResource<Sound>("Resources/Music/XL Ant - No-Trace Land.ogg")};
    music->SetLooped(true);
    SoundSource* musicSource{world_.scene_->CreateComponent<SoundSource>()};
    musicSource->SetGain(0.32f);
    musicSource->SetSoundType(SOUND_MUSIC);
    musicSource->Play(music);
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
    world_.scene_ = new Scene(context_);

    world_.scene_->CreateComponent<Octree>();
    world_.scene_->CreateComponent<DebugRenderer>();

    PhysicsWorld* physicsWorld = world_.scene_->CreateComponent<PhysicsWorld>();
    physicsWorld->SetGravity(Vector3::DOWN*42.0f);

    //Add sky
    Node* skyNode{world_.scene_->CreateChild("Sky")};
    Skybox* skybox{skyNode->CreateComponent<Skybox>()};
    skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Skybox.xml"));

    //Add ground
    Node* groundNode{world_.scene_->CreateChild("Ground")};
    groundNode->SetScale(1.0f);
    groundNode->Translate(Vector3(-1.0f, -0.5f, 0.0f));
    StaticModel* groundModel{groundNode->CreateComponent<StaticModel>()};
    Model* phyisicalModel{CACHE->GetResource<Model>("Resources/Models/Level1_physical.mdl")};
    groundModel->SetModel(phyisicalModel);
    groundModel->SetMaterial(CACHE->GetResource<Material>("Resources/Materials/VCol.xml"));
    groundModel->SetCastShadows(true);
    RigidBody* groundBody{groundNode->CreateComponent<RigidBody>()};
    groundBody->SetFriction(0.8f);
    CollisionShape* groundCollider{groundNode->CreateComponent<CollisionShape>()};
    groundCollider->SetTriangleMesh(phyisicalModel);//SetBox(Vector3(23.0f, 2.0f, 23.0f), Vector3::DOWN);

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{world_.scene_->CreateChild("DirectionalLight")};
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
    Node* blipNode{world_.scene_->CreateChild("Blip")};
    blip_ = blipNode->CreateComponent<Fish>();
    GetSubsystem<InputMaster>()->SetPlayerControl(1, blip_);

    Node* blupNode{world_.scene_->CreateChild("Blup")};
    blup_ = blupNode->CreateComponent<Fish>();
    blup_->BecomeBlup();
    GetSubsystem<InputMaster>()->SetPlayerControl(2, blup_);

    //Create camera
    Node* camNode{world_.scene_->CreateChild("Camera")};
    world_.camera_ = camNode->CreateComponent<BnBCam>();

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport* viewport(new Viewport(context_, world_.scene_, world_.camera_->GetCamera()));

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
