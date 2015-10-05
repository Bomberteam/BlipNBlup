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

DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_{false}
{
}

void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "Blip 'n Blup: Skyward Adventures";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"blipnblup.log";
}
void MasterControl::Start()
{
    new InputMaster(context_, this);
    cache_ = GetSubsystem<ResourceCache>();
    graphics_ = GetSubsystem<Graphics>();
    renderer_ = GetSubsystem<Renderer>();

    defaultStyle_ = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();

    Sound* music = cache_->GetResource<Sound>("Resources/Music/XL Ant - No-Trace Land.ogg");
    music->SetLooped(true);
    SoundSource* musicSource = world_.scene_->CreateComponent<SoundSource>();
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
    SubscribeToEvent(E_UPDATE, HANDLER(MasterControl, HandleUpdate));
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(MasterControl, HandleUpdate));
}

void MasterControl::CreateConsoleAndDebugHud()
{
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    cache_ = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    world_.cursor.uiCursor = new Cursor(context_);
    world_.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world_.cursor.uiCursor);
    world_.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);
}

void MasterControl::CreateScene()
{
    world_.scene_ = new Scene(context_);

    world_.scene_->CreateComponent<Octree>();
    world_.scene_->CreateComponent<DebugRenderer>();

    PhysicsWorld* physicsWorld = world_.scene_->CreateComponent<PhysicsWorld>();
    physicsWorld->SetGravity(Vector3::DOWN*42.0f);

    //Add sky
    Node* skyNode = world_.scene_->CreateChild("Sky");
    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache_->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache_->GetResource<Material>("Materials/Skybox.xml"));

    //Add ground
    Node* groundNode = world_.scene_->CreateChild("Ground");
    groundNode->SetScale(5.0f);
    StaticModel* groundModel = groundNode->CreateComponent<StaticModel>();
    groundModel->SetModel(cache_->GetResource<Model>("Resources/Models/Ground.mdl"));
    groundModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/VCol.xml"));
    groundModel->SetCastShadows(true);
    RigidBody* groundBody = groundNode->CreateComponent<RigidBody>();
    groundBody->SetFriction(0.8f);
    CollisionShape* groundCollider = groundNode->CreateComponent<CollisionShape>();
    groundCollider->SetBox(Vector3(23.0f, 2.0f, 23.0f), Vector3::DOWN);

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = world_.scene_->CreateChild("DirectionalLight");
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.9f);
    light->SetColor(Color(0.8f, 0.95f, 0.9f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000125f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create some Fish
    for (int k = 0; k < 0; k++){
        Node* objectNode = world_.scene_->CreateChild("Fish");
        objectNode->SetPosition((-5.0f + 3.0f*k) * Vector3::RIGHT + (k%2) * 5.0f * Vector3::FORWARD);
        objectNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        objectNode->SetScale(0.8f);

        AnimatedModel* animatedModel = objectNode->CreateComponent<AnimatedModel>();
        if (Random(2)) {
            animatedModel->SetModel(cache_->GetResource<Model>("Resources/Models/Blip.mdl"));
        } else {
            animatedModel->SetModel(cache_->GetResource<Model>("Resources/Models/Blup.mdl"));
            animatedModel->SetMorphWeight(1,1.0f);
        }
        animatedModel->SetMorphWeight(0,Random(0.0f, 0.5f));
        animatedModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/VCol.xml"));
        animatedModel->SetCastShadows(true);

        AnimationController* animCtrl = objectNode->CreateComponent<AnimationController>();
        animCtrl->PlayExclusive("Resources/Models/Walk.ani", 0, true);
        animCtrl->SetSpeed("Resources/Models/Walk.ani", Random(1.0f,2.0f));
    }

    //Create a player
    blip_ = new Fish(context_, this, BLIP);
    blip_->rootNode_->SetPosition(Vector3::LEFT*7.0f);
    blup_ = new Fish(context_, this, BLUP);
    blup_->rootNode_->SetPosition(Vector3::RIGHT*3.0f);
    blup_->human_ = false;

    //Create camera
    world_.camera = new BnBCam(context_, this);
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
}

Fish *MasterControl::GetPlayer(int id) {
    assert(id == 1 || id == 2);
    switch (id){
    case 1: return blip_; break;
    case 2: return blup_; break;
    default: return nullptr; break;}
}
bool MasterControl::PlayerIsAlive(int id) {
    assert(id == 1 || id == 2);
    Fish* player = GetPlayer(id);
    if (player != nullptr) return player->IsAlive();
    else return false;
}
bool MasterControl::PlayerIsHuman(int id) {
    assert(id == 1 || id == 2);
    Fish* player = GetPlayer(id);
    if (player != nullptr) return player->IsHuman();
    else return false;
}






































