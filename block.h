#ifndef BLOCK_H
#define BLOCK_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"

class Block : public LogicComponent
{
    URHO3D_OBJECT(Block, LogicComponent);
public:
    static HashMap<StringHash, StaticModelGroup*> blockGroups_;

    Block(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);
    void Initialize(Model* model, Material* material);
private:
    CollisionShape* collider_;
};

#endif // BLOCK_H
