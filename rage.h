#ifndef RAGE_H
#define RAGE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Rage : public LogicComponent
{
    URHO3D_OBJECT(Rage, LogicComponent);
public:
    Rage(Context* context);
    static void RegisterObject(Context* context);
    virtual void Update(float timeStep);

    void SetCooldown(float time) { cooldown_ = time; }
    float GetAnger() const { return anger_; }
    void FillAnger() { anger_ = 1.0f; sinceAngered_ = 0.0f;}
private:
    float anger_;

    float cooldown_;
    float sinceAngered_;
};

#endif // RAGE_H
