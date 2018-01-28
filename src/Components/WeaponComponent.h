#pragma once

#include "entityx.h"

struct WeaponComponent {
	WeaponComponent() {
        hasTarget = false;
        projectile = NULL;
        distanceToTarget = 0.0f;
    }
	WeaponComponent(OPrendererEntity* proj) {
        hasTarget = false;
        projectile = proj;
        distanceToTarget = 0.0f;
    }

	entityx::Entity::Id targetId;
    bool hasTarget;
    OPrendererEntity* projectile;
    OPfloat distanceToTarget;
};