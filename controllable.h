#ifndef CONTROLLABLE_H
#define CONTROLLABLE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"
#include <bitset>

class Controllable : public LogicComponent
{
    friend class InputMaster;
    URHO3D_OBJECT(Controllable, LogicComponent);
public:
    Controllable(Context* context);
    void SetActions(std::bitset<4> actions);
protected:
    bool controlled_;
    Vector3 move_;
    Vector3 aim_;
    std::bitset<4> actions_;

    void ResetInput() { move_ = aim_ = Vector3::ZERO; actions_.reset(); }
    virtual void HandleAction(int actionId);
};

#endif // CONTROLLABLE_H
