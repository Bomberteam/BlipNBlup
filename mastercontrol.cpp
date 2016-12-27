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

#include "mastercontrol.h"
#include "bnbcam.h"
#include "block.h"
#include "fish.h"
#include "bubble.h"
#include "wind.h"
#include "catchable.h"
#include "rage.h"
#include "wasp.h"
#include "resourcemaster.h"
#include "inputmaster.h"
#include "castmaster.h"
#include "effectmaster.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_{false}
{
    BnBCam::RegisterObject(context_);
    Block::RegisterObject(context_);
    Fish::RegisterObject(context_);
    Bubble::RegisterObject(context_);
    Wind::RegisterObject(context_);

    Catchable::RegisterObject(context_);
    Rage::RegisterObject(context_);
    Wasp::RegisterObject(context_);
}

void MasterControl::Setup()
{
    engineParameters_[EP_WINDOW_TITLE] = "Blip 'n Blup: Skyward Adventures";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"blipnblup.log";
    engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;Resources";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";

//    engineParameters_[EP_FULL_SCREEN] = false;
}
void MasterControl::Start()
{
    ENGINE->SetMaxFps(100);
    SetRandomSeed(TIME->GetSystemTime());

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new ResourceMaster(context_));
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new CastMaster(context_));
    context_->RegisterSubsystem(new EffectMaster(context_));

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
    scene_->GetComponent<PhysicsWorld>()->SetGravity(Vector3::DOWN * 42.0f);

    //Add sky
    Node* skyNode{ scene_->CreateChild("Sky") };
    Skybox* skybox{ skyNode->CreateComponent<Skybox>() };
    skybox->SetModel(RM->GetModel("Box"));
    skybox->SetMaterial(RM->GetMaterial("Skybox"));
    skybox->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.5f, 0.6f, 1.0f));

    //Add ground
    /*Node* groundNode{scene_->CreateChild("Ground")};
    groundNode->SetScale(1.0f);
    groundNode->Translate(Vector3(-1.0f, -0.5f, 0.0f));
    StaticModel* groundModel{groundNode->CreateComponent<StaticModel>()};
    Model* phyisicalModel{RM->GetModel("Level1_physical")};
    groundModel->SetModel(phyisicalModel);
    groundModel->SetMaterial(RM->GetMaterial("VCol"));
    groundModel->SetCastShadows(true);
    RigidBody* groundBody{groundNode->CreateComponent<RigidBody>()};
    groundBody->SetCollisionLayer(LAYER(0));
    groundBody->SetFriction(2.3f);
    CollisionShape* groundCollider{groundNode->CreateComponent<CollisionShape>()};
    groundCollider->SetTriangleMesh(phyisicalModel);//SetBox(Vector3(23.0f, 2.0f, 23.0f), Vector3::DOWN);*/

//    world_

    LoadBlockMap("Resources/TestMap.xml");

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{ scene_->CreateChild("DirectionalLight") };
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light{ lightNode->CreateComponent<Light>() };
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.9f);
    light->SetShadowIntensity(0.3f);
    light->SetColor(Color(0.95f, 0.9f, 0.85f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000023f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create fish
    Node* blipNode{ scene_->CreateChild("Blip") };
    blip_ = blipNode->CreateComponent<Fish>();
    GetSubsystem<InputMaster>()->SetPlayerControl(1, blip_);

    Node* blupNode{ scene_->CreateChild("Blup") };
    blup_ = blupNode->CreateComponent<Fish>();
    blup_->BecomeBlup();
    GetSubsystem<InputMaster>()->SetPlayerControl(2, blup_);

    //Create wasps
    for (int w{0}; w < 10; ++w) {
        Node* waspNode{scene_->CreateChild("Wasp")};
        waspNode->CreateComponent<Wasp>();
    }

    //Create camera
    Node* camNode{ scene_->CreateChild("Camera") };
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
    float phase{SinePhase(freq, shift)};
    float add{0.5f * (min + max)};
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    return Sine(freq, min, max, shift + 0.25f);
}
float MasterControl::SinePhase(float freq, float shift)
{
    return M_PI * 2.0f * (freq * scene_->GetElapsedTime() + shift);
}

void MasterControl::LoadBlockMap(String fileName) {

    if (!FILES->FileExists(fileName))
        return;

    File mapFile{ MC->GetContext(), fileName, FILE_READ };

    XMLFile* mapXML{ new XMLFile(MC->GetContext()) };
    mapXML->BeginLoad(mapFile);
    mapFile.Close();
    XMLElement mapElem{ mapXML->GetRoot("blockmap") };

    int mapWidth{ mapElem.GetInt("map_width") };
    int mapHeight{ mapElem.GetInt("map_height") };
    int mapDepth{ mapElem.GetInt("map_deoth") };

    Vector3 blockSize{ mapElem.GetVector3("block_size") };

    HashMap<int, HashMap<int, String>> blockSets;

    XMLElement blockSetRefElem{ mapElem.GetChild("blockset") };

    while(blockSetRefElem) {
        int blockSetId{ blockSetRefElem.GetInt("id") };

        File blockSetFile{ MC->GetContext(), blockSetRefElem.GetAttribute("name"), FILE_READ };

        XMLFile* blockSetXML{ new XMLFile(MC->GetContext()) };
        blockSetXML->BeginLoad(blockSetFile);
        blockSetFile.Close();
        XMLElement blockSetElem{ blockSetXML->GetRoot("blockset") };

        XMLElement blockElem{ blockSetElem.GetChild("block") };
        while (blockElem) {

            blockSets[blockSetId][blockElem.GetInt("id")] = blockElem.GetAttribute("model");

            blockElem = blockElem.GetNext("block");
        }

        blockSetRefElem = blockSetRefElem.GetNext("blockset");
    }

    XMLElement blockElem{ mapElem.GetChild("gridblock") };
    while (blockElem) {
        Node* blockNode{ scene_->CreateChild("Block") };
        blockNode->SetPosition(Vector3((blockElem.GetInt("x") - mapWidth / 2) * blockSize.x_,
                                       (blockElem.GetInt("y") - mapHeight/ 2) * blockSize.y_,
                                       (blockElem.GetInt("z") - mapDepth / 2) * blockSize.z_));
        blockNode->SetRotation(blockElem.GetQuaternion("rot"));

        Block* block{ blockNode->CreateComponent<Block>() };
        block->Initialize(CACHE->GetResource<Model>(blockSets[blockElem.GetInt("set")][blockElem.GetInt("block")]),
                          RM->GetMaterial("VCol"));

        blockElem = blockElem.GetNext("gridblock");
    }
}
