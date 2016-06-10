#ifndef WIND_H
#define WIND_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Wind : public LogicComponent
{
    URHO3D_OBJECT(Wind, LogicComponent);
    static void RegisterObject(Context *context);
public:
    Wind(Context* context);
    void Update(float timeStep);
};

#endif // WIND_H
