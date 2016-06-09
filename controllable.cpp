#include "controllable.h"

Controllable::Controllable(Context* context) : LogicComponent(context),
    controlled_{false},
    move_{},
    aim_{}
{

}

void Controllable::SetActions(std::bitset<4> actions)
{
    if (actions == actions_)
        return;
    else
        for (int i{0}; i < actions.size(); ++i){
            if (actions[i]!=actions_[i]){
                if (actions[i])
                    HandleAction(i);
                actions_[i]=actions[i];
            }
        }
}

void Controllable::HandleAction(int actionId)
{

}





