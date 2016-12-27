#include "block.h"
#include "resourcemaster.h"

HashMap<StringHash, StaticModelGroup*> Block::blockGroups_{};

void Block::RegisterObject(Context* context)
{
    context->RegisterFactory<Block>();
}

Block::Block(Context* context) : LogicComponent(context)
{

}

void Block::OnNodeSet(Node* node)
{ (void)node;

//    node_->CreateComponent<StaticModel>();

    RigidBody* rigidBody{ node_->CreateComponent<RigidBody>() };
    rigidBody->SetCollisionLayer(LAYER(0));
    rigidBody->SetFriction(0.8f);
    collider_ = node_->CreateComponent<CollisionShape>();
}

void Block::Initialize(Model* model, Material* material)
{
//    node_->GetComponent<StaticModel>()->SetModel(model);
//    node_->GetComponent<StaticModel>()->

    StringHash modelNameHash{ model->GetNameHash() };

    collider_->SetTriangleMesh(model);

    if (!blockGroups_.Contains(modelNameHash)) {

        blockGroups_[modelNameHash] = MC->GetScene()->CreateComponent<StaticModelGroup>();
        blockGroups_[modelNameHash]->SetModel(model);
        blockGroups_[modelNameHash]->SetCastShadows(true);
        blockGroups_[modelNameHash]->SetMaterial(material);
    }

    blockGroups_[modelNameHash]->AddInstanceNode(node_);
}

void Block::Update(float timeStep)
{
}




