#include "wind.h"


Wind::Wind(Context* context) : LogicComponent(context)
{

}

void Wind::RegisterObject(Context *context)
{
    context->RegisterFactory<Wind>();
}

void Wind::Update(float timeStep)
{
    RigidBody* rigidBody{node_->GetComponent<RigidBody>()};
    if (rigidBody){
        rigidBody->ApplyForce(Vector3::LEFT *
                              MC->Sine(0.5f, -10.0f, 50.0f, 0.5f * node_->GetPosition().y_) * timeStep);
    }
}

