#include "rage.h"


Rage::Rage(Context* context) : LogicComponent(context),
    anger_{0.0f},
    cooldown_{5.0f},
    sinceAngered_{cooldown_}
{
}

void Rage::RegisterObject(Context *context)
{
    context->RegisterFactory<Rage>();
}

void Rage::Update(float timeStep)
{
    if (anger_ == 0.0f)
        return;


    sinceAngered_ += timeStep;
    anger_ = Clamp(1.0f - (Max(sinceAngered_, 1.0e-9) / cooldown_), 0.0f, 1.0f);
}
