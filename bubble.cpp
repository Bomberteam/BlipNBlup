#include "bubble.h"


Bubble::Bubble(Context* context) : LogicComponent(context)
{
}
void Bubble::RegisterObject(Context* context)
{
    context->RegisterFactory<Bubble>();
}

void Bubble::OnNodeSet(Node *node)
{ (void)node;

    model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(CACHE->GetResource<Model>("Models/Bubble.mdl"));
    model_->SetMaterial(CACHE->GetResource<Material>("Materials/Bubble.xml"));

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetCollisionMask(1);
    rigidBody_->SetMass(0.1f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetGravityOverride(Vector3::UP * 9.0f);

    collider_ = node_->CreateComponent<CollisionShape>();
    collider_->SetSphere(1.0f);
}
void Bubble::Update(float timeStep)
{
    node_->Rotate(Quaternion(13.0f * timeStep, 23.0f * timeStep, 34.0f * timeStep));
}
