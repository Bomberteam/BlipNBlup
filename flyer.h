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

#ifndef FLYER_H
#define FLYER_H

#include <Urho3D/Urho3D.h>
#include "controllable.h"

class Flyer : public Controllable
{
    URHO3D_OBJECT(Flyer, Controllable);
public:
    Flyer(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

protected:
    float maxFlySpeed_;
    float flyThrust_;
    float flyHeight_;

    float chargeForce_;
    float chargeInterval_;

    float altitude_;
private:
    void CorrectAltitude();
};

#endif // FLYER_H
